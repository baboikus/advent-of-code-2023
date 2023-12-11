#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Galaxy {
  size_t row_idx;
  size_t column_idx;
} galaxy_t;

size_t shortes_path(galaxy_t *g1, galaxy_t *g2) {
  size_t len = 0;
  len += g1->row_idx > g2->row_idx ? g1->row_idx - g2->row_idx
                                   : g2->row_idx - g1->row_idx;

  len += g1->column_idx > g2->column_idx ? g1->column_idx - g2->column_idx
                                         : g2->column_idx - g1->column_idx;

  return len;
}

size_t solve(char *part, size_t age) {
  FILE *f;
  f = fopen("input", "r");

  char buffer[256];

  size_t n_rows = 0;
  size_t n_columns = 0;
  size_t rows_galaxy_present[256];
  size_t columns_galaxy_present[256];

  for (size_t i = 0; i < 256; ++i) {
    rows_galaxy_present[i] = age;
    columns_galaxy_present[i] = age;
  }

  galaxy_t galaxies[512];
  size_t n_galaxies = 0;

  while (fgets(buffer, 256, f)) {
    char *line = buffer;
    n_columns = 0;
    for (; *line && *line != '\n'; ++line) {
      if (*line == '#') {
        galaxies[n_galaxies++] =
            (galaxy_t){.row_idx = n_rows, .column_idx = n_columns};
        rows_galaxy_present[n_rows] = 0;
        columns_galaxy_present[n_columns] = 0;
      }
      ++n_columns;
    }
    ++n_rows;
  }

  size_t galaxies_present_acc = 0;
  for (size_t i = 0; i < n_rows; ++i) {
    galaxies_present_acc += rows_galaxy_present[i];
    rows_galaxy_present[i] = galaxies_present_acc;
  }
  galaxies_present_acc = 0;
  for (size_t i = 0; i < n_columns; ++i) {
    galaxies_present_acc += columns_galaxy_present[i];
    columns_galaxy_present[i] = galaxies_present_acc;
  }

  for (size_t i = 0; i < n_galaxies; ++i) {
    galaxy_t *g = &galaxies[i];
    g->row_idx += rows_galaxy_present[g->row_idx];
    g->column_idx += columns_galaxy_present[g->column_idx];
  }

  size_t part_sum = 0;
  for (size_t i = 0; i < n_galaxies; ++i) {
    for (size_t j = i + 1; j < n_galaxies; ++j) {
      part_sum += shortes_path(&galaxies[i], &galaxies[j]);
    }
  }

  printf("%s sum=%ld\n", part, part_sum);
  return part_sum;
}

int main() {
  assert(solve("part1", 1) == 9591768);
  solve("part2", 1000000 - 1);
  return 0;
}
