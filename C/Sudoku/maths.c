int get_sum_subsection(int arr[], int start, int end) {
    int sum = 0;
    for (int i = start; i < end; i++) {
        sum += arr[i];
    }
    return sum;
}

int calc_box(int r, int c) {
    int box = r / 3 * 3 + c / 3;
    return box;
}

int calc_box_index(int r, int c) {
    int index = r % 3 * 3 + c % 3;
    return index;
}