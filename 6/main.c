#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int count_win_ways(double time, double distance) {
  double min_speed = ((double)time - sqrt(time * time - 4 * distance)) / 2.0;
  double max_speed = ((double)time + sqrt(time * time - 4 * distance)) / 2.0;

  if (ceil(min_speed) - min_speed < 0.00001)
    min_speed += 0.5;
  if (max_speed - floor(max_speed) < 0.00001)
    max_speed -= 0.5;

  return floor(max_speed) - ceil(min_speed) + 1;
}

void part1_part2() {
  FILE *fptr;
  fptr = fopen("input", "r");

  char time_buffer[128];
  char distance_buffer[128];
  fgets(time_buffer, 128, fptr);
  fgets(distance_buffer, 128, fptr);

  char *time_line = time_buffer;
  char *distance_line = distance_buffer;
  time_line += 5;
  distance_line += 9;
  int product1 = 1;
  for (;;) {
    int time = strtol(time_line, &time_line, 10);
    int distance = strtol(distance_line, &distance_line, 10);

    if (time == 0)
      break;

    product1 *= count_win_ways(time, distance);
  }

  printf("part1 product = %d\n", product1);
  assert(product1 == 741000);

  time_line = time_buffer;
  distance_line = distance_buffer;
  time_line += 9;
  distance_line += 9;

  double time_acc = 0;
  double distance_acc = 0;
  for (; *time_line != '\n'; ++time_line, ++distance_line) {
    if (isdigit(*time_line))
      time_acc = time_acc * 10 + *time_line - '0';
    if (isdigit(*distance_line))
      distance_acc = distance_acc * 10 + *distance_line - '0';
  }

  int win_ways = count_win_ways(time_acc, distance_acc);
  printf("part2 win ways = %d\n", win_ways);
  assert(win_ways == 38220708);
}

int main() {
  part1_part2();
  return 0;
}
