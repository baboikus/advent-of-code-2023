#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Platform {
  char map[256][256];
  size_t n_rows;
  size_t n_columns;
} platform_t;

void platform_fall_north_tick(platform_t *p, size_t t) {
  for (size_t r = 1; r < (p->n_rows - t); ++r) {
    for (size_t c = 0; c < p->n_columns; ++c) {
      if (p->map[r][c] == 'O' && p->map[r - 1][c] == '.') {
        p->map[r][c] = '.';
        p->map[r - 1][c] = 'O';
      }
    }
  }
}

void platform_fall_east_tick(platform_t *p, size_t t) {
  for (size_t i = 1, c = p->n_columns - 1 - i; i < p->n_columns - t; ++i, --c) {
    for (size_t r = 0; r < p->n_rows; ++r) {
      if (p->map[r][c] == 'O' && p->map[r][c + 1] == '.') {
        p->map[r][c] = '.';
        p->map[r][c + 1] = 'O';
      }
    }
  }
}

void platform_fall_south_tick(platform_t *p, size_t t) {
  for (size_t i = 1, r = p->n_rows - 1 - i; i < p->n_rows - t; ++i, --r) {
    for (size_t c = 0; c < p->n_columns; ++c) {
      if (p->map[r][c] == 'O' && p->map[r + 1][c] == '.') {
        p->map[r][c] = '.';
        p->map[r + 1][c] = 'O';
      }
    }
  }
}

void platform_fall_west_tick(platform_t *p, size_t t) {
  for (size_t c = 1; c < p->n_columns - t; ++c) {
    for (size_t r = 0; r < p->n_rows; ++r) {
      if (p->map[r][c] == 'O' && p->map[r][c - 1] == '.') {
        p->map[r][c] = '.';
        p->map[r][c - 1] = 'O';
      }
    }
  }
}

size_t platform_calc_load(platform_t *p) {
  size_t load = 0;

  for (size_t r = 0; r < p->n_rows; ++r) {
    for (size_t c = 0; c < p->n_columns; ++c) {
      if (p->map[r][c] == 'O')
        load += p->n_rows - r;
    }
  }

  return load;
}

void platform_fall_cycle(platform_t *p) {
  for (size_t t = 0; t < p->n_rows; ++t) {
    platform_fall_north_tick(p, t);
  }
  for (size_t t = 0; t < p->n_rows; ++t) {
    platform_fall_west_tick(p, t);
  }
  for (size_t t = 0; t < p->n_rows; ++t) {
    platform_fall_south_tick(p, t);
  }
  for (size_t t = 0; t < p->n_rows; ++t) {
    platform_fall_east_tick(p, t);
  }
}

void part1() {
  FILE *f = fopen("input", "r");

  char buffer[256];
  platform_t platform = {.n_rows = 0, .n_columns = 0};
  while (fgets(buffer, 256, f)) {
    strcpy(platform.map[platform.n_rows], buffer);
    ++platform.n_rows;
  }
  platform.n_columns = strlen(platform.map[0]);

  for (size_t t = 0; t < platform.n_rows; ++t) {
    platform_fall_north_tick(&platform, t);
  }

  size_t part1_load = platform_calc_load(&platform);
  printf("part1 load=%ld\n", part1_load);
  assert(part1_load == 109638);
}

#define PART2_N_CYCLES 1000000000
void part2() {
  FILE *f = fopen("input", "r");

  char buffer[256];
  platform_t platform = {.n_rows = 0, .n_columns = 0};
  while (fgets(buffer, 256, f)) {
    strcpy(platform.map[platform.n_rows], buffer);
    ++platform.n_rows;
  }
  platform.n_columns = strlen(platform.map[0]);

  size_t mem[1024][128][2];
  size_t n_mem = 0;
  size_t part2_load = 0;
  for (size_t n = 0; n < PART2_N_CYCLES; ++n) {
    platform_fall_cycle(&platform);

    size_t bits[128][2];
    memset(bits, 0, sizeof(size_t) * 128 * 2);
    for (size_t r = 0; r < platform.n_rows; ++r) {
      size_t p1 = 0;
      size_t p2 = 0;
      size_t i = 0;
      for (; i < platform.n_rows && i < 64; ++i) {
        if (platform.map[r][i] == 'O') {
          p1 |= 1 << i;
        }
      }
      for (size_t j = 0; i < platform.n_rows && i < 128; ++i, ++j) {
        if (platform.map[r][i] == 'O') {
          p2 |= 1 << j;
        }
      }
      bits[r][0] = p1;
      bits[r][1] = p2;
    }

    for (size_t m = 0; m < n_mem; ++m) {
      char found = 0;
      for (size_t r = 0; r < platform.n_rows; ++r) {
        if (mem[m][r][0] != bits[r][0] || mem[m][r][1] != bits[r][1]) {
          found = 0;
          break;
        }
        found = 1;
      }

      if (found) {
        size_t cycle_len = n_mem - m;
        printf("cycle start at %lu, cycle len is %lu\n", m, cycle_len);

        for (n += 1; n + cycle_len < PART2_N_CYCLES; n += cycle_len) {
        }
        for (; n < PART2_N_CYCLES; ++n) {
            platform_fall_cycle(&platform);
        }
        goto cycle_found;
      }
    }

    for (size_t n = 0; n < platform.n_rows; ++n) {
      mem[n_mem][n][0] = bits[n][0];
      mem[n_mem][n][1] = bits[n][1];
    }
    n_mem++;
  }

cycle_found:

  part2_load = platform_calc_load(&platform);
  printf("part2 load=%ld\n", part2_load);
  assert(part2_load == 102657);
}

int main() {
  part1();
  part2();
  return 1;
}
