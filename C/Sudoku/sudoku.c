#include <stdio.h>
#include "maths.h"

/* Declare global variables */
int grid[9][9][11];
int row[9][9][2];
int col[9][9][2];
int box[9][9][2];

// Grid will have 9x9x11, of which index 0 is the count of possible thing, 1-9 are the flags for if it's possible for 
// that digit, and 10 will be flag for if it's been declared a solved cell (in which it will remove all possibilities from cells it interacts ith)



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

void x_remove(int x, int unit[9][2]) {
    int m;
    int n;
    for (int i = 0; i < 9; i++) {
        m = unit[i][0];
        n = unit[i][1];

        if (!grid[m][n][10]) {
            grid[m][n][x] = 0;
            grid[m][n][0] = get_sum_subsection(grid[m][n], 1, 10);
        }
    }
}

void new_solved_number(int r, int c) {
    
    /* Extract what the new digit is */
    int x = grid[r][c][10];

    /* Remove 'x' as a possibility from its row, column and box */
    x_remove(x, row[r]);
    x_remove(x, col[c]);
    x_remove(x, box[calc_box(r, c)]);

}

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

void print_grid() {
    printf("----------------------\n");
    for (int i = 0; i < 9; i++) {

        if (i % 3 == 0 && i != 0) {
            printf("-------+------+-------\n");
        }

        for (int j = 0; j < 9; j++) {
            
            if (j % 3 == 0) {
                printf("|");
            }

            if (grid[i][j][10]) {
                for (int k = 1; k < 10; k++) {
                    if (grid[i][j][k]) { printf("%i ", k); }
                }
            } else {
                printf(". ");
            }
            // printf("%i ", grid[i][j][0]);
        }
        printf("|\n");
    }
    printf("----------------------\n");
}

void print_possibilities(int r, int c) {
    printf("Possibilities are: ");
    for (int i = 1; i < 10; i++) {
        if (grid[r][c][i]) { printf("%i, ", i); }
    }
}



int naked_singles() {
    int prog = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j][0] == 1 && !grid[i][j][10]) {
                for (int k = 1; k < 10 && !grid[i][j][10]; k++) {
                    if (grid[i][j][k]) { 
                        grid[i][j][10] = k; 
                        printf("Naked single: placed %i at (%i, %i)\n", k, i, j); 
                    }
                }
                new_solved_number(i, j);
                prog = 1;
            }
        }
    }

    return prog;
}

void solve_digit(int x, int r, int c) {
    for (int i = 1; i < 10; i++) {
        grid[r][c][i] = 0;
    }
    grid[r][c][0] = 1;
    grid[r][c][x] = 1;
    grid[r][c][10] = x;

    new_solved_number(r, c);
}

// Can speed up in the future by maybe not checking every single digit - maybe find which digits in unit have been solved already
int check_unique(int unit[9][2], char* type, int index) {
    int m, n, y, a, b;
    int prog = 0;
    for (int x = 1; x < 10; x++) {
        int uniq = 0;
        int solved = 0;
        for (int i = 0; i < 9; i++) {
            m = unit[i][0];
            n = unit[i][1];
            if (grid[m][n][10] == x) { solved = 1; break; }
            if (grid[m][n][x]) { uniq++; y = x; a = m; b = n; }
        }
        if (uniq == 1 && !solved) {
            solve_digit(y, a, b);
            printf("Unique digit: placed %i at (%i, %i), unique in %s %i\n", y, a, b, type, index);
            prog = 1;
        }
    }

    return prog;
}

int unique() {
    int prog = 0;

    for (int i = 0; i < 9; i++) {
        prog = check_unique(row[i], "row", i) || prog;
        prog = check_unique(col[i], "col", i) || prog;
        prog = check_unique(box[i], "box", i) || prog;
    }

    return prog;
}


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
    };

    init_grid(array);

    /* A flag to know if any progress is being made */
    int progress = 1;
    while (progress) { 
        progress = naked_singles();
        progress = progress || unique();
    }

    print_grid();

    return 0;

}