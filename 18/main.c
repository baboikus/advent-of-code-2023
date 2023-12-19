#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N_SIDE 1024

void part1() {
  FILE *f = fopen("input", "r");

  char map[N_SIDE][N_SIDE];
  memset(map, '.', N_SIDE * N_SIDE);

  size_t row = N_SIDE / 2;
  size_t column = N_SIDE / 2;

  char buffer[64];
  while (fgets(buffer, 64, f)) {
    char *line = buffer;
    size_t meters = strtol(line + 1, &line, 10);

    while (meters-- > 0) {
      switch (buffer[0]) {
      case 'U':
        --row;
        break;
      case 'D':
        ++row;
        break;
      case 'L':
        --column;
        break;
      case 'R':
        ++column;
        break;
      default:
        assert(0);
      }
      map[row][column] = '#';
    }
  }

  for (size_t r = 0; r < N_SIDE; ++r) {
    char is_inside = 0;
    char horizontal = 0;
    for (size_t c = 0; c < N_SIDE; ++c) {
      if (map[r][c] == '#') {
        if (horizontal == 0) {
          if (map[r][c + 1] == '#') {
            if (map[r - 1][c] == '#')
              horizontal = 'U';
            if (map[r + 1][c] == '#')
              horizontal = 'D';
          } else {
            is_inside = !is_inside;
          }
        } else {
          if (map[r][c + 1] == '.') {
            if (map[r - 1][c] == '#' && horizontal == 'D')
              is_inside = !is_inside;
            if (map[r + 1][c] == '#' && horizontal == 'U')
              is_inside = !is_inside;
            horizontal = 0;
          }
        }
      }
      if (is_inside && map[r][c] == '.')
        map[r][c] = 'X';
    }
  }

  size_t part1_sum = 0;
  for (size_t r = 0; r < N_SIDE; ++r) {
    for (size_t c = 0; c < N_SIDE; ++c) {
      if (map[r][c] != '.')
        ++part1_sum;
    }
  }
  printf("part1 sum=%lu\n", part1_sum);
  assert(part1_sum == 40761);
}

typedef struct Edge {
  long int from_x;
  long int from_y;
  long int to_x;
  long int to_y;
} edge_t;

typedef struct Rectangle {
  long int x1;
  long int y1;
  long int x2;
  long int y2;
  char type;
} rect_t;

void rect_intersect(rect_t *r1, rect_t *r2, long int *x, long int *y) {
  if (r1->type == 'R' && r2->type == 'D') {
    *x = r1->x2;
    *y = r2->y1;
  }
  if (r1->type == 'R' && r2->type == 'U') {
    *x = r1->x2 - 1;
    *y = r1->y2 + 1;
  }

  if (r1->type == 'D' && r2->type == 'R') {
    *x = r1->x1 + 1;
    *y = r1->y2 + 1;
  }
  if (r1->type == 'D' && r2->type == 'L') {
    *x = r1->x2;
    *y = r1->y2;
  }

  if (r1->type == 'L' && r2->type == 'D') {
    *x = r1->x2 + 1;
    *y = r1->y2 - 1;
  }
  if (r1->type == 'L' && r2->type == 'U') {
    *x = r1->x2;
    *y = r1->y1;
  }

  if (r1->type == 'U' && r2->type == 'L') {
    *x = r1->x1 - 1;
    *y = r1->y2 - 1;
  }
  if (r1->type == 'U' && r2->type == 'R') {
    *x = r1->x2;
    *y = r1->y2;
  }
}

void part2() {
  FILE *f = fopen("input", "r");
  char buffer[64];

  long int x = 0;
  long int y = 0;

  edge_t edges[1024];
  size_t n_edges = 0;
  edges[0] = (edge_t){.from_x = 0, .from_y = 0};

  rect_t rects[1024];
  size_t n_rects = 0;
  while (fgets(buffer, 64, f)) {
    char *line = buffer;
    line += 1;
    strtol(line, &line, 10);
    line += 3;
    char dir = line[5];
    line[5] = 0;
    long int meters = strtol(line, &line, 16);

    edge_t *edge = &edges[n_edges];

    edge->to_x = edge->from_x;
    edge->to_y = edge->from_y;

    switch (dir) {
    case '0':
      edge->to_x += meters;
      break;
    case '1':
      edge->to_y -= meters;
      break;
    case '2':
      edge->to_x -= meters;
      break;
    case '3':
      edge->to_y += meters;
      break;
    default:
      assert(0);
    }

    assert(n_edges < 1023);
    edges[n_edges + 1] = (edge_t){.from_x = edge->to_x, .from_y = edge->to_y};
    ++n_edges;

    rect_t rect = {.x1 = edge->from_x,
                   .y1 = edge->from_y,
                   .x2 = edge->to_x,
                   .y2 = edge->to_y};
    switch (dir) {
    case '0':
      rect.x2 += 1;
      rect.y2 -= 1;
      rect.type = 'R';
      break;
    case '1':
      rect.x2 += 1;
      rect.y2 -= 1;
      rect.type = 'D';
      break;
    case '2':
      rect.x1 += 1;
      rect.y1 -= 1;
      rect.type = 'L';
      break;
    case '3':
      rect.x1 += 1;
      rect.y1 -= 1;
      rect.type = 'U';
      break;
    default:
      assert(0);
    }
    rects[n_rects++] = rect;
  }

  assert(n_rects == n_edges);
  for (size_t i = 0; i < n_rects; ++i) {
    long int x = 0;
    long int y = 0;
    rect_intersect(&rects[i], &rects[(i + 1) % n_rects], &x, &y);
    edges[i].to_x = edges[(i + 1) % n_rects].from_x = x;
    edges[i].to_y = edges[(i + 1) % n_rects].from_y = y;
  }

  long int area = 0;
  for (size_t i = 0; i < n_edges; ++i) {
    area += edges[i].from_x * edges[i].to_y - edges[i].from_y * edges[i].to_x;
  }
  area = labs(area) / 2;

  printf("part2 area=%ld\n", area);
  assert(area == 106920098354636);
}

int main() {
  part1();
  part2();
  return 0;
}
