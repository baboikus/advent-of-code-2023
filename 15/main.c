#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Lens {
  char label[8];
  size_t focal_len;
  size_t hash;
} lens_t;

typedef struct Box {
  lens_t *lenses[64];
  size_t n_lenses;
} box_t;

size_t str_hash(char *str) {
  size_t acc = 0;
  while (*str) {
    if (*str == ',' || *str == '\n') {
      return acc;
    } else {
      acc = (*str + acc) * 17 % 256;
    }
    ++str;
  }
  return acc;
}

char *lens_read(char *line, lens_t *lens, char *command) {
  lens->focal_len = 0;
  char *label = lens->label;
  while (*line != '=' && *line != '-') {
    *label++ = *line++;
  }
  *label = 0;

  if (*line == '=') {
    *command = '+';
    ++line;
    lens->focal_len = strtol(line, &line, 10);
  } else if (*line == '-') {
    *command = '-';
    ++line;
  } else {
    printf("%s", line);
    assert(0);
  }

  assert(*line == ',' || *line == '\n');

  lens->hash = str_hash(lens->label);

  return line;
}

void box_add_lens(box_t *box, lens_t *lens) {
  for (size_t i = 0; i < box->n_lenses; ++i) {
    if (box->lenses[i] && strcmp(lens->label, box->lenses[i]->label) == 0) {
      box->lenses[i] = lens;
      return;
    }
  }
  box->lenses[box->n_lenses++] = lens;
  assert(box->n_lenses < 64);
}

void box_remove_lens(box_t *box, lens_t *lens) {
  for (size_t i = 0; i < box->n_lenses; ++i) {
    if (box->lenses[i] && strcmp(lens->label, box->lenses[i]->label) == 0) {
      box->lenses[i] = 0;
    }
  }
}

size_t box_focusing_power(box_t *box, size_t box_idx) {
  size_t acc = 0;
  for (size_t i = 0, lens_idx = 0; i < box->n_lenses; ++i) {
    if (box->lenses[i]) {
      acc += (box_idx + 1) * (lens_idx + 1) * (box->lenses[i]->focal_len);
      ++lens_idx;
    }
  }
  return acc;
}

void part1() {
  FILE *f = fopen("input", "r");
  char buffer[32 * 1024];
  char *line = fgets(buffer, 32 * 1024, f);
  size_t part1_sum = 0;
  size_t acc = 0;
  while (*line && *line != '\n') {
    if (*line == ',') {
      part1_sum += acc;
      acc = 0;
    } else {
      acc = (*line + acc) * 17 % 256;
    }
    ++line;
  }
  part1_sum += acc;

  printf("part1 sum=%lu\n", part1_sum);
}

void part2() {
  FILE *f = fopen("input", "r");
  char buffer[32 * 1024];
  char *line = fgets(buffer, 32 * 1024, f);

  box_t boxes[256];
  memset(boxes, 0, sizeof(box_t) * 256);

  lens_t lenses[4 * 1024];
  memset(lenses, 0, sizeof(lens_t) * 4 * 1024);
  size_t n_lenses = 0;

  while (*line != '\n') {
    char command;
    lens_t *lens = &lenses[n_lenses++];
    assert(n_lenses < 4 * 1024);

    line = lens_read(line, lens, &command);
    if(*line == ',') ++line;
    if (command == '+') {
      box_add_lens(&boxes[lens->hash], lens);
    } else if (command == '-') {
      box_remove_lens(&boxes[lens->hash], lens);
    } else {
      assert(0);
    }
  }

  size_t part2_sum = 0;
  for (size_t i = 0; i < 256; ++i) {
    part2_sum += box_focusing_power(&boxes[i], i);
  }

  printf("part2 sum=%lu\n", part2_sum);
  assert(part2_sum == 279470);
}

int main() {
  part1();
  part2();
  return 0;
}
