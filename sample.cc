#include <cassert>
#include <string>
#include <iostream>

#include "memo.hpp"

struct spell {
    unsigned damage;
    unsigned level;
};

struct shield {
    unsigned coins;
    double defense;
    bool lefthanded;
};

int main( int argc, const char **argv ) {
    memo::factory< std::string > store;

    // register
    store["waterball-spell"] = spell( {150, 3} );
    store["fireball-spell"] = spell( {200, 5} );
    store["wooden-shield"] = shield( {10, 2, true} );
    store["iron-shield"] = shield( {50, 5, true});

    // list
    std::cout << "list { ";
    for( auto &in : store ) {
        std::cout << in.first << ',';
    }
    std::cout << " }" << std::endl;

    // clone items (automatic typing)
    shield clone = store["wooden-shield"];
    spell clone2 = store["fireball-spell"];

    // verify
    assert( 10 == clone.coins );
    assert( 2 == clone.defense );
    assert( true == clone.lefthanded );

    assert( 200 == clone2.damage );
    assert( 5 == clone2.level );

    std::cout << "All ok." << std::endl;
}

