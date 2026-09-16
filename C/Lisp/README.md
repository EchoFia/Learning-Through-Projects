C Project No. 1: Building a Lisp

**Tutorial**: https://www.buildyourownlisp.com/

## What is a Lisp?

- A **lisp** is a family of programming languages in which all their computation is represented by lists.

# Overview of Project

This project involves building a lisp based on the Polish Notation for arithmetic, in which the operand comes first:

```c
3 + 5       is written as  + 3 5
14 / 2      is written as  / 14 2
2 * 3 * 6   is written as  * 2 3 6
```

We shall use regular expressions to define a grammar, at its core consisting of numbers, operators and expressions, in which an expression is defined as either a number or an operator followed by one or more expressions. This shall then be re-factored to include S-Expressions, and then Q-Expressions.

Using the defined grammar, we can then take in a user-inputted expression and evaluate it. Furthermore, error-handling will ensure that the input is of the correct structure as defined by Polish Notation, as well as declare any other errors, such as division by zero.

# What are Regular Expressions?

**Regular expressions** (Regex) are a way of writing grammars for small sections of texts. Regexes are precise and concise, but in exchange, grammars written in them can’t consist of multiple rules. The required regexes for this project are:

```c
.	        Any character is required.
a	        The character a is required.
[abcdef]	Any character in the set abcdef is required.
[a-f]	    Any character in the range a to f is required.
a?	      The character a is optional.
a*       	Zero or more of the character a are required.
a+	      One or more of the character a are required.
^	        The start of input is required.
$	        The end of input is required.
```

# S-Expressions & Q-Expressions

**S-Expressions**, or Symbolic Expressions, are used to represent nested lists. Typically, S-Expressions are defined inductively as either an **atom** such as a symbol or number, or two other S-Expressions joined, or **cons**, together. This would naturally involve using **linked lists**. For simplicity, we shall instead represent S-Expressions as variable sized arrays.

## New Concepts

- Compiling code with
  `cc -std=c99 -Wall <file>.c -o <file>`
- Pointers using `*` after declaring variable type
- The preprocessor, written starting with `#` , is run before the `main` . Potential uses include importing modules and detecting the OS
- Memory allocation & de-allocation using `malloc()` , `realloc()` and `free()`
- Idea of **The Stack & The Heap**, in which The Stack is the memory where the program lives including all of its temporary variables, and The Heap is the memory that stores objects with a longer lifespan.
- Strings end in **null terminators**, `\0`
- `struct`'s referencing themselves must only reference pointers to themselves
- Functions can refer to each other - to do so, define one of them with no body (replaced by a semi-colon), then define the other function and then the original.

## New Libraries

- `editline` for user input (add `-ledit` to compile command)
- `mpc` , a custom Parser Combinator library written by the author of the tutorial
- `errno` aids in reporting and retrieving errors, by setting a globally defined `int errno` variable to the error. Here, we often use `ERANGE`, and error for if the variable is too large for the memory allocated

# New Coding Terminology

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
  - `TYPE* x = malloc(N)` allocates `N` `TYPE`'s of memory and returns a pointer to the space in memory, stored here in `x`
  - `realloc(x, M)` re-allocates `x` `M` `TYPE`'s of memory and returns a pointer
  - `free(x)`de-allocates the memory of variable `x`
  - `sizeof(x)` returns the size of the memory allocated to variable `x`
