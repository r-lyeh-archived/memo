memo
====

- Simple and lightweight factory class, written in C++11.
- An abstract factory class, designed mainly for resource handling.
- Automatic type casting. Store resources of any kind in the very same factory.
- Deterministic lifetime of objects, that get destroyed when factory is destroyed.
- Cross-platform. Builds on Windows/Linux/MacosX. Compiles on g++/clang/msvc.
- No external dependencies.
- Tiny. Header only.
- BOOST licensed.

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
