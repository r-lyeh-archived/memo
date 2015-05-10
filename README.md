memo <a href="https://travis-ci.org/r-lyeh/memo"><img src="https://api.travis-ci.org/r-lyeh/memo.svg?branch=master" align="right" /></a>
====

- Memo is a simple and lightweight factory class, written in C++11.
- Memo is an abstract factory class, designed mainly for resource handling.
- Memo is automatic type casting. Store resources of any kind in the very same factory.
- Memo is deterministic lifetime of objects, that get destroyed when factory is destroyed.
- Memo is cross-platform. Builds on Windows/Linux/MacosX. Compiles on g++/clang/msvc.
- Memo is tiny, header-only, self-contained.
- Memo is zlib/libpng licensed.

sample
------

```c++
    // abstract factory: locate items by string id
    memo::factory< std::string > factory;

    // inscribe items of any kind (textures, anims, maps, texts...)
    factory["numbers"] = std::vector<int>( {1,2,3,4,5} );
    factory["names"] = std::list<std::string>( {"john", "mark", "tony"} );

    // clone items (implicit casting)
    std::vector<int> numbers = factory["numbers"];

    // clone items (explicit casting)
    std::list<std::string> names = factory["names"].as< std::list<std::string> >();

    // tests
    assert( 5 == numbers.size() );
    assert( 3 == names.size() );

    // delete items on factory destruction
```

Check [sample.cc](sample.cc) for detailed usage.
