#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Map {
  char layout[256][256];
  size_t n_rows;
  size_t n_columns;
} map_t;

typedef struct MapPoint {
  size_t row;
  size_t column;
  char dir;
} map_point_t;

size_t map_move(map_t *map, map_point_t *from, map_point_t *to, char dir) {
  size_t n_to = 0;
  switch (dir) {
  case '>':
    if (from->column < map->n_columns - 1)
      to[n_to++] = (map_point_t){
          .row = from->row, .column = from->column + 1, .dir = '>'};
    break;
  case '<':
    if (from->column > 0)
      to[n_to++] = (map_point_t){
          .row = from->row, .column = from->column - 1, .dir = '<'};
    break;
  case '^':
    if (from->row > 0)
      to[n_to++] = (map_point_t){
          .row = from->row - 1, .column = from->column, .dir = '^'};
    break;
  case 'v':
    if (from->row < map->n_rows - 1)
      to[n_to++] = (map_point_t){
          .row = from->row + 1, .column = from->column, .dir = 'v'};
    break;
  default:
    assert(0);
  }
  return n_to;
}

size_t calc_energized_tiles(map_t *map, map_point_t start) {
  char visited[256][256];
  memset(visited, 0, 256 * 256);

  map_point_t buffers[2][8 * 1024];
  map_point_t *current_front = buffers[0];
  map_point_t *next_front = buffers[1];
  size_t n_current_front = 0;
  current_front[n_current_front++] = start;
  while (n_current_front > 0) {
    char has_new_visited_tiles = 0;
    assert(n_current_front < 8 * 1024);
    size_t n_next_front = 0;
    for (size_t i = 0; i < n_current_front; ++i) {
      map_point_t *p = &current_front[i];
      assert(p->row < map->n_rows);
      assert(p->column < map->n_columns);

      char old_v = visited[p->row][p->column];
      char new_v = visited[p->row][p->column];
      switch (p->dir) {
      case '>':
        new_v |= 1 << 0;
        break;
      case '<':
        new_v |= 1 << 1;
        break;
      case '^':
        new_v |= 1 << 2;
        break;
      case 'v':
        new_v |= 1 << 3;
        break;
      default:
        assert(0);
      }

      if (old_v != new_v) {
        visited[p->row][p->column] = new_v;
        has_new_visited_tiles = 1;
      } else {
        continue;
      }

      char current_symbol = map->layout[p->row][p->column];
      switch (current_symbol) {
      case '.':
        n_next_front += map_move(map, p, next_front + n_next_front, p->dir);
        break;
      case '|':
        switch (p->dir) {
        case '>':
        case '<':
          n_next_front += map_move(map, p, next_front + n_next_front, '^');
          n_next_front += map_move(map, p, next_front + n_next_front, 'v');
          break;
        case '^':
        case 'v':
          n_next_front += map_move(map, p, next_front + n_next_front, p->dir);
          break;
        default:
          assert(0);
        }
        break;
      case '-':
        switch (p->dir) {
        case '>':
        case '<':
          n_next_front += map_move(map, p, next_front + n_next_front, p->dir);
          break;
        case '^':
        case 'v':
          n_next_front += map_move(map, p, next_front + n_next_front, '<');
          n_next_front += map_move(map, p, next_front + n_next_front, '>');
          break;
        default:
          assert(0);
        }
        break;
      case '\\':
        switch (p->dir) {
        case '>':
          n_next_front += map_move(map, p, next_front + n_next_front, 'v');
          break;
        case '<':
          n_next_front += map_move(map, p, next_front + n_next_front, '^');
          break;
        case '^':
          n_next_front += map_move(map, p, next_front + n_next_front, '<');
          break;
        case 'v':
          n_next_front += map_move(map, p, next_front + n_next_front, '>');
          break;
        default:
          assert(0);
        }
        break;
      case '/':
        switch (p->dir) {
        case '>':
          n_next_front += map_move(map, p, next_front + n_next_front, '^');
          break;
        case '<':
          n_next_front += map_move(map, p, next_front + n_next_front, 'v');
          break;
        case '^':
          n_next_front += map_move(map, p, next_front + n_next_front, '>');
          break;
        case 'v':
          n_next_front += map_move(map, p, next_front + n_next_front, '<');
          break;
        default:
          assert(0);
        }
        break;
      default:
        printf("unknown symbol '%c' code=%u at r=%lu c=%lu\n", current_symbol,
               current_symbol, p->row, p->column);
        assert(0);
      }
    }

    map_point_t *tmp = current_front;
    current_front = next_front;
    next_front = tmp;
    n_current_front = n_next_front;

    if (!has_new_visited_tiles)
      break;
  }

  size_t n_energized_tiles = 0;
  for (size_t r = 0; r < map->n_rows; ++r) {
    for (size_t c = 0; c < map->n_columns; ++c) {
      if (visited[r][c] > 0)
        ++n_energized_tiles;
    }
  }

  return n_energized_tiles;
}

size_t MAX(size_t n1, size_t n2) { return n1 > n2 ? n1 : n2; }

void part1_part2() {
  FILE *f = fopen("input", "r");
  map_t map = {.n_rows = 0, .n_columns = 0};
  while (fgets(map.layout[map.n_rows], 256, f)) {
    ++map.n_rows;
  }
  map.n_columns = strlen(map.layout[0]) - 1;

  size_t part1_sum = calc_energized_tiles(
      &map, (map_point_t){.row = 0, .column = 0, .dir = '>'});
  printf("part1 sum=%lu\n", part1_sum);
  assert(part1_sum == 7046);

  size_t part2_sum = 0;
  for (size_t r = 0; r < map.n_rows; ++r) {
    part2_sum = MAX(
        part2_sum, calc_energized_tiles(
                       &map, (map_point_t){.row = r, .column = 0, .dir = '>'}));
    part2_sum = MAX(
        part2_sum,
        calc_energized_tiles(
            &map,
            (map_point_t){.row = r, .column = map.n_columns - 1, .dir = '<'}));
  }
  for (size_t c = 0; c < map.n_columns; ++c) {
    part2_sum = MAX(
        part2_sum, calc_energized_tiles(
                       &map, (map_point_t){.row = 0, .column = c, .dir = 'v'}));
    part2_sum =
        MAX(part2_sum,
            calc_energized_tiles(
                &map,
                (map_point_t){.row = map.n_rows - 1, .column = c, .dir = '^'}));
  }

  printf("part2 sum=%lu\n", part2_sum);
  assert(part2_sum == 7313);
}

int main() {
  part1_part2();
  return 1;
}
