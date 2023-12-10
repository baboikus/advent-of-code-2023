#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Connection {
  size_t column_idx;
  size_t row_idx;
} connection_t;

typedef struct Pipe {
  connection_t conn1;
  connection_t conn2;
  char type;
  char is_empty;
  size_t row_idx;
  size_t column_idx;
} pipe_t;

#define N_MAP_SIZE 256
typedef struct Map {
  pipe_t pipes[N_MAP_SIZE][N_MAP_SIZE];
  char bits[N_MAP_SIZE * 3][N_MAP_SIZE * 3];
} map_t;

pipe_t *map_add_pipe(map_t *map, size_t pipe_column_idx, size_t pipe_row_idx,
                     char pipe_type) {
  pipe_t pipe;
  pipe.column_idx = pipe_column_idx;
  pipe.row_idx = pipe_row_idx;
  pipe.is_empty = 0;
  pipe.type = pipe_type;
  switch (pipe_type) {
  case '|':
    pipe.conn1.column_idx = pipe_column_idx;
    pipe.conn1.row_idx = pipe_row_idx - 1;
    pipe.conn2.column_idx = pipe_column_idx;
    pipe.conn2.row_idx = pipe_row_idx + 1;
    break;
  case '-':
    pipe.conn1.column_idx = pipe_column_idx + 1;
    pipe.conn1.row_idx = pipe_row_idx;
    pipe.conn2.column_idx = pipe_column_idx - 1;
    pipe.conn2.row_idx = pipe_row_idx;
    break;
  case 'L':
    pipe.conn1.column_idx = pipe_column_idx;
    pipe.conn1.row_idx = pipe_row_idx - 1;
    pipe.conn2.column_idx = pipe_column_idx + 1;
    pipe.conn2.row_idx = pipe_row_idx;
    break;
  case 'J':
    pipe.conn1.column_idx = pipe_column_idx;
    pipe.conn1.row_idx = pipe_row_idx - 1;
    pipe.conn2.column_idx = pipe_column_idx - 1;
    pipe.conn2.row_idx = pipe_row_idx;
    break;
  case '7':
    pipe.conn1.column_idx = pipe_column_idx - 1;
    pipe.conn1.row_idx = pipe_row_idx;
    pipe.conn2.column_idx = pipe_column_idx;
    pipe.conn2.row_idx = pipe_row_idx + 1;
    break;
  case 'F':
    pipe.conn1.column_idx = pipe_column_idx + 1;
    pipe.conn1.row_idx = pipe_row_idx;
    pipe.conn2.column_idx = pipe_column_idx;
    pipe.conn2.row_idx = pipe_row_idx + 1;
    break;
  case '.':
    pipe.is_empty = 1;
    break;
  default:
    assert(0);
  }

  map->pipes[pipe_column_idx][pipe_row_idx] = pipe;
  return &(map->pipes[pipe_column_idx][pipe_row_idx]);
}

pipe_t *map_step(map_t *map, pipe_t *current_pipe, pipe_t *prev_pipe) {
  assert(current_pipe);
  assert(prev_pipe);

  if (&(map->pipes[current_pipe->conn1.column_idx]
                  [current_pipe->conn1.row_idx]) == prev_pipe) {
    return &(map->pipes[current_pipe->conn2.column_idx]
                       [current_pipe->conn2.row_idx]);
  }

  if (&(map->pipes[current_pipe->conn2.column_idx]
                  [current_pipe->conn2.row_idx]) == prev_pipe) {
    return &(map->pipes[current_pipe->conn1.column_idx]
                       [current_pipe->conn1.row_idx]);
  }

  return 0;
}

void map_add_bits(map_t *map, size_t column_idx, size_t row_idx) {
  size_t bit_column_idx = column_idx * 3;
  size_t bit_row_idx = row_idx * 3;

  char wall = '#';

  switch (map->pipes[column_idx][row_idx].type) {
  case '|':
    map->bits[bit_column_idx + 1][bit_row_idx] = wall;
    map->bits[bit_column_idx + 1][bit_row_idx + 1] = wall;
    map->bits[bit_column_idx + 1][bit_row_idx + 2] = wall;
    break;
  case '-':
    map->bits[bit_column_idx][bit_row_idx + 1] = wall;
    map->bits[bit_column_idx + 1][bit_row_idx + 1] = wall;
    map->bits[bit_column_idx + 2][bit_row_idx + 1] = wall;
    break;
  case 'L':
    map->bits[bit_column_idx + 1][bit_row_idx + 1] = wall;
    map->bits[bit_column_idx + 1][bit_row_idx] = wall;
    map->bits[bit_column_idx + 2][bit_row_idx + 1] = wall;
    break;
  case 'J':
    map->bits[bit_column_idx + 1][bit_row_idx + 1] = wall;
    map->bits[bit_column_idx + 1][bit_row_idx] = wall;
    map->bits[bit_column_idx][bit_row_idx + 1] = wall;
    break;
  case '7':
    map->bits[bit_column_idx + 1][bit_row_idx + 1] = wall;
    map->bits[bit_column_idx][bit_row_idx + 1] = wall;
    map->bits[bit_column_idx + 1][bit_row_idx + 2] = wall;
    break;
  case 'F':
    map->bits[bit_column_idx + 1][bit_row_idx + 1] = wall;
    map->bits[bit_column_idx + 2][bit_row_idx + 1] = wall;
    map->bits[bit_column_idx + 1][bit_row_idx + 2] = wall;
    break;
  }
}

pipe_t *map_read(map_t *map, FILE *f) {
  char buffer[256];

  memset(map->pipes, 0, N_MAP_SIZE * N_MAP_SIZE * sizeof(pipe_t));
  memset(map->bits, '.', N_MAP_SIZE * N_MAP_SIZE * 3 * 3);
  for (size_t r = 0; r < N_MAP_SIZE; ++r) {
    for (size_t c = 0; c < N_MAP_SIZE; ++c) {
      map->bits[c * 3 + 1][r * 3 + 1] = 'X';
    }
  }

  pipe_t *starting_pipe = 0;
  size_t row_idx = 1;
  while (fgets(buffer, 256, f)) {
    char *line = buffer;
    for (size_t column_idx = 1; *line && *line != '\n'; ++column_idx, ++line) {
      if (*line == 'S') {
        starting_pipe = map_add_pipe(map, column_idx, row_idx, 'J');
      } else {
        map_add_pipe(map, column_idx, row_idx, *line);
      }
    }
    ++row_idx;
  }

  return starting_pipe;
}

typedef struct MapBitsPoint {
  size_t column_idx;
  size_t row_idx;
} bits_point_t;

size_t map_bits_step(map_t *map, bits_point_t *old_front,
                     bits_point_t *new_front) {
  size_t tiles_counter = 0;

  for (; old_front->column_idx > 0; ++old_front) {
    size_t left_column_idx = old_front->column_idx - 1;
    size_t right_column_idx = old_front->column_idx + 1;
    size_t top_row_idx = old_front->row_idx - 1;
    size_t bottom_row_idx = old_front->row_idx + 1;

    char *top_bit = &(map->bits[old_front->column_idx][top_row_idx]);
    char *bottom_bit = &(map->bits[old_front->column_idx][bottom_row_idx]);
    char *left_bit = &(map->bits[left_column_idx][old_front->row_idx]);
    char *right_bit = &(map->bits[right_column_idx][old_front->row_idx]);

    if (*top_bit != '#' && *top_bit != 'V') {
      if (*top_bit == 'X')
        tiles_counter++;

      *top_bit = 'V';
      *new_front++ = (bits_point_t){.column_idx = old_front->column_idx,
                                    .row_idx = top_row_idx};
    }

    if (*bottom_bit != '#' && *bottom_bit != 'V') {
      if (*bottom_bit == 'X')
        tiles_counter++;

      *bottom_bit = 'V';
      *new_front++ = (bits_point_t){.column_idx = old_front->column_idx,
                                    .row_idx = bottom_row_idx};
    }

    if (*left_bit != '#' && *left_bit != 'V') {
      if (*left_bit == 'X')
        tiles_counter++;

      *left_bit = 'V';
      *new_front++ = (bits_point_t){.column_idx = left_column_idx,
                                    .row_idx = old_front->row_idx};
    }

    if (*right_bit != '#' && *right_bit != 'V') {
      if (*right_bit == 'X')
        tiles_counter++;

      *right_bit = 'V';
      *new_front++ = (bits_point_t){.column_idx = right_column_idx,
                                    .row_idx = old_front->row_idx};
    }
  }
  new_front->column_idx = 0;

  return tiles_counter;
}

void part1_part2() {
  FILE *fptr;
  fptr = fopen("input", "r");

  /*PART 1*/
  map_t map;
  pipe_t *starting_pipe = map_read(&map, fptr);
  assert(starting_pipe);
  pipe_t *current_pipe = map_step(&map, starting_pipe,
                                  &(map.pipes[starting_pipe->conn1.column_idx]
                                             [starting_pipe->conn1.row_idx]));
  pipe_t *prev_pipe = starting_pipe;
  assert(current_pipe);
  size_t steps_counter = 0;
  while (current_pipe != starting_pipe) {
    map_add_bits(&map, prev_pipe->column_idx, prev_pipe->row_idx);
    pipe_t *t = current_pipe;
    current_pipe = map_step(&map, current_pipe, prev_pipe);
    assert(current_pipe);
    prev_pipe = t;
    steps_counter++;
  }
  map_add_bits(&map, prev_pipe->column_idx, prev_pipe->row_idx);

  steps_counter = steps_counter / 2 + 1;

  printf("part1 steps %ld\n", steps_counter);
  assert(steps_counter == 6828);

  /*PART 2*/
  bits_point_t fronts[2][1024];
  bits_point_t *current_front = fronts[0];
  bits_point_t *next_front = fronts[1];

  current_front[0] = (bits_point_t){.column_idx = 276, .row_idx = 126};
  current_front[1].column_idx = 0;
  size_t tiles_counter = 0;
  while (1) {
    tiles_counter += map_bits_step(&map, current_front, next_front);
    if ((*next_front).column_idx == 0)
      break;

    bits_point_t *t = current_front;
    current_front = next_front;
    next_front = t;
  }

  printf("part2 tiles %ld\n", tiles_counter);
  assert(tiles_counter == 459);
}

int main() {
  part1_part2();
  return 0;
}
