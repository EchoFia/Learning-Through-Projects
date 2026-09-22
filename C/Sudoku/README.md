C Project No. 2: Building a Sudoku Solver

**Tutorial**: _Sudoku Programming with C_ by Giulio Zambon

## Terminology

- A **unit** is either a row, column or box
- **Sudoku rules** are that each unit must contain the digit 1 to 9 exactly once each
- A **candidate** of a cell is a number that could be placed in that cell without breaking sudoku rules
- A **line** is a row or a column

# Overview of Project

The aim of this project is to code more independently to create a sudoku solver & sudoku generator. I shall using a few sources that describe different strategies for generating & solving sudokus, that I will then implement in the code.

Everything shall be stored in `int grid[9][9][11]`, in which for every cell of the 9x9 grid:

- indices `1` through `9` are flags for if that cell could be that digit by sudoku rules
- index `0` is the total number of candidates for that cell
- index `10` is a flag that stores the value of a solved cell, or `0` if unsolved

As the code for the solver grows with more and more complex strategies, the computing time will likely take longer and longer. This will be a good chance to optimise both the algorithms and, more importantly, their implementation in the code.

# Strategies to Implement

Based on the book _Sudoku Programming with C_, I shall implement 5 levels of strategies as described below. To implement these strategies, it’s crucial to maintain an updated list of potential candidates of each cell in `grid`. As such, `grid` must be initialised with the starting digits and each cell’s initial candidates, and these candidates must be updated every time a new digit is solved (known as **cleaning up**).

## Level 0 Strategies

- **Naked Single**: A cell with only a single candidate is solved by said candidate
- **Unique Candidate**: A candidate that is only present once in a single unit must solve that cell it is present in

## Level 1 Strategies

- **Naked Pair**: If two cells in the same unit only contain the same two candidates, those candidates solve those two cells
- **Hidden Pair**: If two candidates only appear in two cells of a unit, those candidates solve those two cells
- **Box-Line**: When a candidate within a line only appears in one box, that candidate’s solution for that box must appear on that line
- **Pointing-Line**: When a candidate within a box only appears in one line, that candidate’s solution for that line must appear in the box

## Level 2 Strategies

- **Naked Triple**: Same concept as Naked Pair
- **Naked Quadruple**: Same concept as naked Pair

## New Concepts

- A concept I was struggling with was Variable Length Arrays (VLA’s), especially higher-dimension array, which I believe there are multiple ways of doing, of which I’ve chosen the one described below
- Code can be separated into files by creating a set of a `.h` file, which declares the functions and their parameters, and a `.c` file, which contains the code of said functions. Use `#include "file-name.h"` in the core file, and add the `.c` file in the compile command.

## New Libraries

# New Coding Terminology

```c
/* 2D VLA - malloc/realloc + trick */

/* Initialising the 2D array */
int** arr = malloc(m * sizeof(int*));
arr[0] = malloc(m * n * sizeof(int));
/* Trick: stores the array together, thus other
pointers are just slightly further in the memory */
for(int i = 1; i < m; i++) {
    arr[i] = arr[0] + i * n;
}

/* Adding to the VLA */
arr = realloc(arr, (m + 1) * sizeof(int*));
arr[0] = realloc(arr[0], (m + 1) * n * sizeof(int));
for(int i = 1; i < m; i++) {
    arr[i] = arr[0] + i * n;
}

/* Cleaning Up */
free(arr[0]);
free(arr);
```

- **NOTE**: `a || b` will not evaluate `b` if `a` is `TRUE`
- `extern` is used to bring global variables from the core file to imported files
- `typedef a b` makes `b` an alias for `a` (also works for `struct a'` as `a`)
- To do nothing (’pass’), just put a `;`
