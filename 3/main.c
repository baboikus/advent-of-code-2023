#include <assert.h>
#include <stdio.h>

#define IS_SYMBOL(c)                                                           \
  (c != '.' && c != '\n' && c != '\0' && (c < '0' || c > '9'))
#define IS_NUMBER(c) (c >= '0' && c <= '9')

int find_parts_sum(char *top_line, char *inner_line, char *bottom_line) {
  assert(inner_line != 0);

  int acc = 0;
  int sum = 0;
  int is_part_number = 0;
  int is_number_completed = 0;
  for (size_t i = 0; inner_line[i]; ++i) {
    char c = inner_line[i];

    if (IS_NUMBER(c))
      acc = acc * 10 + (c - '0');
    if (acc > 0 && !IS_NUMBER(inner_line[i + 1]))
      is_number_completed = 1;

    if (!is_part_number && acc > 0) {
      if (top_line) {
        if ((i > 0 && IS_SYMBOL(top_line[i - 1])) || IS_SYMBOL(top_line[i]) ||
            IS_SYMBOL(top_line[i + 1]))
          is_part_number = 1;
      }
      if (bottom_line) {
        if ((i > 0 && IS_SYMBOL(bottom_line[i - 1])) ||
            IS_SYMBOL(bottom_line[i]) || IS_SYMBOL(bottom_line[i + 1]))
          is_part_number = 1;
      }
      if ((i > 0 && IS_SYMBOL(inner_line[i - 1])) ||
          IS_SYMBOL(inner_line[i + 1]))
        is_part_number = 1;
    }

    if (is_number_completed) {
      if (is_part_number) {
        sum += acc;
        is_part_number = 0;
      }
      acc = 0;
      is_number_completed = 0;
    }
  }

  return sum;
}

int find_number(char *line, size_t from, size_t *left_idx, size_t *right_idx) {
  while (from > 0 && IS_NUMBER(line[from]))
    from--;
  if (!IS_NUMBER(line[from]))
    from++;
  *left_idx = from;
  int acc = 0;
  while (IS_NUMBER(line[from])) {
    acc = acc * 10 + line[from] - '0';
    from++;
  }
  *right_idx = from - 1;

  printf("found number = %d\n", acc);

  return acc;
}

int find_gears_ratio_sum(char *top_line, char *inner_line, char *bottom_line) {
  int sum = 0;
  int acc = 1;
  int acc_counter = 0;
  size_t left_idx = 0, right_idx = 0;
  for (size_t i = 0; inner_line[i]; ++i) {
    if (inner_line[i] == '*') {
      acc = 1;
      acc_counter = 0;
      if (top_line) {
        if (i > 0 && IS_NUMBER(top_line[i - 1])) {
          acc *= find_number(top_line, i - 1, &left_idx, &right_idx);
          acc_counter++;
          if (right_idx < i && IS_NUMBER(top_line[i + 1])) {
            acc *= find_number(top_line, i + 1, &left_idx, &right_idx);
            acc_counter++;
          }
        } else if (IS_NUMBER(top_line[i])) {
          acc *= find_number(top_line, i, &left_idx, &right_idx);
          acc_counter++;
        } else if (IS_NUMBER(top_line[i + 1])) {
          acc *= find_number(top_line, i + 1, &left_idx, &right_idx);
          acc_counter++;
        }
      }
      if (bottom_line) {
        if (i > 0 && IS_NUMBER(bottom_line[i - 1])) {
          acc *= find_number(bottom_line, i - 1, &left_idx, &right_idx);
          acc_counter++;
          if (right_idx < i && IS_NUMBER(bottom_line[i + 1])) {
            acc *= find_number(bottom_line, i + 1, &left_idx, &right_idx);
            acc_counter++;
          }
        } else if (IS_NUMBER(bottom_line[i])) {
          acc *= find_number(bottom_line, i, &left_idx, &right_idx);
          acc_counter++;
        } else if (IS_NUMBER(bottom_line[i + 1])) {
          acc *= find_number(bottom_line, i + 1, &left_idx, &right_idx);
          acc_counter++;
        }
      }
      if (i > 0 && IS_NUMBER(inner_line[i - 1])) {
        acc *= find_number(inner_line, i - 1, &left_idx, &right_idx);
        acc_counter++;
      }

      if (IS_NUMBER(inner_line[i + 1])) {
        acc *= find_number(inner_line, i + 1, &left_idx, &right_idx);
        acc_counter++;
      }
      if (acc_counter == 2) {
        sum += acc;
      }
    }
  }

  return sum;
}

void part1() {
  FILE *fptr;
  fptr = fopen("input", "r");

  char buffer[3][256];

  fgets(buffer[0], 256, fptr);

  char *top_line = 0;
  char *inner_line = buffer[0];
  char *bottom_line = 0;

  int sum = 0;
  size_t buffer_idx = 1;
  while (fgets(buffer[buffer_idx], 256, fptr)) {
    bottom_line = buffer[buffer_idx];

    sum += find_parts_sum(top_line, inner_line, bottom_line);

    top_line = inner_line;
    inner_line = bottom_line;
    buffer_idx = (buffer_idx + 1) % 3;
  }

  sum += find_parts_sum(top_line, inner_line, 0);

  printf("sum=%d\n", sum);
  assert(sum == 553825);
}

void part2() {
  FILE *fptr;
  fptr = fopen("input", "r");

  char buffer[3][256];

  fgets(buffer[0], 256, fptr);

  char *top_line = 0;
  char *inner_line = buffer[0];
  char *bottom_line = 0;

  int sum = 0;
  size_t buffer_idx = 1;
  while (fgets(buffer[buffer_idx], 256, fptr)) {
    bottom_line = buffer[buffer_idx];

    sum += find_gears_ratio_sum(top_line, inner_line, bottom_line);

    top_line = inner_line;
    inner_line = bottom_line;
    buffer_idx = (buffer_idx + 1) % 3;
  }
  sum += find_gears_ratio_sum(top_line, inner_line, 0);

  printf("sum=%d\n", sum);
  assert(sum == 93994191);
}

int main() {
  part1();
  part2();

  return 0;
}
