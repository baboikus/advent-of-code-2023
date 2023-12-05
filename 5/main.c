#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(x, y) (x < y ? x : y)

typedef size_t u64;

typedef struct MapRow {
  u64 src_start;
  u64 dst_start;
  u64 len;
} map_row_t;

typedef map_row_t *map_t;

typedef struct Range {
  u64 start;
  u64 len;
} range_t;

void seeds_read(FILE *f, u64 *seeds) {
  char buffer[256];
  fgets(buffer, 256, f);

  char *line = buffer;
  line += 6;

  while (*(seeds++) = strtol(line, &line, 10))
    ;
  seeds = 0;
  fgets(buffer, 256, f);
  assert(buffer[0] == '\n');
}

void ranges_read(FILE *f, range_t *ranges) {
  char buffer[256];
  fgets(buffer, 256, f);

  char *line = buffer;
  line += 6;

  for (; ranges->start = strtol(line, &line, 10); ranges++) {
    ranges->len = strtol(line, &line, 10);
  }

  ranges->len = 0;
  fgets(buffer, 256, f);
  assert(buffer[0] == '\n');
}

void map_read(FILE *f, map_t map) {
  char buffer[256];
  if (feof(f) != 0) {
    map->len = 0;
    return;
  }

  fgets(buffer, 256, f);
  for (;;) {
    if (fgets(buffer, 256, f) == NULL || buffer[0] == '\n') {
      map->len = 0;
      return;
    }

    char *line = buffer;
    map->dst_start = strtol(line, &line, 10);
    map->src_start = strtol(line, &line, 10);
    map->len = strtol(line, &line, 10);
    map++;
  }
}

u64 map_project(map_t map, u64 src) {
  while (map->len > 0) {
    if (src >= map->src_start && src < (map->src_start + map->len)) {
      return map->dst_start + (src - map->src_start);
    }
    map++;
  }
  return src;
}

range_t *map_project_range(map_t map, range_t src, range_t *dst) {
  map_t map_start = map;
  range_t q_src[32];
  size_t q_src_head_idx = 0, q_src_tail_idx = 1;

  q_src[q_src_head_idx] = src;
  while (map->len > 0) {
    if (q_src_head_idx == q_src_tail_idx)
      break;

    src = q_src[q_src_head_idx];
    if (src.start <= map->src_start &&
        (src.start + src.len) >= map->src_start &&
        (src.start + src.len) <= (map->src_start + map->len)) {
      q_src_head_idx++;

      dst->start = map->dst_start;
      dst->len = (src.start + src.len) - map->src_start;
      dst++;
      if (src.start < map->src_start) {
        q_src[q_src_tail_idx++] =
            (range_t){.start = src.start, .len = map->src_start - src.start};
      }
    } else if (src.start >= map->src_start &&
               (src.start + src.len) <= (map->src_start + map->len)) {
      q_src_head_idx++;

      dst->start = map->dst_start + (src.start - map->src_start);
      dst->len = src.len;
      dst++;
    } else if (src.start >= map->src_start &&
               src.start <= (map->src_start + map->len) &&
               (src.start + src.len) >= (map->src_start + map->len)) {
      q_src_head_idx++;

      dst->start = map->dst_start + (src.start - map->src_start);
      dst->len = (map->src_start + map->len) - src.start;
      dst++;
      if ((src.start + src.len) > (map->src_start + map->len)) {
        q_src[q_src_tail_idx++] = (range_t){.start = map->src_start + map->len,
                                            .len = (src.start + src.len) -
                                                   (map->src_start + map->len)};
      }
    } else if (src.start < map->src_start &&
               (src.start + src.len) > (map->src_start + map->len)) {
      q_src_head_idx++;
      dst->start = map->dst_start;
      dst->len = map->len;
      dst++;

      q_src[q_src_tail_idx++] =
          (range_t){.start = src.start, .len = map->src_start - src.start};
      q_src[q_src_tail_idx++] =
          (range_t){.start = map->src_start + map->len,
                    .len = (src.start + src.len) - (map->src_start + map->len)};
    }

    map++;
  }

  while(q_src_head_idx != q_src_tail_idx) {
      *dst++ = q_src[q_src_head_idx++];
  }

  dst->len = 0;
  return dst;
}

void part1() {
  FILE *fptr;
  fptr = fopen("input", "r");

  char buffer[512];
  u64 current_mapping[128];

  seeds_read(fptr, current_mapping);

  map_row_t map[256];
  for (; feof(fptr) == 0;) {
    map_read(fptr, map);
    for (size_t i = 0; current_mapping[i]; ++i) {
      current_mapping[i] = map_project(map, current_mapping[i]);
    }
  }
  fclose(fptr);

  u64 min = LONG_MAX;
  for (size_t i = 0; current_mapping[i]; ++i)
    min = MIN(min, current_mapping[i]);
  printf("part1 min = %ld\n", min);
  assert(min == 57075758);
}

void part2() {
  FILE *fptr;
  fptr = fopen("input", "r");

  char buffer[512];
  range_t mapping[2][256];
  range_t *current_mapping = mapping[0];
  range_t *new_mapping = mapping[1];

  ranges_read(fptr, current_mapping);
  map_row_t map[256];
  for (; feof(fptr) == 0;) {
    map_read(fptr, map);
    range_t* new_mapping_tail = new_mapping;
    for (size_t n = 0; current_mapping[n].len; ++n) {
      new_mapping_tail = map_project_range(map, current_mapping[n], new_mapping_tail);
    }

    range_t *tmp = current_mapping;
    current_mapping = new_mapping;
    new_mapping = tmp;
  }
  fclose(fptr);

  u64 min = LONG_MAX;
  for (size_t i = 0; current_mapping[i].len; ++i)
    min = MIN(min, current_mapping[i].start);
  printf("part2 min = %ld\n", min);
  assert(min == 31161857);
}

int main() {
  part1();
  part2();

  return 0;
}
