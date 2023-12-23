#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_X 256
#define MAX_Y 256
#define N_MAX_BRICKS 2048

typedef struct TopCube {
  size_t height;
  size_t brick_id;
} top_cube_t;

typedef struct Brick {
  size_t x1, y1, z1, x2, y2, z2;
} brick_t;

typedef struct BrickNode {
  size_t depending_nodes[32];
  size_t n_depending_nodes;
  size_t under_nodes_counter;
} brick_node_t;

int brick_compare(const void *v1, const void *v2) {
  const brick_t *b1 = (const brick_t *)v1;
  const brick_t *b2 = (const brick_t *)v2;
  if (b1->z1 < b2->z1)
    return -1;
  if (b1->z1 > b2->z1)
    return 1;
  return 0;
}

void part1() {
  FILE *f = fopen("input", "r");
  assert(f);
  char buffer[64];
  top_cube_t ground[MAX_Y][MAX_X];
  memset(ground, 0, sizeof(top_cube_t) * MAX_X * MAX_Y);
  size_t max_brick_id = 0;
  size_t disintegrated_counter;
  char disintegrated_bricks_idx_set[2048];
  // char covered_bricks_idx_set[N_MAX_BRICKS];
  memset(disintegrated_bricks_idx_set, 1, N_MAX_BRICKS);
  // memset(covered_bricks_idx_set, 0, N_MAX_BRICKS);
  brick_t bricks[N_MAX_BRICKS];
  size_t n_bricks = 0;
  while (fgets(buffer, 64, f)) {
    assert(max_brick_id < N_MAX_BRICKS);
    brick_t *brick = &bricks[n_bricks++];
    char *line = buffer;
    brick->x1 = strtol(line, &line, 10);
    assert(*line == ',');
    ++line;
    brick->y1 = strtol(line, &line, 10);
    assert(*line == ',');
    ++line;
    brick->z1 = strtol(line, &line, 10);
    assert(*line == '~');
    ++line;
    brick->x2 = strtol(line, &line, 10);
    assert(*line == ',');
    ++line;
    brick->y2 = strtol(line, &line, 10);
    assert(*line == ',');
    ++line;
    brick->z2 = strtol(line, &line, 10);

    assert(brick->x1 <= brick->x2);
    assert(brick->y1 <= brick->y2);
    assert(brick->z1 <= brick->z2);
  }

  qsort(bricks, n_bricks, sizeof(brick_t), brick_compare);

  brick_node_t bricks_nodes[N_MAX_BRICKS];
  memset(bricks_nodes, 0, sizeof(brick_node_t) * N_MAX_BRICKS);
  for (size_t i = 0; i < n_bricks; ++i) {
    ++max_brick_id;
    brick_t *brick = &bricks[i];
    size_t new_brick_height = brick->z2 - brick->z1 + 1;

    char bricks_under_idx_set[N_MAX_BRICKS];
    memset(bricks_under_idx_set, 0, N_MAX_BRICKS);
    size_t max_current_height = 0;
    for (size_t y = brick->y1; y <= brick->y2; ++y) {
      for (size_t x = brick->x1; x <= brick->x2; ++x) {
        if (ground[y][x].height >= max_current_height) {
          max_current_height = ground[y][x].height;
        }
      }
    }
    size_t bricks_under_counter = 0;
    for (size_t y = brick->y1; y <= brick->y2; ++y) {
      for (size_t x = brick->x1; x <= brick->x2; ++x) {
        top_cube_t *c = &ground[y][x];
        if (c->height == max_current_height && c->brick_id > 0) {
          if (!bricks_under_idx_set[c->brick_id]) {
            ++bricks_under_counter;
            bricks_nodes[c->brick_id - 1].depending_nodes
                [bricks_nodes[c->brick_id - 1].n_depending_nodes++] = i;
          }
          bricks_under_idx_set[c->brick_id] = 1;
        }
        *c = (top_cube_t){.height = new_brick_height + max_current_height,
                          .brick_id = max_brick_id};
      }
    }

    bricks_nodes[i].under_nodes_counter = bricks_under_counter;
    if (bricks_under_counter == 1) {
      for (size_t j = 0; j <= max_brick_id; ++j)
        if (bricks_under_idx_set[j]) {
          disintegrated_bricks_idx_set[j] = 0;
          break;
        }
    }
  }

  size_t disintegrated_bricks_counter = 0;
  for (size_t i = 1; i <= max_brick_id; ++i) {
    if (disintegrated_bricks_idx_set[i]) {
      ++disintegrated_bricks_counter;
    }
    brick_node_t *node = &bricks_nodes[i - 1];
  }

  size_t total_fall_bricks_counter = 0;

  for (size_t i = 0; i < n_bricks; ++i) {
    brick_node_t nodes[N_MAX_BRICKS];
    memcpy(nodes, bricks_nodes, sizeof(brick_node_t) * N_MAX_BRICKS);
    size_t q_nodes[256];
    size_t q_nodes_first = 0;
    size_t q_nodes_last = 0;
    q_nodes[q_nodes_last++] = i;
    while (q_nodes_first < q_nodes_last) {
      brick_node_t *node = &nodes[q_nodes[q_nodes_first++]];
      for (size_t j = 0; j < node->n_depending_nodes; ++j) {
        size_t depending_node_idx = node->depending_nodes[j];
        size_t n = --(nodes[depending_node_idx].under_nodes_counter);
        if (n == 0) {
          q_nodes[q_nodes_last++] = depending_node_idx;
          ++total_fall_bricks_counter;
        }
      }
    }
  }

  printf("part1 disintegrated=%ld\n", disintegrated_bricks_counter);
  assert(disintegrated_bricks_counter == 454);

  printf("part2 fall bricks=%ld\n", total_fall_bricks_counter);
  assert(total_fall_bricks_counter == 74287);
}

int main() {
  part1();
  return 0;
}
