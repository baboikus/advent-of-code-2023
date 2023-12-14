#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t find_mirror(size_t *arr, size_t n_arr, size_t prev_center) {
  char mirror_found = 0;
  for (int center = 0; center < n_arr; ++center) {
    mirror_found = 0;
    for (int t = center, b = center + 1; t >= 0 && b < n_arr; --t, ++b) {
      if (arr[t] != arr[b]) {
        mirror_found = 0;
        break;
      }
      mirror_found = 1;
    }

    if (mirror_found) {
      if (prev_center > 0 && (center + 1 != prev_center)) {
        return center + 1;
        break;
      } else if (prev_center == 0) {
        return center + 1;
        break;
      }
    }
  }

  return 0;
}

void part1_part2() {
  FILE *f = fopen("input", "r");
  char buffer[256];

  size_t part1_h_sum = 0;
  size_t part1_v_sum = 0;
  size_t part2_h_sum = 0;
  size_t part2_v_sum = 0;
  while (!feof(f)) {
    size_t rows[64];
    size_t columns[64];

    memset(rows, 0, sizeof(size_t) * 64);
    memset(columns, 0, sizeof(size_t) * 64);

    size_t n_rows = 0;
    size_t n_columns = 0;

    while (fgets(buffer, 256, f)) {
      if (buffer[0] == '\n')
        break;

      for (n_columns = 0; buffer[n_columns] && buffer[n_columns] != '\n';
           ++n_columns) {
        if (buffer[n_columns] == '#') {
          rows[n_rows] |= 1 << n_columns;
          columns[n_columns] |= 1 << n_rows;
        }
      }

      n_rows++;
    }

    size_t part1_h_center = find_mirror(rows, n_rows, 0);
    part1_h_sum += part1_h_center;
    size_t part1_v_center = find_mirror(columns, n_columns, 0);
    part1_v_sum += part1_v_center;
    assert((part1_h_center == 0 && part1_v_center != 0) ||
           (part1_h_center != 0 && part1_v_center == 0));

    for (size_t r = 0; r < n_rows; ++r) {
      for (size_t c = 0; c < n_columns; ++c) {
        size_t old_row = rows[r];
        size_t old_column = columns[c];
        rows[r] ^= 1 << c;
        columns[c] ^= 1 << r;


        size_t part2_h_center = find_mirror(rows, n_rows, part1_h_center);
        size_t part2_v_center = find_mirror(columns, n_columns, part1_v_center);

        rows[r] ^= 1 << c;
        columns[c] ^= 1 << r;

        assert(rows[r] == old_row);
        assert(columns[c] == old_column);

        if (part2_h_center > 0) {
          //printf("h %ld fix r=%ld c=%ld\n", part2_h_center, r, c);
          part2_h_sum += part2_h_center;
          goto next_pattern;
        }
        if (part2_v_center > 0) {
          //printf("v %ld fix r=%ld c=%ld\n", part2_v_center, r, c);
          part2_v_sum += part2_v_center;
          goto next_pattern;
        }
      }
    }
    part2_h_sum += part1_h_center;
    part2_v_sum += part1_v_center;

  next_pattern:;
  }

  printf("part1 sum=%ld\n", part1_h_sum * 100 + part1_v_sum);
  printf("part2 sum=%ld\n", part2_h_sum * 100 + part2_v_sum);

  assert((part1_h_sum * 100 + part1_v_sum) == 35691);
  assert((part2_h_sum * 100 + part2_v_sum) == 39037);
}

int main() {
  part1_part2();
  return 0;
}
