#include <stdio.h>
#include <stdlib.h>
#include "maths.h"
#include "strats.h"

/* Declare global variables */
int grid[9][9][11];
int row[9][9][2];
int col[9][9][2];
int box[9][9][2];

// int x_found(int x, int unit[9][2]) { 
//     int result = 0;
//     for (int i = 0; i < 9 && !result; i++) {
//         int iR = unit[i][0];
//         int iC = unit[i][1];
//         int *cands = grid[iR][iC];
//         result = (cands[0] == 1) && cands[x];
//     }
//     return result;
// }

/* Generating the sudoku */

// NOTE: Later on, will actually generate sudokus, for now will just import one

/* Inputs the starting digits and calculates all the initial candidates */
void init_grid(int array[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            int x = array[i][j];

            for (int k = 1; k < 11; k++) {
                grid[i][j][k] = 0;
            }

            if (x != 0) {
                grid[i][j][0] = 1;
                grid[i][j][x] = 1;
                grid[i][j][10] = x;
            } else {
                grid[i][j][0] = 9;
                for (int k = 1; k < 10; k++) {
                    grid[i][j][k] = 1;
                }
            }
        }
    }

    /* Initialises row, col and box */
    for (int k = 0; k < 9; k++) {
        for (int j = 0; j < 9; j++) {
            row[k][j][0] = k;
            row[k][j][1] = j;
            col[j][k][0] = k;
            col[j][k][1] = j;
            box[calc_box(k, j)][calc_box_index(k, j)][0] = k;
            box[calc_box(k, j)][calc_box_index(k, j)][1] = j;
        }
    }

    /* Cleans up each of the starting digits */
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j][0] == 1) {
                new_solved_number(i, j);
            }
        }
    }
}

/* Printing Functions */

/* Prints the grid in a readable format, a grid of 3x3 boxes */
/* If 'flag_num_cands' is TRUE (1), will print the number of candidates instead of the solved digits */
void print_grid(int flag_num_cands) {
    printf("----------------------\n");
    for (int i = 0; i < 9; i++) {

        if (i % 3 == 0 && i != 0) {
            printf("-------+------+-------\n");
        }

        for (int j = 0; j < 9; j++) {
            
            if (j % 3 == 0) {
                printf("|");
            }

            if (!flag_num_cands) {
                if (grid[i][j][10]) {
                    for (int k = 1; k < 10; k++) {
                        if (grid[i][j][k]) { printf("%i ", k); }
                    }
                } else {
                    printf(". ");
                }
            } else {
                printf("%i ", grid[i][j][0]);
            }
            
        }
        printf("|\n");
    }
    printf("----------------------\n");
}

/* Prints the candidates for a cell */
void print_possibilities(int r, int c) {
    printf("\nPossibilities are: ");
    for (int i = 1; i < 10; i++) {
        if (grid[r][c][i]) { printf("%i, ", i); }
    }
}

/* Main */

int main() {

    int array[9][9] = {
        {0, 1, 9,   0, 0, 2,   0, 0, 0},
        {4, 7, 0,   6, 9, 0,   0, 0, 1},
        {0, 0, 0,   4, 0, 0,   0, 9, 0},

        {8, 9, 4,   5, 0, 7,   0, 0, 0},
        {0, 0, 0,   0, 0, 0,   0, 0, 0},
        {0, 0, 0,   2, 0, 1,   9, 5, 8},

        {0, 5, 0,   0, 0, 6,   0, 0, 0},
        {6, 0, 0,   0, 2, 8,   0, 7, 9},
        {0, 0, 0,   1, 0, 0,   8, 6, 0},

        // {0, 5, 6,   0, 0, 0,   0, 4, 0},
        // {1, 0, 0,   0, 2, 8,   0, 7, 9},
        // {2, 0, 0,   1, 0, 0,   8, 3, 0},
    };

    init_grid(array);

    ///// OOh, have a flag for each strategy, like an index, and then have the solver function have a queue of which strategies to do next, and each strategy can add things that might work after it, as well as a general one that adds just one of each

    /* A flag to know if any progress is being made */
    // int prog = 1;
    // while (prog) { 
    //     prog = naked_single();
    //     // prog = progress || unique();
    //     prog = naked_pair();
    //     print_grid(1);
    // }

    print_grid(1);

    // check_hidden_pair(row[2], "row", 2);
    int z = hidden_pair();
    printf("%i\n", z);

    print_grid(1);

    return 0;

}