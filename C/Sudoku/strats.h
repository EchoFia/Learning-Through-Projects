/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Co-ords Handling */
typedef struct coords coords;

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* A VLA for storing co-ords */
struct coords {
    int count;
    int** arr;
};

coords init_coords();
coords add_coord(coords coords, int rc[2]);
void del_coords(coords coords);
coords remove_last_coord(coords cells);
coords unit_to_coords(int unit[9][2]);
coords get_touching_block(int rc[2]);

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Chain Handling */
typedef struct chain chain;

/* Will store the needed information as the XY-Wing is built */
/* NOTE: Chains are between cells that go AB->BC->CD->etc..., and so 'digits' will store {A, B, C, D, ...} */
/* As such, 'digits' will be 1 longer than 'chain_len' */
struct chain {
    int chain_len;
    coords cells;
    int* digits;
};

chain init_chain(int rc[2], int digit_index);
chain add_chain(chain curr_chain, int rc[2], int digit);
void del_chain(chain curr_chain);
chain remove_child(chain curr_chain);

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Cleaning Up */
void x_remove_single(int x, int r, int c);
void x_remove_block(int x, coords cells);
void x_remove_unit_unsolv(int x, int unit[9][2]);
void new_solved_digit(int r, int c);
void solve_digit(int x, int r, int c);

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Retrieving Info from the Grid */
int* get_cands(int r, int c);
int comp_cands(int rc1[2], int rc2[2]);
int x_freq_block(int x, coords cells);
int x_freq_unit(int x, int unit[9][2]);
int x_solved_block(int x, coords cells);
int x_solved_unit(int x, int unit[9][2]);
int* find_x_cand_unit(int unit[9][2], int x); // CHANGE ORDER

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Misc - REORGANISE */
int compare_arrays(int* a, int* b, int len_a, int len_b);
int int_in_array(int x, int* arr, int length);
int coord_in_array(int rc[2], coords cells);

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Level 0 Strategies */
int naked_single();
int check_unique_cand(int unit[9][2], char* type, int index);
int unique_cand();

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Generalised Functions for Level 1 & 2 Strategies */
int check_naked_set(int z, int unit[9][2], char* type, int index);
int naked_set(int z);
int check_hidden_set(int z, int unit[9][2], char* type, int index);
int hidden_set(int z);

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Level 1 Strategies */
int naked_pair();
int hidden_pair();
coords non_overlap_blocks(coords cells_a, coords cells_b);
coords non_overlap_units(int unit_a[9][2], int unit_b[9][2]);
int check_overlapping_units(int unit_a[9][2], int unit_b[9][2], char* a_type, int a_index, char* b_type, int b_index, char* strat_name);
int box_line();
int pointing_line();

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Generalised Functions for Level 2 & 3 Strategies */
int check_lines_z(int z, char* type, char* strat_name);

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Level 2 Strategies */
int naked_triple();
int naked_quadruple();
int hidden_triple();
int x_wing();

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= */
/* Level 3 Strategies */
int lines_3();
int lines_4();
void print_xy_chain(chain curr_chain, int x, int start);
coords find_children_block (chain curr_chain, int rc[2]);
int check_chain(int new_cell[2]);
int xy_chain();
coords find_corner(int x, coords cells, int box_num);
int find_rectangles(int x, coords corner_cells);
int rectangles();

