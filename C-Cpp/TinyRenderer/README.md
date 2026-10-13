C++ Project No. 1: Building a Simplified Renderer

**Tutorial**: https://haqr.eu/tinyrenderer/

## What is a Renderer?

- A renderer takes in raw data and processes them via the **rendering pipeline** to give a 2D image of pixels.

# Overview of Project

- To begin with, we shall learn a few simple methods of drawing lines (Bresenham’s line algorithm) and triangles (scanline rendering & bounding box methods with barycentric co-ordinates). By storing the **depth** of each pixel in a `zbuffer` , we can ignore background pixels. We shall use the tutorial’s `tgaimage` library to create a `.tga` output file.
- Together, the previous ideas allow us to create **orthographic** projections, those in which depth has no impact on the size.
- To simulate a camera being moved, we instead inversely transform the scene (i.e. to rotate the camera left, we instead rotate the scene right). The camera shall be described by 3 vectors: the **centre** of the screen, the position of the **eye** relative to the centre, and an **up** vector. These can be used to calculate transformation matrices, `ViewPort` scales the scene to the size of the screen, a `Perspective` deformation using focal length `f`, and `ModelView` which places the object relative to the camera.

# The Rendering Pipeline

!Screenshot 2026-10-13 at 11.38.17.png

- **Vertex Processing**: Extracts the vertices from the raw data
- **Primitive Assembly**: Connects vertices to give primitives (i.e. triangles)
- **Rasterisation**: Converts each primitive into a set of fragments
- **Fragment Processing**: Applies properties (i.e. colour, …) to the fragments
- **Output Merging**: Combines the fragments of all primitives in 3D space to yield a 2D image of pixels

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
- `#pragma omp parallel <func>` causes the next `<func>` to be run in parallel
