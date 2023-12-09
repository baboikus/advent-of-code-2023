#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
  size_t left_idx;
  size_t right_idx;
  char is_final;
} node_t;

#define N_LABEL_RANGE ('Z' - 'A' + 1)
#define N_NODES (N_LABEL_RANGE * N_LABEL_RANGE * N_LABEL_RANGE)

typedef struct Map {
  node_t nodes[N_NODES];
} map_t;

typedef struct PathState {
  size_t node_idx;
  size_t instruction_idx;
} path_state_t;

typedef struct PathShortcat {
  size_t from_node_idx;
  size_t from_instruction_idx;

  size_t to_node_idx;
  size_t to_instruction_idx;

  size_t n_steps;
} path_shortcat_t;

size_t node_idx(char *node) {
  assert(node[0] >= 'A' && node[0] <= 'Z');
  assert(node[1] >= 'A' && node[1] <= 'Z');
  assert(node[2] >= 'A' && node[2] <= 'Z');

  return (node[0] - 'A') * N_LABEL_RANGE * N_LABEL_RANGE +
         (node[1] - 'A') * N_LABEL_RANGE + (node[2] - 'A');
}

size_t map_step(map_t *map, size_t from_node_idx, char instruction) {
  if (instruction == 'L') {
    return map->nodes[from_node_idx].left_idx;
  } else if (instruction == 'R') {
    return map->nodes[from_node_idx].right_idx;
  } else {
    assert(0);
  }
  return 0;
};

void prime_factorization(size_t n, char* primes) {
    for(size_t i = 2; i <= n; ++i) {
        if(n % i == 0) {
           primes[i]++;
           n /= i;
           i = 1;
        }
    }
}

size_t find_shortcat(path_shortcat_t *shorcats, size_t from_idx,
                     size_t instruction_idx) {
  for (size_t s = 0; shorcats->n_steps; ++shorcats, ++s) {
    if (shorcats->from_node_idx == from_idx &&
        shorcats->from_instruction_idx == instruction_idx) {
      return s;
    }
  }
  return 0;
}

void part1_part2() {
  FILE *fptr;
  fptr = fopen("input", "r");

  char instructions[512];
  fgets(instructions, 512, fptr);
  size_t n_instructions = strlen(instructions) - 1;

  char buffer[64];
  fgets(buffer, 64, fptr);

  map_t map;
  for (size_t i = 0; i < N_NODES; ++i) {
    map.nodes[i] = (node_t){.left_idx = 0, .right_idx = 0, .is_final = 0};
  }

  size_t current_nodes_idx[N_LABEL_RANGE * N_LABEL_RANGE];
  size_t n_current_nodes = 0;

  while (fgets(buffer, 64, fptr)) {
    char *from = buffer;
    char *left = buffer + 7;
    char *right = buffer + 12;

    node_t *node = &map.nodes[node_idx(from)];
    assert(node->left_idx == 0 && node->right_idx == 0);
    *node = (node_t){.left_idx = node_idx(left),
                     .right_idx = node_idx(right),
                     .is_final = (from[2] == 'Z')};

    if (from[2] == 'A') {
      current_nodes_idx[n_current_nodes++] = node_idx(from);
    }
  }

  /* PART 1 */
  size_t current_idx = node_idx("AAA");
  const size_t final_idx = node_idx("ZZZ");
  size_t steps_counter = 0;
  while (1) {
    for (char *i = instructions; i && *i != '\n'; ++i) {
      current_idx = map_step(&map, current_idx, *i);
      steps_counter++;
      if (current_idx == final_idx)
        goto part1_end;
    }
  }
part1_end:
  printf("part1 steps = %ld\n", steps_counter);
  assert(steps_counter == 19631);

  /* PART 2 */
  path_shortcat_t shortcats[1024];
  size_t n_shortcats = 0;
  for (size_t n = 0; n < n_current_nodes; ++n) {
    current_idx = current_nodes_idx[n];
    steps_counter = 0;
    shortcats[n_shortcats].from_node_idx = current_idx;
    shortcats[n_shortcats].from_instruction_idx = 0;
    while (1) {
      for (size_t i = 0; instructions[i] != '\n'; ++i) {
        current_idx = map_step(&map, current_idx, instructions[i]);
        steps_counter++;
        if (map.nodes[current_idx].is_final) {
          size_t next_i_idx = ((i + 1) % n_instructions);
          shortcats[n_shortcats].to_node_idx = current_idx;
          shortcats[n_shortcats].to_instruction_idx = next_i_idx;
          shortcats[n_shortcats].n_steps = steps_counter;

          for (size_t s = 0; s < n_shortcats; ++s) {
            if (shortcats[s].from_node_idx == current_idx &&
                shortcats[s].from_instruction_idx == next_i_idx) {
              goto cycle_found;
            }
          }
          n_shortcats++;
          assert(n_shortcats < 1024);
          shortcats[n_shortcats].from_node_idx = current_idx;
          shortcats[n_shortcats].from_instruction_idx = next_i_idx;
          steps_counter = 0;
        }
      }
    }

  cycle_found: /* goto next node */;
  }
  shortcats[n_shortcats].n_steps = 0;

  for (size_t s = 0; s < n_shortcats; s += 2) {
    path_shortcat_t *ps1 = &(shortcats[s]);
        path_shortcat_t *ps2 = &(shortcats[s+1]);
        assert(ps1->n_steps == ps2->n_steps);
        assert(ps1->to_node_idx == ps2->from_node_idx);
                assert(ps2->from_node_idx == ps2->to_node_idx);
  }

  #define N_PRIMES 512
  char lcm_primes[N_PRIMES];
  memset(lcm_primes, 0, N_PRIMES);
  for (size_t s = 0; s < n_shortcats; s += 2) {
      char primes[N_PRIMES];
      memset(primes, 0, N_PRIMES);
      prime_factorization(shortcats[s].n_steps, primes);
      for(size_t p = 0; p < N_PRIMES; ++p) {
          if(lcm_primes[p] < primes[p]) lcm_primes[p] = primes[p];
      }
  }

  steps_counter = 1;
  for(size_t p = 0; p < N_PRIMES; ++p) {
      if(lcm_primes[p]) {
          steps_counter *= p * lcm_primes[p];
      }
  }

part2_end:
  printf("part2 steps = %ld\n", steps_counter);
  assert(steps_counter == 21003205388413);
}

int main() {
  part1_part2();
  return 0;
}
