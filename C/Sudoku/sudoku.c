#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "maths.h"
#include "strats.h"
#include "grid.h"

/* Declare global variables */
int grid[9][9][11];
int row[9][9][2];
int col[9][9][2];
int box[9][9][2];
int print_flag;

/* Runs every strategy until either solved or nothing new is being found */
void run_strats() {
    /* A flag to know if any progress is being made */
    int prog = 1;
    while (prog && !check_finished() && !check_not_possible()) { 
        if (print_flag) { print_grid(0); }
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
        prog = xy_chain() || prog;
        prog = rectangles() || prog;
    }
}

void backtracking() {

    /* Runs all the strategies to see if the current grid can be solved */
    run_strats();

    /* If the grid is un-solved but still seemingly possible */
    if (!check_finished() && !check_not_possible()) { 

        /* Find first un-solved cell that isn't finished, with the least number of candidates */
        int cell[2];
        int num_cands = 10; /* NOTE: 10 is arbitrarily higher than any possible number of candidates */

        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (grid[i][j][0] < num_cands && !grid[i][j][10]) {
                    cell[0] = i;
                    cell[1] = j;
                    num_cands = grid[i][j][0];
                }
            }
        }

        int* snapshot = take_snapshot();

        /* Loop through all the possible candidates of the selected cell and try with that candidate solving that cell */
        int* cands = get_cands(cell[0], cell[1]);
        for (int i = 0; i < num_cands; i++) {
            solve_digit(cands[i], cell[0], cell[1]);
            if (print_flag) { printf("Backtracking: Placed %i in (%i, %i)\n", cands[i], cell[0], cell[1]); }
            // scanf("%i", &print_flag);
            backtracking();

            if (check_finished()) { break; }
            else if (check_not_possible()) {
                if (print_flag) { printf("Backtracking: Incorrect, removed %i in (%i, %i)\n", cands[i], cell[0], cell[1]); }
            }

            reset_to_snapshot(snapshot);
        }

        free(cands);
        free(snapshot);

    }
}

/* Main */

int main() {

    /* If set to 'TRUE', then will print out each step */
    print_flag = 0;

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
    // int array[9][9] = { /// CHANGeD TO HAVE AN XY-CHAIN BUT INSTEAD HAS NAKED TRIPLES THAT ARE MISSED
    //     {4, 0, 0,   0, 9, 7,   2, 0, 0},
    //     {8, 7, 3,   0, 0, 4,   0, 0, 0},
    //     {0, 0, 0,   0, 1, 0,   0, 0, 8},

    //     {0, 4, 0,   0, 0, 9,   0, 5, 0},
    //     {0, 0, 0,   1, 5, 0,   6, 0, 0},
    //     {6, 0, 0,   0, 0, 2,   9, 3, 0},

    //     {0, 2, 4,   0, 8, 0,   0, 0, 0},
    //     {0, 0, 0,   5, 0, 0,   7, 0, 3},
    //     {7, 9, 5,   0, 0, 6,   4, 0, 0},
    // };

    // int array[9][9] = {
    //     {7, 0, 0,   0, 0, 0,   0, 1, 0},
    //     {0, 0, 0,   0, 0, 0,   9, 0, 0},
    //     {0, 3, 0,   1, 0, 4,   0, 0, 5},

    //     {0, 0, 0,   9, 0, 0,   4, 0, 1},
    //     {0, 8, 0,   0, 0, 0,   0, 0, 6},
    //     {2, 0, 0,   0, 7, 0,   0, 5, 0},

    //     {0, 0, 0,   0, 1, 0,   3, 0, 7},
    //     {5, 0, 0,   0, 9, 0,   0, 6, 0},
    //     {6, 0, 8,   0, 0, 0,   0, 0, 9},
    // };

    /* Hardest one */
    int array[9][9] = {
        {8, 0, 0,   0, 0, 0,   0, 0, 0},
        {0, 0, 3,   6, 0, 0,   0, 0, 0},
        {0, 7, 0,   0, 9, 0,   2, 0, 0},

        {0, 5, 0,   0, 0, 7,   0, 0, 0},
        {0, 0, 0,   0, 4, 5,   7, 0, 0},
        {0, 0, 0,   1, 0, 0,   0, 3, 0},

        {0, 0, 1,   0, 0, 0,   0, 6, 8},
        {0, 0, 8,   5, 0, 0,   0, 1, 0},
        {0, 9, 0,   0, 0, 0,   4, 0, 0},
    };

    init_grid(array);

    printf("Starting Grid:\n");
    print_grid(0);

    backtracking();

    ///// OOh, have a flag for each strategy, like an index, and then have the solver function have a queue of which strategies to do next, and each strategy can add things that might work after it, as well as a general one that adds just one of each

    // REMEMBER TO ADD TESTING TO MAKE SURE THAT IT CAN SPOT EVERYTHING

    print_grid(0);
    if (check_finished()) {
        printf("The sudoku is solved! :)\n");
    } else {
        print_grid(1);
        printf("The solver is stuck! :(\n");
    }

    return 0;

}