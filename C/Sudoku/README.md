C Project No. 2: Building a Sudoku Solver

**Tutorial**: _Sudoku Programming with C_ by Giulio Zambon

# Overview of Project

The aim of this project is to code more independently to create a sudoku solver & sudoku generator. I shall using a few sources that describe different strategies for generating & solving sudokus, that I will then implement in the code.

Everything shall be stored in `int grid[9][9][11]`, in which for every cell of the 9x9 grid:

- indices `1` through `9` are flags for if that cell could be that digit by sudoku rules
- index `0` is the total number of candidates for that cell
- index `10` is a flag that stores the value of a solved cell, or `0` if unsolved

As the code for the solver grows with more and more complex strategies, the computing time will likely take longer and longer. This will be a good chance to optimise both the algorithms and, more importantly, their implementation in the code.

# Terminology

- A **unit** is either a row, column or box
- **Sudoku rules** are that each unit must contain the digit 1 to 9 exactly once each
- A **candidate** of a cell is a number that could be placed in that cell without breaking sudoku rules

# Strategies to Implement

Based on the book _Sudoku Programming with C_, I shall implement 5 levels of strategies as described below. To implement these strategies, it’s crucial to maintain an updated list of potential candidates of each cell in `grid`. As such, `grid` must be initialised with the starting digits and each cell’s initial candidates, and these candidates must be updated every time a new digit is solved (known as **cleaning up**).

## Level 0

- **Naked Single**: A cell with only a single candidate is solved by said candidate
- **Unique**: A candidate that is only present once in a single unit must solve that cell it is present in

## New Concepts

## New Libraries

# New Coding Terminology
