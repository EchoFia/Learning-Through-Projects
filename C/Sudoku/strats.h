/* Co-ords Handling */
typedef struct coords coords;

/* A VLA for storing co-ords */
struct coords {
    int count;
    int** arr;
};

coords init_coords();
coords add_coord(coords coords, int rc[2]);
void del_coords(coords coords);
coords unit_to_coords(int unit[9][2]);

/* Cleaning Up */
void x_remove_single(int x, int r, int c);
void x_remove_block(int x, coords cells);
void x_remove_unit_unsolv(int x, int unit[9][2]);
void new_solved_digit(int r, int c);
void solve_digit(int x, int r, int c);

/* Retrieving Info from the Grid */
int* get_cands(int r, int c);
int comp_cands(int rc1[2], int rc2[2]);
int x_freq_block(int x, coords cells);
int x_freq_unit(int x, int unit[9][2]);
int x_solved_block(int x, coords cells);
int x_solved_unit(int x, int unit[9][2]);
int* find_x_cand_unit(int unit[9][2], int x);

/* Misc - REORGANISE */
int compare_arrays(int* a, int* b, int len_a, int len_b);
int int_in_array(int x, int* arr, int length);
int coord_in_array(int rc[2], coords cells);

/* Level 0 Strategies */
int naked_single();
int check_unique_cand(int unit[9][2], char* type, int index);
int unique_cand();

/* Generalised Functions for Level 1 & 2 Strategies */
int check_naked_set(int z, int unit[9][2], char* type, int index);
int naked_set(int z);
int check_hidden_set(int z, int unit[9][2], char* type, int index);
int hidden_set(int z);

/* Level 1 Strategies */
int naked_pair();
int hidden_pair();
coords non_overlap_blocks(coords cells_a, coords cells_b);
coords non_overlap_units(int unit_a[9][2], int unit_b[9][2]);
int check_overlapping_units(int unit_a[9][2], int unit_b[9][2], char* a_type, int a_index, char* b_type, int b_index, char* strat_name);
int box_line();
int pointing_line();

/* Level 2 Strategies */
int naked_triple();
int naked_quadruple();
int hidden_triple();