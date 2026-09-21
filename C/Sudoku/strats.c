/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include "maths.h"

/* Global Variables */
extern int grid[9][9][11];
extern int row[9][9][2];
extern int col[9][9][2];
extern int box[9][9][2];

/* Cleaning Up */

/* Removes 'x' as a candidate from all un-solved cells in 'unit' */
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

/* Updates the rest of the grid's candidates */
void new_solved_number(int r, int c) {
    
    /* Extract what the new digit is */
    int x = grid[r][c][10];

    /* Remove 'x' as a possibility from its row, column and box */
    x_remove(x, row[r]);
    x_remove(x, col[c]);
    x_remove(x, box[calc_box(r, c)]);

}

/* Initiates grid clean up when a digit, 'x', is solved */
void solve_digit(int x, int r, int c) {

    /* Removes all other candidates for the solved cell */
    for (int i = 1; i < 10; i++) {
        grid[r][c][i] = 0;
    }

    /* Sets the flags of the solved cell */
    grid[r][c][0] = 1;
    grid[r][c][x] = 1;
    grid[r][c][10] = x;

    /* Updates the grid */
    new_solved_number(r, c);
}

// Function to check if any cells other than 2 have candidates x & y (will update to be variable number of each!!!!)
int check_cand_excl(int unit[9][2], int i, int j, int x, int y) {
    int flag = 0;
    int m, n;

    for (int a = 0; a < 9 && !flag; a++) { 
        if (!(a == i || a == j)) {
            m = unit[a][0];
            n = unit[a][1];
            flag = flag || grid[m][n][x]; // x is the number to be excluded
            flag = flag || grid[m][n][y];
        }
    }

    return flag;
}

// Function to remove a pair of candidates from all cells bar the original pair cells; UPDATE FOR UNLIMITED VARIABLES!!!!
void cleanup_cand_excl(int unit[9][2], int i, int j, int x, int y) {
    int m, n;

    for (int a = 0; a < 9; a++) { 
        if (!(a == i || a == j)) {
            // printf("%i, ", a); // !!!!!!
            m = unit[a][0];
            n = unit[a][1];
            grid[m][n][x] = 0;
            grid[m][n][y] = 0;
            grid[m][n][0] = get_sum_subsection(grid[m][n], 1, 10);
        }
    }
}

/* Misc */

/* A function to return true if every potential candidate of 2 cells is identical */
int compare_candidates(int r1, int c1, int r2, int c2) {
    int flag = 1;

    for (int i = 1; i < 10; i++) {
        flag = flag && (grid[r1][c1][i] == grid[r2][c2][i]);
    }

    return flag;
}

/* Gets the list of candidates from a cell */
/* NOTE: Remember to free(cands) after use */
int* get_cands(int r, int c) {
    int num_cands = 0;
    int* cands = malloc(num_cands * sizeof(int));

    for (int i = 1; i < 10; i++) {
        if (grid[r][c][i]) {
            num_cands++;
            cands = realloc(cands, num_cands * sizeof(int));
            cands[num_cands - 1] = i;
         }
    }

    return cands;
}

/* Level 0 Strategies */

int naked_single() {
    int prog = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (grid[i][j][0] == 1 && !grid[i][j][10]) {
                for (int k = 1; k < 10 && !grid[i][j][10]; k++) {
                    if (grid[i][j][k]) { 
                        grid[i][j][10] = k; 
                        printf("Naked Single: placed %i at (%i, %i)\n", k, i, j); 
                    }
                }
                new_solved_number(i, j);
                prog = 1;
            }
        }
    }

    return prog;
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
            printf("Unique Digit: placed %i at (%i, %i), unique in %s %i\n", y, a, b, type, index);
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

/* Level 1 Strategies */

int check_naked_pair(int unit[9][2], char* type, int index) {
    // Need to find naked pair, then remove those 2 digits from the candidates of every other cell in that unit - no prog if nothing else is removed
    // There's also naked triples and things like that... could universalise the code to cope with those?

    /* Find all cells with explicitly 2 candidates */
    int m, n;
    int count = 0;
    int *list_possible = malloc(count * sizeof(int)); // Don't need but helps me understand for now

    int flag = 0; // To be returned !!!!!

    for (int i = 0; i < 9; i++) {
        m = unit[i][0];
        n = unit[i][1];
        if (grid[m][n][0] == 2) {
            count++;
            list_possible = realloc(list_possible, sizeof(int) * count);
            list_possible[count - 1] = i;
        }
    }

    // for (int i = 0; i < count; i++) { // !!!!!!!
    //     printf("%i, ", list_possible[i]);
    // }

    // MAYBE STORE FOUND NAKED PAIRS SO THEY DON'T KEEP FLAGGING???

    for (int i = 0; i < count; i++) {
        for (int j = 0; j < i; j++) {
            // printf("\n(%i, %i): %i", list_possible[i], list_possible[j], compare_candidates(unit[list_possible[i]][0], unit[list_possible[i]][1], unit[list_possible[j]][0], unit[list_possible[j]][1]));
            if (compare_candidates(unit[list_possible[i]][0], unit[list_possible[i]][1], unit[list_possible[j]][0], unit[list_possible[j]][1])) {
                
                int* cands = get_cands(unit[list_possible[i]][0], unit[list_possible[i]][1]);
                if (check_cand_excl(unit, list_possible[i], list_possible[j], cands[0], cands[1])) {
                    flag = 1;
                    cleanup_cand_excl(unit, list_possible[i], list_possible[j], cands[0], cands[1]);
                    printf("Naked Pair: found and useful\n"); // ADD MORE DETAILS LATER !!!!!
                } else {
                    printf("Naked Pair: found but not useful\n" ); // ADD MORE DETAILS LATER !!!!!
                }                

                free(cands);
            }
        }
    }

    free(list_possible);

    return flag; ////// CHANGE!!!!!!
}

int naked_pair() {
    int prog = 0;

    for (int i = 0; i < 9; i++) {
        prog = check_naked_pair(row[i], "row", i) || prog;
        prog = check_naked_pair(col[i], "col", i) || prog;
        prog = check_naked_pair(box[i], "box", i) || prog;
    }

    return prog;
}