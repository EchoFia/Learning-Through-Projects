/* Libraries */
#include <stdio.h>
#include "strats.h"
#include "maths.h"

/* Import Global Variables */
extern int grid[9][9][11];
extern int row[9][9][2];
extern int col[9][9][2];
extern int box[9][9][2];

/* Generating the Grid */

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
                new_solved_digit(i, j);
            }
        }
    }
}

/* Printing Properties of Cell(s) */

/* Prints the candidates for a cell */
void print_possibilities(int r, int c) {
    printf("\nPossibilities are: ");
    for (int i = 1; i < 10; i++) {
        if (grid[r][c][i]) { printf("%i, ", i); }
    }
}

/* Printing the Grid */

//WANT TO TIDY THESE UP INTO ONE FUNCTION, BUT WOULD NEED TO PASS A FUNCTION TO A FUNCTION AND UNSURE HOW TO DO THAT!!!!

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
                    printf("%i ", grid[i][j][10]);
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

/* Printing a grid to show which cells are in 'coords' struct */
void print_grid_coords(coords cells) {
    printf("----------------------\n");
    for (int i = 0; i < 9; i++) {

        if (i % 3 == 0 && i != 0) {
            printf("-------+------+-------\n");
        }

        for (int j = 0; j < 9; j++) {
            
            if (j % 3 == 0) {
                printf("|");
            }

            // NICER WAY TO WRITE THIS>?????!!!!!
            int rc[2] = {i, j};
            if (coord_in_array(rc, cells)) {
                printf("X ");
            } else {
                printf(". ");
            }

            
        }
        printf("|\n");
    }
    printf("----------------------\n");
}

/* Returns 'TRUE' if grid is solved */
int check_finished() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!grid[i][j][10]) { return 0; }
        }
    }
    return 1;
}