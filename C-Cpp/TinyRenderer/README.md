C++ Project No. 1: Building a Simplified Renderer

**Tutorial**: https://haqr.eu/tinyrenderer/

## What is a Renderer?

-

# Overview of Project

-

NOTE: SHAll use their tga library

## New Concepts

- Evaluating certain variables pre-compiling to optimise computation time
- **Barycentric Coordinates** are a coordinate system in which points are defined by reference to a **simplex**, that being a triangle for 2D space
- A `.tga` file is a raster graphics format
- `template` for working with undefined **types**
- `#pragma` is used to provide additional info to the compiler

## New Libraries

- `cmath` adds in general mathematical functions
- `tuple` adds the tuple class
- `vector` adds functionality for working with vectors
- `iostream` adds standard **i**nput/**o**utput streams
- `cstring` adds functionality to work on arrays and C-style strings (an array of `char`'s)
- `fstream` adds functionality for file handling
- `sstream` adds functionality for handling strings, such as those from user input (`cin`)

# New Coding Terminology

- `std::func` means that `func` is a function from the `std` library - can instead add `using namespace std`
- `cout <<` prints out strings
- `cin >>` takes in user input, can be used in conjunction with `getline`
- `cerr <<` outputs error messages
- Use `template <typename T>` will allow for creating functions, classes, etc… with a designated type `T`
- Using `template` with non-type arguments (i.e. `template <int X>` generates the template during compiling, and thus can be used for fixed values

- `constexpr` will evaluate a variable before compilation, useful for fixed values like `minsPerHour`
- `#pragma once` makes sure that the compiler only includes the current header once
