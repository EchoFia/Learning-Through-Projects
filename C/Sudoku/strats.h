/* Cleaning Up */
void x_remove(int x, int unit[9][2]);
void solve_digit(int x, int r, int c);
void new_solved_number(int r, int c);
int check_cand_excl(int unit[9][2], int i, int j, int x, int y);
void cleanup_cand_excl(int unit[9][2], int i, int j, int x, int y);

/* Misc */
int compare_candidates(int r1, int c1, int r2, int c2);
int* get_cands(int r, int c);

/* Level 0 Strategies */
int naked_single();
int check_unique(int unit[9][2], char* type, int index);
int unique();

/* Level 1 Strategies */
int check_naked_pair(int unit[9][2], char* type, int index);
int naked_pair();