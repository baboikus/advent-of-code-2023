#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Hand {
  char cards[5];
  int bid;
  int strength;
} hand_t;

char card_value(char card) {
  if (card >= '1' && card <= '9')
    return card - '0';
  switch (card) {
  case 'T':
    return 10;
  case 'J':
    return 11;
  case 'Q':
    return 12;
  case 'K':
    return 13;
  case 'A':
    return 14;
  }

  assert(0);
  return 0;
};

int calc_hand_pattern(hand_t *hand, char *pattern) {
  for (size_t i = 0; i < 15; ++i)
    pattern[i] = 0;

  int jokers_counter = 0;
  for (size_t i = 0; i < 5; ++i) {
    if (hand->cards[i] == 0) {
      jokers_counter++;
    } else {
      pattern[hand->cards[i]]++;
    }
  }
  return jokers_counter;
}

int compare_chars(const void *v1, const void *v2) {
  const char c1 = *(const char *)v1;
  const char c2 = *(const char *)v2;
  if (c1 < c2)
    return 1;
  if (c1 > c2)
    return -1;
  return 0;
}

int calc_hand_srength(hand_t *hand) {
  char hp[15];
  int jokers_counter = calc_hand_pattern(hand, hp);

  qsort(hp, 15, sizeof(char), compare_chars);

  hp[0] += jokers_counter;

  if (hp[0] == 5)
    return 7;
  if (hp[0] == 4)
    return 6;
  if (hp[0] == 3 && hp[1] == 2)
    return 5;
  if (hp[0] == 3)
    return 4;
  if (hp[0] == 2 && hp[1] == 2)
    return 3;
  if (hp[0] == 2)
    return 2;

  assert(hp[0] == 1);
  return 1;
};

void parse_hand(char *line, hand_t *hand) {
  for (int i = 0; i < 5; ++i, ++line)
    hand->cards[i] = card_value(*line);
  hand->bid = strtol(line, &line, 10);
  hand->strength = 0;
}

int compare_hands(const void *v1, const void *v2) {
  const hand_t *h1 = *(const hand_t **)v1;
  const hand_t *h2 = *(const hand_t **)v2;

  if (h1->strength < h2->strength)
    return -1;
  if (h1->strength > h2->strength)
    return 1;

  for (size_t i = 0; i < 5; ++i) {
    if (h1->cards[i] < h2->cards[i])
      return -1;
    if (h1->cards[i] > h2->cards[i])
      return 1;
  }

  return 0;
}

#define N_HANDS 1000

void part1_part2() {
  FILE *fptr;
  fptr = fopen("input", "r");

  char buffer[64];
  hand_t hands[N_HANDS];
  hand_t *sorted_hands[N_HANDS];
  size_t hands_idx = 0;
  while (fgets(buffer, 64, fptr)) {
    hand_t *hand = &hands[hands_idx];
    parse_hand(buffer, hand);
    hand->strength = calc_hand_srength(hand);
    sorted_hands[hands_idx] = hand;
    hands_idx++;
  }

  qsort(sorted_hands, N_HANDS, sizeof(hand_t *), compare_hands);

  int total_winnings = 0;
  for (size_t i = 0; i < N_HANDS; ++i) {
    total_winnings += (i + 1) * sorted_hands[i]->bid;
  }

  printf("part1 total winnings = %d\n", total_winnings);
  assert(total_winnings == 251029473);

  for (size_t i = 0; i < N_HANDS; ++i) {
    hand_t *hand = sorted_hands[i];
    for (size_t n = 0; n < 5; ++n) {
      if (hand->cards[n] == 11)
        hand->cards[n] = 0;
    }
    hand->strength = calc_hand_srength(hand);
  }

  qsort(sorted_hands, N_HANDS, sizeof(hand_t *), compare_hands);

  total_winnings = 0;
  for (size_t i = 0; i < N_HANDS; ++i) {
    total_winnings += (i + 1) * sorted_hands[i]->bid;
  }

  printf("part2 total winnings = %d\n", total_winnings);
  assert(total_winnings == 251003917);
}

int main() {
  part1_part2();
  return 0;
}
