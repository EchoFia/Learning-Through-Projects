SQL Project No. 1: Creating a Mock Bookstore System

**Tutorial**: https://www.geeksforgeeks.org/sql/bookstore-analytics-with-sql/

## Project Summary

 As an introduction to SQL (with MySQL), we shall create a sample database about a bookstore’s sales. SQL can then be used to extract and calculate specific marketing information from the data.

## New Concepts

- **Common Table Expressions** (CTE) are used to create a temporary set of results, aiding readability

## New Libraries

- 

## New Coding Terminology

- Standard Keywords - Definitions Online:
    - `(INNER) JOIN`, `LEFT JOIN`, `RIGHT JOIN`, `FULL JOIN`
    - `GROUP BY`
    - `HAVING` for **grouped** data, `WHERE` for **row-level** data

- `WITH (...)` creates **CTE**’s
- `FROM customers c` will give `customers` the **alias** `c`
- `LIMIT X` will restrict the output to only the first `X` rows
- Date Manipulation:
    - `DATEDIFF(A, B)` gives `A` - `B` essentially
    - `DATE_FORMAT()` for **MySQL** - rather than `STRFTIME()`
    - `CURDATE()` gives the current date