# soacpp

<!---
[![Travis CI Build Status](https://api.travis-ci.org/rams3s/soacpp?branch=master)](https://travis-ci.org/rams3s/soacpp)
[![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/XXX?svg=true)](https://ci.appveyor.com/project/rams3s/soacpp)
[![codecov.io](http://codecov.io/github/rams3s/soacpp/coverage.svg?branch=master)](http://codecov.io/github/rams3s/soacpp?branch=master)
[![Documentation Status](https://readthedocs.org/projects/soacpp/badge/?version=latest)](http://soacpp.readthedocs.io/en/latest/?badge=latest)
--->

A single-header Struct-of-Arrays (SoA) container in C++.

DISCLAIMER: this project is still an early proof of concept and hasn't been tested in production code.

This project is distributed under the [MIT license](https://opensource.org/licenses/MIT).

## Introduction

For a brief struct-of-arrays (SoA) vs array of structs (AoS) introduction, please check https://en.wikipedia.org/wiki/AOS_and_SOA.

## Features

## Examples

### Using helper macro

The library provides the `DECLARE_SOA_TYPE` macro to ease transitioning existing code to SoA.

```cpp
```

### A simple SoA without macro

### Customize container type

### Customize reference type

Providing a custom reference type (must be aggregate initializable http://en.cppreference.com/w/cpp/language/aggregate_initialization) can help make code easier to read / maintain.
Compare multiple `soa::get<A>(i), soa::get<B>(i)` vs `ref.a, ref.b`.

## Documentation

## Build

### Compiling

```bash
make
./build/examples
```

### Running the examples

```bash
./build/examples
```

### Running the tests

```bash
cd build
ctest -V
```

## Benchmarks

## Similar projects
- https://github.com/eXistence/Arrays
- https://github.com/gentryx/libflatarray
- https://github.com/Lunarsong/StructureOfArrays

