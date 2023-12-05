#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Card {
  int num;
  char numbers[128];
} card_t;

char *parse_card(char *str, card_t *card) {
  str += 4;
  card->num = strtol(str, &str, 10);
  str++;

  memset(card->numbers, 0, 128);
  while (*(str + 1) != '|') {
    size_t idx = strtol(str, &str, 10);
    card->numbers[idx] = 1;
  }

  return str + 1;
}

void part1_part2() {
  FILE *fptr;
  fptr = fopen("input", "r");

  char buffer[512];
  int part1_sum = 0;
  int part2_sum = 0;
  int copy_counter[256];
  size_t current_ticket_idx = 0;

  for (size_t i = 0; i < 256; ++i)
    copy_counter[i] = 1;

  while (fgets(buffer, 512, fptr)) {
    card_t card;
    char *line = parse_card(buffer, &card);
    line++;
    size_t winnable_nums_count = 0;
    for (;;) {
      char *rest;
      size_t idx = strtol(line, &rest, 10);
      if (line == rest)
        break;
      if (card.numbers[idx] == 1) {
        winnable_nums_count++;
      }
      line = rest;
    }

    if (winnable_nums_count > 0) {
      part1_sum += 1 << (winnable_nums_count - 1);
    }

    const size_t current_ticket_count = copy_counter[current_ticket_idx];
    part2_sum += current_ticket_count;
    for (size_t n = 1; n <= winnable_nums_count; n++) {
      copy_counter[current_ticket_idx + n] += current_ticket_count;
    }

    current_ticket_idx++;
  }

  printf("part1 sum=%d\n", part1_sum);
  printf("part2 sum=%d\n", part2_sum);

  assert(part1_sum == 17782);
  assert(part2_sum == 8477787);
}

int main() {
  part1_part2();
  return 0;
}
