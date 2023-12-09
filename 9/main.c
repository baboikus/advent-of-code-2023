#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Prediction {
  int prev;
  int next;
} prediction_t;

prediction_t predict(int *values, size_t n_values) {
  assert(n_values < 128);

  int last_value_per_level[128];
  int first_value_per_level[128];
  size_t n_levels = 0;

  int additional_values[128];
  int *next_values = additional_values;
  while (1) {
    char has_only_zeros = 1;
    for (size_t i = 1; i < n_values; ++i) {
      next_values[i - 1] = values[i] - values[i - 1];

      if (next_values[i - 1] != 0)
        has_only_zeros = 0;

      if (i == 1)
        first_value_per_level[n_levels] = values[i-1];

      if (i + 1 == n_values)
        last_value_per_level[n_levels++] = values[i];
    }

    if (has_only_zeros)
      break;

    int *t = values;
    values = next_values;
    next_values = t;
    n_values--;
  }

  prediction_t prediction = {.prev = 0, .next = 0};
  do {
    n_levels--;

    int last_value = last_value_per_level[n_levels];
    prediction.next += last_value;

    int first_value = first_value_per_level[n_levels];
    prediction.prev = first_value - prediction.prev;

  } while (n_levels != 0);

  return prediction;
}

void part1_part2() {
  FILE *fptr;
  fptr = fopen("input", "r");

  char buffer[512];

  int part1_sum = 0;
  int part2_sum = 0;
  int values[128];
  while (fgets(buffer, 512, fptr)) {
    char *line = buffer;
    size_t n_values = 0;
    while (*line && *line != '\n') {
      values[n_values++] = strtol(line, &line, 10);
    }
    prediction_t p = predict(values, n_values);
    part1_sum += p.next;
    part2_sum += p.prev;
  }

  printf("part1 sum = %d\n", part1_sum);
  assert(part1_sum == 1901217887);

  printf("part2 sum = %d\n", part2_sum);
  assert(part2_sum == 905);
}

int main() {
  part1_part2();
  return 0;
}
