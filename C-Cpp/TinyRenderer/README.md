C++ Project No. 1: Building a Simplified Renderer

**Tutorial**: https://haqr.eu/tinyrenderer/

## What is a Renderer?

- A renderer takes in raw data and processes them via the **rendering pipeline** to give a 2D image of pixels.

## The Different Perspectives

**NOTE**: I think this might be wrong, need to double check

- **Normalised Device Co-ordinates** (NDC’s) are stored from -1 to 1 in all axes
- **Clip Co-ordinates** are a 4D projection of NDC’s in which the 4th dimension is a scaling factor to return the 3 dimensions back to NDC’s. Clip co-ordinates are efficient for calculating depths.
- **Eye Co-ordinates** are stored relative to the camera
- **Screen Co-ordinates** are a 2D projection of the eye co-ordinates to the screen

# Overview of Project

- To begin with, we shall learn a few simple methods of drawing lines (Bresenham’s line algorithm) and triangles (scanline rendering & bounding box methods with barycentric co-ordinates). By storing the **depth** of each pixel in a `zbuffer` , we can ignore background pixels. We shall use the tutorial’s `tgaimage` library to create a `.tga` output file.
- Together, the previous ideas allow us to create **orthographic** projections, those in which depth has no impact on the size.
- To simulate a camera being moved, we instead inversely transform the scene (i.e. to rotate the camera left, we instead rotate the scene right). The camera shall be described by 3 vectors: the **centre** of the screen, the position of the **eye** relative to the centre, and an **up** vector. These can be used to calculate transformation matrices, `ViewPort` scales the scene to the size of the screen, a `Perspective` deformation using focal length `f`, and `ModelView` which places the object relative to the camera.
- Textures are taken from `*_nm_tangent.tga` files, which describes the normal at each vertex relative to a local basis, which is composed of the **tangent**, **bitangent** and **normal** vectors.
- Lighting is typically split into 3 factors: an **ambient** term models indirect light (constant background lighting), a **diffuse** term represents light scattered from rough surfaces, and a **specular** term models bright highlights caused by mirror-like reflections. There are multiple ways of calculating each with varying computational costs and accuracies, as described in the tutorial. In our final model:
  - The ambient term is set to a constant (`0.3`), but a better approximation can be calculated using **ambient occlusion**
  - The diffuse term is read in from a `*_diffuse.tga` file, which describes the colours of each fragment
  - The specular term is the product of a power-law decay function and a term read in from a `*_spec.tga` file
- Shadow mapping uses the same concepts as the `zbuffer` for backface culling, but from the light source’s perspective. That is, it stores the **depth** of the closest fragment to the light source, and anything behind that is in shadow.

# The Rendering Pipeline

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
- `sys/stat.h` includes functionality to make directories

# New Coding Terminology

- `std::func` means that `func` is a function from the `std` library - can instead add `using namespace std`
- `cout <<` prints out strings
- `cin >>` takes in user input, can be used in conjunction with `getline`
- `cerr <<` outputs error messages
- Use `template <typename T>` will allow for creating functions, classes, etc… with a designated type `T`
- Using `template` with non-type arguments (i.e. `template <int X>` generates the template during compiling, and thus can be used for fixed values
- `struct B : A` uses a separate `struct A` as an initialiser for `B` (i.e. to initialise variables and functions)
- To convert `std::string s "abc"` to a C-type string, use `const char* cs = s.c_str()` or `char* cs = s.data();`

- `constexpr` will evaluate a variable before compilation, useful for fixed values like `minsPerHour`
- `#pragma once` makes sure that the compiler only includes the current header once
- `#pragma omp parallel <func>` causes the next `<func>` to be run in parallel
- `mkdir(filename, mode)` creates a directory, returning `1` if an error arose, and storing said error to `errno`

```cpp
/* File Handling */
td::ifstream in;
in.open(filename, std::ifstream::in);
if (in.fail()) return;
std::string line;
while (!in.eof()) {
    std::getline(in, line);
    ...
}
```
