#include <cassert>

#include <iostream>
#include <list>
#include <string>
#include <vector>

#include "memo.hpp"

void sample1() {
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
}

struct spell {
    unsigned damage;
    unsigned level;
};

struct shield {
    unsigned coins;
    double defense;
    bool lefthanded;
};

enum {
    WATERBALL_SPELL,
    FIREBALL_SPELL,
    WOODEN_SHIELD,
    IRON_SHIELD
};

void sample2() {

    // create an abstract factory; items located by enum id
    memo::factory< int > factory;

    // inscribe objects of any kind
    factory[WATERBALL_SPELL] = spell( {150, 3} );
    factory[FIREBALL_SPELL] = spell( {200, 5} );
    factory[WOODEN_SHIELD] = shield( {10, 2, true} );
    factory[IRON_SHIELD] = shield( {50, 5, true});

    // list
    std::cout << "list { ";
    for( auto &in : factory ) {
        std::cout << in.first << ',';
    }
    std::cout << " }" << std::endl;

    // clone items (automatic casting)
    shield clone = factory[WOODEN_SHIELD];
    // clone items (explicit casting)
    spell clone2 = factory[FIREBALL_SPELL].as<spell>();

    // verify
    assert( 10 == clone.coins );
    assert( 2 == clone.defense );
    assert( true == clone.lefthanded );

    assert( 200 == clone2.damage );
    assert( 5 == clone2.level );
}

int main( int argc, const char **argv ) {
    sample1();
    sample2();

    std::cout << "All ok." << std::endl;
}

