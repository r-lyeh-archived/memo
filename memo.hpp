/*
 * Simple abstract factory class.
 * Copyright (c) 2014 Mario 'rlyeh' Rodriguez

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.

 * - rlyeh
 */

#pragma once

#include <cstdint>

#include <algorithm>
#include <map>
#include <typeinfo>

namespace memo
{
    class any;

    template< typename K >
    using factory = std::map< K, memo::any >;
}

/* API details following */

/*
 * (C) Copyright Mario "rlyeh" Rodriguez 2010
 * (C) Copyright Christopher Diggins 2005
 * (C) Copyright Pablo Aguilar 2005
 * (C) Copyright Kevlin Henney 2001
 *
 * Changelist: (27/Oct/2010)
 * - Changed all void * type to uintptr_t (rlyeh)
 * - Added support for pointer to method class (rlyeh)
 * - Removed stdio.h dependancy by changing NULL to 0 (rlyeh)
 * - Fixed the missing return value in the swap() function (rlyeh)
 * - Removed operator = that was causing some leaks (rlyeh - to check)
 * - Replaced include guards with #pragma once (rlyeh)
 * - Rename cdiggins:: namespace to memo:: (my own library :o) (rlyeh)
 * - Renamed cast<T>() method to as<T>() to conform my boo library convenion (rlyeh)
 * - Reformatted (rlyeh)
 * - Removed implicit (and dangerous) casting support (rlyeh)
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

namespace memo
{
    using any_voidptr = std::uintptr_t *;

    namespace any_detail {
        // function pointer table

        struct fxn_ptr_table {
            const std::type_info& (*get_type)();
            void (*static_delete)(any_voidptr*);
            void (*clone)(any_voidptr const*, any_voidptr*);
            void (*move)(any_voidptr const*,any_voidptr*);
        };

        // static functions for small value-types

        template<bool is_small>
        struct fxns
        {
            template<typename T>
            struct type {
                static const std::type_info& get_type() {
                    return typeid(T);
                }
                static void static_delete(any_voidptr* x) {
                    reinterpret_cast<T*>(x)->~T();
                }
                static void clone(any_voidptr const* src, any_voidptr* dest) {
                    new(dest) T(*reinterpret_cast<T const*>(src));
                }
                static void move(any_voidptr const* src, any_voidptr* dest) {
                    if(dest)reinterpret_cast<T*>(dest)->~T();
                    *reinterpret_cast<T*>(dest) = *reinterpret_cast<T const*>(src);
                }
            };
        };

        // static functions for big value-types (bigger than a any_voidptr)

        template<>
        struct fxns<false>
        {
            template<typename T>
            struct type {
                static const std::type_info& get_type() {
                    return typeid(T);
                }
                static void static_delete(any_voidptr* x) {
                    delete(*reinterpret_cast<T**>(x));
                }
                static void clone(any_voidptr const* src, any_voidptr* dest) {
                    *dest = (any_voidptr)new T(**reinterpret_cast<T* const*>(src));
                }
                static void move(any_voidptr const* src, any_voidptr* dest) {
                    if(dest) (*reinterpret_cast<T**>(dest))->~T();
                    **reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(src);
                }
            };
        };

        template<typename T>
        struct get_table
        {
            static const bool is_small = sizeof(T) <= sizeof(any_voidptr);

            static fxn_ptr_table* get()
            {
                static fxn_ptr_table static_table = {
                    fxns<is_small>::template type<T>::get_type,
                    fxns<is_small>::template type<T>::static_delete,
                    fxns<is_small>::template type<T>::clone,
                    fxns<is_small>::template type<T>::move
                };
                return &static_table;
            }
        };

        struct empty {
        };
    } // namespace any_detail


    struct any
    {
        // structors

        any() :
            table( any_detail::get_table<any_detail::empty>::get() ),
            object( 0 ),
            refs( 0 )
        {}

        ~any() {
            if( refs )
                table->static_delete(&object);
            object = 0;
        }

        template <typename T>
        any(const T& x) :
            table( any_detail::get_table<T>::get() ),
            refs( 1 ) {
                if (sizeof(T) <= sizeof(any_voidptr)) {
                    new(&object) T(x);
                }
                else {
                    object = (any_voidptr)new T(x);
                }
            }

        any(const any& x) :
            table( any_detail::get_table<any_detail::empty>::get() ),
            object( 0 ),
            refs( 0 ) {
              assign(x);
            }

        // assignment operator
        any& operator=( const any& x ) {
            return assign( x );
        }

        // assignment
        any& assign(const any& x) {

            if( x.empty() ) {
                reset();
                return *this;
            }

            // are we copying between the same type?
            if (table == x.table) {
                // if so, we can avoid reallocation
                x.table->move(&x.object, &object);
                table = x.table;
            }
            else {
                reset();
                x.table->clone(&x.object, &object);
                table = x.table;
            }

            refs = 1;

            return *this;
        }

        // utility functions

        any& swap(any& x) {
            std::swap(table, x.table);
            std::swap(object, x.object);
            std::swap(refs, x.refs);
            return *this;
        }

        const std::type_info& get_type() const {
            return table->get_type();
        }

        template<typename T>
        const T& as() const {
            if (get_type() != typeid(T)) {
                assert( !"<memo/memo.hpp> says: error! bad cast. is object registered? is key right? is factory empty?" );
            }
            if (sizeof(T) <= sizeof(any_voidptr)) {
                return *reinterpret_cast<T const*>(&object);
            }
            else {
                return *reinterpret_cast<T const*>(object);
            }
        }

        // automatic casting operator

        template<typename T>
        operator T() const {
          return as<T>();
        }

        // utils

        bool empty() const {
            return table == any_detail::get_table<any_detail::empty>::get();
        }

        void reset() {
            if (!empty()) {
                table->static_delete(&object);
                table = any_detail::get_table<any_detail::empty>::get();
            }
            object = 0;
            refs = 0;
        }

        // fields

        any_detail::fxn_ptr_table* table;
        any_voidptr object;
        size_t refs;
    };
}
