/* Libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strats.h"
#include "maths.h"
#include "grid.h"

/* Import Global Variables */
extern int grid[9][9][11];
extern int row[9][9][2];
extern int col[9][9][2];
extern int box[9][9][2];

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Co-ords Handling */

coords init_coords() {
    coords cells;
    cells.count = 0;
    cells.arr = malloc(cells.count * sizeof(int*));
    cells.arr[0] = malloc(cells.count * 2 * sizeof(int));
    return cells;
}

/* NOTE: Hardcoded for co-ords to be 2D */
coords add_coord(coords cells, int rc[2]) {
    cells.count++;
    cells.arr = realloc(cells.arr, cells.count * sizeof(int*));

    cells.arr[0] = realloc(cells.arr[0], cells.count * 2 * sizeof(int));
    /* All the memory is allocated to arr[0], which means the whole grid is stored together */
    /* As such, points to arr[i] will be right after arr[0] in memory */
    for(int i = 1; i < cells.count; i++) {
        cells.arr[i] = cells.arr[0] + i * 2; // 2 because coords have dim 2
    }

    cells.arr[cells.count-1][0] = rc[0];
    cells.arr[cells.count-1][1] = rc[1];
    return cells;
}

void del_coords(coords cells) {
    /* Order is important to free() inwards out */
    free(cells.arr[0]);
    free(cells.arr);
}

/* Creates a 'cords' struct for a unit */
coords unit_to_coords(int unit[9][2]) {
    coords cells = init_coords();
    for (int i = 0; i < 9; i++) {
        cells = add_coord(cells, unit[i]);
    }
    return cells;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Cleaning Up Grid */

///// MAYBE MAKE SO IT CHECKS IF IT REMOVED ANYTHING????!!!!
/* Removes 'x' as a candidate from cell (r, c) */
void x_remove_single(int x, int r, int c) {
    grid[r][c][x] = 0;
    grid[r][c][0] = get_sum_subsection(grid[r][c], 1, 10);
}

/* Removes 'x' as a candidate from all cells in 'coords' */
void x_remove_block(int x, coords cells) {
    for (int i = 0; i < cells.count; i++) {
        x_remove_single(x, cells.arr[i][0], cells.arr[i][1]);
    }
}

/* Removes 'x' as a candidate from all un-solved cells in 'unit' */
// MAYBE USE THE BLOCK FUNCTION???!!!!
void x_remove_unit_unsolv(int x, int unit[9][2]) {
    for (int i = 0; i < 9; i++) {

        if (!grid[unit[i][0]][unit[i][1]][10]) {
            x_remove_single(x, unit[i][0], unit[i][1]);
        }
    }
}

/* Updates the rest of the grid's candidates */
void new_solved_digit(int r, int c) {
    
    /* Extract what the new digit is */
    int x = grid[r][c][10];

    /* Remove 'x' as a possibility from its row, column and box */
    x_remove_unit_unsolv(x, row[r]);
    x_remove_unit_unsolv(x, col[c]);
    x_remove_unit_unsolv(x, box[calc_box(r, c)]);
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
    new_solved_digit(r, c);
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Retrieving Info from the Grid */

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

/* A function to return true if every potential candidate of 2 cells is identical */
int comp_cands(int rc1[2], int rc2[2]) {
    int result = 1;

    int r1 = rc1[0];
    int r2 = rc2[0];
    int c1 = rc1[1];
    int c2 = rc2[1];

    for (int i = 1; i < 10; i++) {
        result = result && (grid[r1][c1][i] == grid[r2][c2][i]);
    }

    return result;
}

/* Calculates frequency of 'x' in a set of 'coords' */
int x_freq_block(int x, coords cells) {
    int count = 0;

    for (int i = 0; i < cells.count; i++) {
        count += grid[cells.arr[i][0]][cells.arr[i][1]][x];
    }

    return count;
}

/* Calculates frequency of 'x' in 'unit' */
int x_freq_unit(int x, int unit[9][2]) {
    coords cells = unit_to_coords(unit);
    int count = x_freq_block(x, cells);
    del_coords(cells);
    return count;
}

/* Returns 1 if 'x' is solved in 'cells' */
int x_solved_block(int x, coords cells) {
    int result = 0;
    for (int i = 0; i < cells.count && !result; i++) {
        int *cands = grid[cells.arr[i][0]][cells.arr[i][1]];
        result = (cands[10] > 0) && cands[x];
    }

    return result;
}

///// CAN I JUST HAVE ONE FUNCTION THAT CAN BE PASSED ANOTHER FUNCTION??? TO DO ALL OF THESE BLOCK TO UNIT THINGS???!!!

/* Returns 1 if 'x' is solved in 'unit' */
int x_solved_unit(int x, int unit[9][2]) {
    coords cells = unit_to_coords(unit);
    int count = x_solved_block(x, cells);
    del_coords(cells);
    return count;
}

/* Find which cells in a unit have 'x' as a candidate */
/* NOTE: Remember to free(indices) after use */
int* find_x_cand_unit(int unit[9][2], int x) {
    int count = 0;
    int* indices = malloc(count * sizeof(int));
    int m, n;

    for (int i = 0; i < 9; i++) {
        m = unit[i][0];
        n = unit[i][1];
        if (grid[m][n][x]) {
            count++;
            indices = realloc(indices, count * sizeof(int));
            indices[count - 1] = i;
        }
    }

    return indices;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Misc */

int compare_arrays(int* a, int* b, int len_a, int len_b) {
    if (len_a != len_b) {
        return 0;
    } else {
        for (int i = 0; i < len_a; i++) {
            if (a[i] != b[i]) { return 0; }
        }
        return 1;
    }
}

/* Returns 'TRUE' if 'x' is in 'arr' */
// Anyway to have it calculate LEN?? I WAS STRUGGLING WITH IT BEING A POINTER!!!!!
int int_in_array(int x, int* arr, int length) {
    int result = 0;
    for (int i = 0; i < length && !result; i++) {
        result = (x == arr[i]);
    }
    return result;
}

/* Returns 'TRUE' if the cell 'rc[2]' is in 'cells' */
int coord_in_array(int rc[2], coords cells) {
    int result = 0;
    for (int i = 0; i < cells.count && !result; i++) {
        result = (rc[0] == cells.arr[i][0] && rc[1] == cells.arr[i][1]);
    }
    return result;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Level 0 Strategies */

/* Loops through the grid to look for Naked Singles */
int naked_single() {

    /* Flag to track if anything in the grid has been updated */
    int prog = 0;

    /* Loops through all cells */
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {

            /* If only one candidate but cell isn't labelled as solved */
            if (grid[i][j][0] == 1 && !grid[i][j][10]) {

                /* Finding which candidate is left */
                for (int k = 1; k < 10 && !grid[i][j][10]; k++) {
                    if (grid[i][j][k]) { 

                        /* Set the 'solved' index to the value of the cell */
                        grid[i][j][10] = k; 
                        printf("Naked Single: placed %i at (%i, %i)\n", k, i, j); 
                    }
                }

                /* Clean up the candidates that share a unit with the newly solved cell */
                new_solved_digit(i, j);
                prog = 1;
            }
        }
    }

    return prog;
}

/* Checks wihtin a unit if any digit is only a candidate for one cell */
int check_unique_cand(int unit[9][2], char* type, int index) {

    /* Flag to track if anything in the grid has been updated */
    int prog = 0;

    for (int x = 1; x < 10; x++) {
        /* If 'x' isn't already solved in the unit, and only appears once */
        if (!x_solved_unit(x, unit) && x_freq_unit(x, unit) == 1) {
            
            /* Finding which cell 'x' is a candidate for */
            for (int i = 0; i < 9; i++) {
                if (grid[unit[i][0]][unit[i][1]][x]) { 

                    /* Add in the solved digit to the cell */
                    solve_digit(x, unit[i][0], unit[i][1]);

                    printf("Unique Candidate: placed %i at (%i, %i), unique in %s %i\n", x, unit[i][0], unit[i][1], type, index);
                    prog = 1;
                }
            }
        }
    }

    return prog;
}

/* Loops through the grid to look for units with a Unique Candidate */
int unique_cand() {
    
    /* Flag to track if anything in the grid has been updated */
    int prog = 0;

    for (int i = 0; i < 9; i++) {
        prog = check_unique_cand(row[i], "row", i) || prog;
        prog = check_unique_cand(col[i], "col", i) || prog;
        prog = check_unique_cand(box[i], "box", i) || prog;
    }

    return prog;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Generalised Functions for Level 1 & 2 Strategies */

/* Checks for a naked set of size 'z', i.e. if 'z' is 2, it'll be a Naked Pair */
/* NOTE: Can only find one naked set each time it's passed */
int check_naked_set(int z, int unit[9][2], char* type, int index) {

    /* Check that 'z' is in range */
    if ((z < 1) || (z > 4)) {
        printf("Error: z out of range\n");
        return 0;
    }

    /* Flag to track if anything in the grid has been updated */
    int prog = 0;

    int count = 0;

    /* z_cands will be indices relative to 'unit' */
    int* z_cands = malloc(count * sizeof(int)); // Evaluates to 0 but helps with readability

    /* Find all cells with explicitly 'z' candidates */
    for (int i = 0; i < 9; i++) {
        if (grid[unit[i][0]][unit[i][1]][0] == z) {
            count++;
            z_cands = realloc(z_cands, count * sizeof(int));
            z_cands[count - 1] = i;
        }
    }

    /* If there's less than 'z' cells with 'z' candidates, there's no chance for a naked set of size 'z' */
    if (count >= z) {

        /* Need to have 'z' matching cells with exactly the same candidates */
        for (int i = 0; i < count; i++) {
            int num_matching = 1;
            
            /* Will store the indices of all matching cells, indexing relative to 'unit' still */
            int* matching = malloc(num_matching * sizeof(int));
            matching[0] = z_cands[i];

            /* Can end loop early if there's already enough cells that match */
            for (int j = 0; j < i && num_matching < z; j++) {
                if (comp_cands(unit[z_cands[i]], unit[z_cands[j]])) {
                    num_matching++;
                    matching = realloc(matching, num_matching * sizeof(int));
                    matching[num_matching - 1] = z_cands[j];
                }
            }

            /* If enough cells match, we've found a naked set */
            if (num_matching >= z) {

                int* naked_nums = get_cands(unit[matching[0]][0], unit[matching[0]][1]);
                /* NOTE: There will be 'z' candidates by definition */

                /* Get cells in the unit but not in the naked set */
                coords cells = init_coords();
                for (int j = 0; j < 9; j++) {
                    if (!int_in_array(j, matching, z)) {
                        cells = add_coord(cells, unit[j]);
                    }
                }

                /* For each number in the naked set, check if there's any candidates to remove */
                for (int j = 0; j < z; j++) {
                    int x = naked_nums[j];
                    prog = prog || (x_freq_block(x, cells) > 0);
                    x_remove_block(x, cells);
                }

                // CAN MAYBE HAVE A LIST OF KNOWN NAKED SETS TO NOT ALWAYS BE FINDING IT AGAIN

                /* If any candidates were actually removed */
                if (prog) {
                    char* terms[3] = {"Pair", "Triple", "Quadruple"};
                    printf("Naked %s: found in %s %i\n", terms[z-2], type, index); // ADD MORE DETAILS LATER !!!!!
                }

                free(naked_nums);
                del_coords(cells);
            }

            free(matching);
        }
    }

    free(z_cands);
    return prog;

}

int naked_set(int z) {
    int prog = 0;

    for (int i = 0; i < 9; i++) {
        prog = check_naked_set(z, row[i], "row", i) || prog;
        prog = check_naked_set(z, col[i], "col", i) || prog;
        prog = check_naked_set(z, box[i], "box", i) || prog;
    }

    return prog;
}

int check_hidden_set(int z, int unit[9][2], char* type, int index) {

    /* Check that 'z' is in range */
    if ((z < 1) || (z > 3)) {
        printf("Error: z out of range\n");
        return 0;
    }

    /* Flag to track if anything in the grid has been updated */
    int prog = 0;

    int count = 0;

    /* Check what digits only appear 'z' times in the unit */
    int* digits = malloc(count * sizeof(int));

    for (int i = 1; i < 10; i++) {
        if (x_freq_unit(i, unit) == z) {
            count++;
            digits = realloc(digits, count * sizeof(int));
            digits[count - 1] = i;
        }
    }

    if (count >= z) {

        /* Need to have 'z' digits only present in the same 'z' cells */
        for (int i = 0; i < count; i++) {

            int num_matching = 1;
            
            /* Will store the digits that match in where their candidates are in 'unit' */
            int* matching = malloc(num_matching * sizeof(int));
            matching[0] = digits[i];

            /* Find which indcies relative to 'unit' have 'digits[i] */
            int* inds_i = find_x_cand_unit(unit, digits[i]);

            /* Can end loop early if there's already enough cells that match */
            for (int j = 0; j < i && num_matching < z; j++) {

                /* Find which indcies relative to 'unit' have 'digits[i] */
                int* inds_j = find_x_cand_unit(unit, digits[j]);
                
                if (compare_arrays(inds_i, inds_j, z, z)) {
                    num_matching++;
                    matching = realloc(matching, num_matching * sizeof(int));
                    matching[num_matching - 1] = digits[j];
                }

                free(inds_j);

            }

            if (num_matching >= z) {

                /* Convert the 'unit' indices in 'inds_i' to a coords struct */
                coords cells = init_coords();
                for (int j = 0; j < z; j++) {
                    cells = add_coord(cells, unit[inds_i[j]]);
                }

                /* Remove all other digits than those in 'matching' */
                for (int x = 1; x < 10; x++) {
                    if (!int_in_array(x, matching, z)) {
                        prog = prog || (x_freq_block(x, cells) > 0);
                        x_remove_block(x, cells);
                    }
                }

                /* If any candidates were actually removed */
                if (prog) {
                    char* terms[2] = {"Pair", "Triple"};
                    printf("Hidden %s: found in %s %i\n", terms[z-2], type, index); // ADD MORE DETAILS LATER !!!!!
                }

                del_coords(cells);
            }

            free(matching);
            free(inds_i);

        }

    }

    free(digits);
    return prog;
}

int hidden_set(int z) {
    int prog = 0;

    for (int i = 0; i < 9; i++) {
        prog = check_hidden_set(z, row[i], "row", i) || prog;
        prog = check_hidden_set(z, col[i], "col", i) || prog;
        prog = check_hidden_set(z, box[i], "box", i) || prog;
    }

    return prog;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Level 1 Strategies */

int naked_pair() {
    int prog = naked_set(2);
    return prog;
}

int hidden_pair() {
    int prog = hidden_set(2);
    return prog;
}


/* Will get the co-ords of 'cells_a' that don't overlap with 'cells_b' */
/* Make sure to del_coords() to clean up memory */
coords non_overlap_blocks(coords cells_a, coords cells_b) {

    coords cells_a_not_b = init_coords();

    for (int i = 0; i < cells_a.count; i++) {
        if (!coord_in_array(cells_a.arr[i], cells_b)) {
            cells_a_not_b = add_coord(cells_a_not_b, cells_a.arr[i]);
        }
    }

    return cells_a_not_b;
}


coords non_overlap_units(int unit_a[9][2], int unit_b[9][2]) {

    coords cells_a = unit_to_coords(unit_a);
    coords cells_b = unit_to_coords(unit_b);

    coords cells_a_not_b = non_overlap_blocks(cells_a, cells_b);

    del_coords(cells_a);
    del_coords(cells_b);

    return cells_a_not_b;

}


int check_overlapping_units(int unit_a[9][2], int unit_b[9][2], char* a_type, int a_index, char* b_type, int b_index, char* strat_name) {

    /* Flag to track if anything in the grid has been updated */
    int prog = 0;

    coords cells_a_not_b = non_overlap_units(unit_a, unit_b);
    coords cells_b_not_a = non_overlap_units(unit_b, unit_a);

    /* Checks if the units overlap */
    if(cells_a_not_b.count == 9) {
        printf("Error: Passed in units that don't overlap\n"); ///// ADD MORE DETAIL!!!!
        return 0;
    }

    for (int x = 1; x < 10; x++) {

        // Check if the digit is solved in either the box or line
        if (x_solved_unit(x, unit_a) || x_solved_unit(x, unit_b)) { continue; }

        else {

            /* Only if no 'x' candidates in 'cells_b_not_a' and are 'x' candidates to remove in 'cells_a_not_b */
            if (x_freq_block(x, cells_b_not_a) == 0 && x_freq_block(x, cells_a_not_b) > 0) {

                printf("%s: Digit %i must be in %s %i in %s %i\n", strat_name, x, b_type, b_index, a_type, a_index);

                /* Clean up the non-overlapping region of the box */
                x_remove_block(x, cells_a_not_b);
                prog = 1;
            }

            // int ran_var = x_freq_block(x, line_not_box);
            // printf("Number of %i's: %i, in %s %i but not box %i\n", x, ran_var, line_type, line_index, box_index);

        }
    }

    del_coords(cells_a_not_b);
    del_coords(cells_b_not_a);

    return prog;
}

int box_line() {
    int prog = 0;
    int row_index, col_index;

    for (int i = 0; i < 9; i++) {
        /* Rows for box 'i' */
        for (int j = 0; j < 3; j++) {
            row_index = i / 3 * 3 + j;
            col_index = i % 3 * 3 + j;
            prog = check_overlapping_units(box[i], row[row_index], "box", i, "row", row_index, "Box-Line") || prog;
            prog = check_overlapping_units(box[i], col[col_index], "box", i, "col", col_index, "Box-Line") || prog;
        }
    }

    return prog;
}

int pointing_line() {
    int prog = 0;
    int row_index, col_index;

    for (int i = 0; i < 9; i++) {
        /* Rows for box 'i' */
        for (int j = 0; j < 3; j++) {
            row_index = i / 3 * 3 + j;
            col_index = i % 3 * 3 + j;
            prog = check_overlapping_units(row[row_index], box[i], "row", row_index, "box", i, "Pointing Line") || prog;
            prog = check_overlapping_units(col[col_index], box[i], "col", col_index, "box", i, "Pointing Line") || prog;
        }
    }

    return prog;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Generalised Functions for Level 2 & 3 Strategies */

int check_lines_z(int z, char* type, char* strat_name) {

    int (*unit_a)[2];
    int (*unit_b)[2];

    if (!(strcmp(type, "row") == 0 || strcmp(type, "col") == 0)) {
        printf("Error: Incorrect type entered\n");
        return 0;
    }

    int prog = 0;

    for (int x = 1; x < 10; x++) {

        // JUST FOR COLUMNS FOR NOW
        for (int i = 0; i < 9; i++) {

            /* Equivalent to setting 'unit_a' to 'col[i]' OR 'row[i]' */
            if (strcmp(type, "row") == 0) {
                unit_a = row[i];
            } else {
                unit_a = col[i];
            }

            /* If there's more than 'z' candidates for 'x' in 'unit', can't be a Lines-Z */
            if (x_freq_unit(x, unit_a) > z) { continue; }

            int num_matching = 1;
            int* matching = malloc(num_matching * sizeof(int));
            matching[num_matching - 1] = i;

            
            int* inds_i = find_x_cand_unit(unit_a, x);

            for (int j = 0; j < i && num_matching < z; j++) {

                /* Equivalent to setting 'unit_b' to 'col[j]' OR 'row[j]' */
                if (strcmp(type, "row") == 0) {
                    unit_b = row[j];
                } else {
                    unit_b = col[j];
                }

                if (x_freq_unit(x, unit_b) > z) { continue; }

                int* inds_j = find_x_cand_unit(unit_b, x);

                if (compare_arrays(inds_i, inds_j, z, z)) {
                    num_matching++;
                    matching = realloc(matching, num_matching * sizeof(int));
                    matching[num_matching - 1] = j;
                }

                if (num_matching >= z) {

                    /* Convert the 'unit' indices in 'inds_i' to a coords struct */
                    coords cells = init_coords();

                    /* 'inds_i' will be the list of rows/cols (opposite to 'type') to remove from */
                    for (int k = 0; k < z; k++) {

                        for (int l = 0; l < 9; l++) {

                            /* If not in matching */
                            if (!int_in_array(l, matching, z)) {

                                int rc[2];
                                if (strcmp(type, "row") == 0) {
                                    rc[0] = l;
                                    rc[1] = inds_i[k];
                                } else {
                                    rc[0] = inds_i[k];
                                    rc[1] = l;
                                }

                                cells = add_coord(cells, rc);
                            }
                        }
                    }

                    // print_grid_coords(cells);

                    /* Remove 'x' from all 'cells' */
                    prog = prog || (x_freq_block(x, cells) > 0);
                    x_remove_block(x, cells);

                    /* If any candidates were actually removed */
                    if (prog) {
                        // CHANGE - HARD CODED FOR Z = 2!!!!! Need a function probs to just make a nice string
                        printf("Found %s for digit %i between %s %i and %s %i\n", strat_name, x, type, i, type, j);
                    }

                    del_coords(cells);

                }

                free(inds_j);

            }

            free(inds_i);
            free(matching);

        }

    }

    return prog;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Level 2 Strategies */

int naked_triple() {
    int prog = naked_set(3);
    return prog;
}

int naked_quadruple() {
    int prog = naked_set(4);
    return prog;
}

int hidden_triple() {
    int prog = hidden_set(3);
    return prog;
}

int x_wing() {
    int prog = check_lines_z(2, "row", "X-Wing");
    prog = check_lines_z(2, "col", "X-Wing") || prog;
    return prog;
}

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */

/* Level 3 Strategies */

int lines_3() {
    int prog = check_lines_z(3, "row", "Lines-3");
    prog = check_lines_z(3, "col", "Lines-3") || prog;
    return prog;
}

int lines_4() {
    int prog = check_lines_z(4, "row", "Lines-4");
    prog = check_lines_z(4, "col", "Lines-4") || prog;
    return prog; 
}