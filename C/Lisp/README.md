C Project No. 1: Building a Lisp
Tutorial: https://www.buildyourownlisp.com/
NOTE: Notes taken in Notion & Copied across, not sure how that translates to .md files

## What is a Lisp?

- A **lisp** is a family of programming languages in which all their computation is represented by lists.

# Overview of Project

This project involves building a lisp based on the Polish Notation for arithmetic, in which the operand comes first:

```c
3 + 5       is written as  + 3 5
14 / 2      is written as  / 14 2
2 * 3 * 6   is written as  * 2 3 6
```

We shall use regular expressions to define a grammar, at its core consisting of numbers, operators and expressions, in which an expression is defined as either a number or an operator followed by one or more expressions.

Using the defined grammar, we can then take in a user-inputted expression and evaluate it. Furthermore, error-handling will ensure that the input is of the correct structure as defined by Polish Notation, as well as declare any other errors, such as division by zero.

## New Concepts

- Compiling code with
  `cc -std=c99 -Wall <file>.c -o <file>`
- Pointers using `*` after declaring variable type
- The preprocessor, written starting with `#` , is run before the `main` . Potential uses include importing modules and detecting the OS
- Memory allocation & de-allocation using `malloc()` and `free()`
- Strings end in **null terminators**, `\0`

## New Libraries

- `editline` for user input (add `-ledit` to compile command)
- `mpc` , a custom Parser Combinator library written by the author of the tutoria

# New Terminology

- String comparison:
  - `strstr(a, b)` returns a pointer to the index of the first instance of `b` in `a` , or `NULL` if not present
  - `strcmp(a, b)` returns 0 if `a` is identical to `b` (somewhat convoluted if not - read docs)
  - `strlen(a)` returns the length of string `a` .
- Type conversion using functions, i.e. `strtol()` which is ‘**STR**ing **TO** **L**ong"
- `switch() {}` and `case`
- Enumerations using `enum {}`to assign integer values from 0 to a list of variables under the hood
- Ternary operator using
  `(*condition*) ? *expressionTrue* : *expressionFalse*;`

- Field access of pointer types uses `->` , i.e. to access the x-coordinate of a struct variable `coord pos` would be `pos.x` , but of a struct pointer variable `coord* pos` would be `pos->x` .
- Memory allocation & managing:
  - `TYPE* malloc(N)` allocates `N` `TYPE`'s of memory and returns a pointer to the space in memory
  - `free()`de-allocates memory
