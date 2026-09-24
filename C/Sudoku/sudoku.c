#include <stdio.h>
#include <stdlib.h>
#include "maths.h"
#include "strats.h"
#include "grid.h"

/* Declare global variables */
int grid[9][9][11];
int row[9][9][2];
int col[9][9][2];
int box[9][9][2];

/* Main */

int main() {

    /* Easier One */
    // int array[9][9] = {
    //     {0, 1, 9,   0, 0, 2,   0, 0, 0},
    //     {4, 7, 0,   6, 9, 0,   0, 0, 1},
    //     {0, 0, 0,   4, 0, 0,   0, 9, 0},

    //     {8, 9, 4,   5, 0, 7,   0, 0, 0},
    //     {0, 0, 0,   0, 0, 0,   0, 0, 0},
    //     {0, 0, 0,   2, 0, 1,   9, 5, 8},

    //     {0, 5, 0,   0, 0, 6,   0, 0, 0},
    //     {6, 0, 0,   0, 2, 8,   0, 7, 9},
    //     {0, 0, 0,   1, 0, 0,   8, 6, 0},
    // };

    /* Harder One */
    int array[9][9] = {
        {4, 0, 0,   0, 9, 0,   2, 0, 0},
        {0, 7, 0,   0, 0, 0,   0, 0, 0},
        {0, 0, 0,   0, 1, 0,   0, 0, 8},

        {0, 4, 0,   0, 0, 9,   0, 5, 0},
        {0, 0, 0,   1, 5, 0,   6, 0, 0},
        {6, 0, 0,   0, 0, 2,   9, 3, 0},

        {0, 2, 0,   0, 8, 0,   0, 0, 0},
        {0, 0, 0,   0, 0, 0,   7, 0, 3},
        {0, 9, 5,   0, 0, 6,   4, 0, 0},
    };

    /* Hardest one */
    // int array[9][9] = {
    //     {8, 0, 0,   0, 0, 0,   0, 0, 0},
    //     {0, 0, 3,   6, 0, 0,   0, 0, 0},
    //     {0, 7, 0,   0, 9, 0,   2, 0, 0},

    //     {0, 5, 0,   0, 0, 7,   0, 0, 0},
    //     {0, 0, 0,   0, 4, 5,   7, 0, 0},
    //     {0, 0, 0,   1, 0, 0,   0, 3, 0},

    //     {0, 0, 1,   0, 0, 0,   0, 6, 8},
    //     {0, 0, 8,   5, 0, 0,   0, 1, 0},
    //     {0, 9, 0,   0, 0, 0,   4, 0, 0},
    // };

    init_grid(array);

    ///// OOh, have a flag for each strategy, like an index, and then have the solver function have a queue of which strategies to do next, and each strategy can add things that might work after it, as well as a general one that adds just one of each

    /* A flag to know if any progress is being made */
    int prog = 1;
    while (prog && !check_finished()) { 
        print_grid(0);
        prog = 0;
        prog = naked_single() || prog;
        prog = unique_cand() || prog;
        prog = naked_pair() || prog;
        prog = hidden_pair() || prog;
        prog = box_line() || prog;
        prog = pointing_line() || prog;
        prog = naked_triple() || prog;
        prog = naked_quadruple() || prog;
        prog = hidden_triple() || prog;
        prog = x_wing() || prog;
        prog = lines_3() || prog;
        prog = lines_4() || prog;
    }

    if (check_finished()) {
        print_grid(0);
        printf("The sudoku is solved! :)\n");
    } else {
        print_grid(1);
        printf("The solver is stuck! :(\n");
    }

    return 0;

}