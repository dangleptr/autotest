# Autotest

Autotest facilitates the testing of class interfaces by taking proven methods, then combining and augmenting them in order to create an effective yet easy to use automatic testing tool.

It can fulfill two different use-cases with its coverage-guided automatic testing:
- It can be used as a nightly automatic tester tool like a fuzzer to:
  - highlight possible interface misuses which cause sever bugs like crashes or undefined behaviour (and everything else for which Clang has an implemented sanitizer)
  - discover invariant violations if the user choses to provide an invariant for the class state
- It can be used to generate a test corpus once when the interface changes and to quickly rerun the same tests after every new commit to make sure that no severe bugs are introduced

This smart testing process is powered by [LLVM libFuzzer](https://llvm.org/docs/LibFuzzer.html).

## Usage

There is a more elaborate use case within the `example` folder which includes the ability to measure LOC coverage, but the basic usage is the following.

### Installation

This library requires a preinstalled LLVM-Clang. It was tested with version 10.0.0 and 11.0.0

To compile and install the library:
```
$ mkdir build && cd build
$ cmake .. 
$ make install
```
The argument `-DCMAKE_CXX_COMPILER=path/to/clang` might also be necessary to explicitly tell CMake to use Clang.

### CMake integration

Minimal CMakeLists.txt:
```cmake
find_package(autotest REQUIRED)

add_executable(baz-tester "src/main.cpp")

set_target_properties(baz-tester PROPERTIES COMPILE_FLAGS "-fsanitize=fuzzer")
set_target_properties(baz-tester PROPERTIES LINK_FLAGS "-fsanitize=fuzzer")

target_link_libraries(baz-tester PRIVATE autotest::autotest)
```

### C++ integration

Supposing the following interface of baz.hpp:
```cpp
struct Baz {
    void foo() const;
    int bar(int t);
};
```

The following main.cpp can serve as the testing configuration:
```cpp
#include "baz.hpp"

#include "autotest/autotest.hpp"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    using AutoTest::Args::integral;
    AutoTest::Builder<Baz>{ Data, Size } // forward the fuzzer input
        .AUTOTEST_CONST_FUN(foo) // register a T() const method
        .AUTOTEST_FUN(bar, integral<int>) // register a T(int) method
    .execute(); 

    return 0;
}
```

The resulting binary is a libFuzzer target, so all arguments of the library can be used freely.

The compilation of this target also has to be done with Clang.

### Running

Running the target for a given time to discover possible faults:
```
$ ./baz-tester -max_total_time=TIME_IN_SECONDS 
```

Running the target for a given time to discover test cases:
```
$ ./baz-tester DIR/TO/STORE/CASES -max_total_time=TIME_IN_SECONDS 
```

The average size of the test cases generated grows overtime. If we want to make sure that it generates simpler test cases, it might be a good idea to also use the argument `-len_control=RATE` the higher number `RATE` is, the slower libFuzzer will expand the length.

To run the target on the previously generated test cases, add the filenames to the target as parameters:

```
$ ./example DIR/TO/STORE/CASES/df9e7e9f6dc5365fbccfc282fe99c2f758d7dd4a DIR/TO/STORE/CASES/a96bf7fc9666fe5a22aaf055edc70a7b1a191cd9...
```

Or:

```
$ ./example DIR/TO/STORE/CASES/*
```

## Enchancements

### Coverage guidance

For coverage guidance, libFuzzer can use the Clang sancov tools, like inline 8-bit counters. To use this, add the following compile flags the CMake file:
`-fsanitize-coverage=GRANULITY,pc-table,inline-8bit-counters`, where `GRANULITY` is from least to most information: `func`, `bb` , `edge`.

More information on sanitizer coverage guidance can be found [here](https://clang.llvm.org/docs/SanitizerCoverage.html).

### Sanitizers

To make sure that as many errors as possible can be recognized, it is a good idea to use sanitizers. Since `-fsanitize=` expects a list, turning for example the AddressSanitizer on one is as easy as adding `,address` on the end of it.

More information on different sanitizers:
- [AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
- [MemorySanitizer](https://clang.llvm.org/docs/MemorySanitizer.html)
- [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html)

Be vary that including sanitizers can have a noticable performance impact.

### Invariant

To add an invariant that is checked after construction and also after every method call, simply pass a function with the signature `bool(const CLASS&)` to the `execute` function.