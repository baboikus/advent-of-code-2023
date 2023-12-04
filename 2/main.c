#include <stdio.h>
#include <assert.h>

typedef struct Reveal {
    int red;
    int green;
    int blue;
} reveal_t;

char* game_parse(char *str, int *game_num) {
    assert(*str == 'G');
    str += 5;
    int acc = 0;

    while(*str && *str != ':') {
        acc = acc * 10 + (*str - '0'), str++;
    }
    *game_num = acc;
    str++;
    return str;
}

char* reveal_parse(char* str, reveal_t *r){
    if(*str == ';') str++;

    *r = (reveal_t){0, 0, 0};

    assert(*str == ' ');
    int acc = 0;
    while(*str && *str != ';' && *str != '\n') {
        if(*str >= '0' && *str <= '9') acc = acc * 10 + (*str - '0');
        if(*str == 'r') r->red = acc, str += 2, acc = 0;
        if(*str == 'g') r->green = acc, str += 4, acc = 0;
        if(*str == 'b') r->blue = acc, str += 3, acc = 0;
        str++;
    }


    return str;
}

int max(int i1, int i2) {
    return i1 > i2 ? i1 : i2;
}

reveal_t* reveal_union_max(reveal_t *r_union, reveal_t *r_addtion) {
    r_union->red = max(r_union->red, r_addtion->red);
    r_union->green = max(r_union->green, r_addtion->green);
    r_union->blue = max(r_union->blue, r_addtion->blue);

    return r_union;
}

int reveal_power(reveal_t *r) {
    return r->red * r->green * r->blue;
}

int is_game_possible(reveal_t *r) {
    return r->red <= 12 && r->green <= 13 && r->blue <= 14;
}

void reveal_print(reveal_t *r) {
    printf("{%d red, %d green, %d blue}\n", r->red, r->green, r->blue);
}

void part1() {
  FILE *fptr;
  fptr = fopen("input", "r");

  char buffer[256];

  int sum = 0;
  while (fgets(buffer, 256, fptr)) {
    printf("%s", buffer);
    char* line = buffer;
    int game_num = 0;
    line = game_parse(line, &game_num);
    printf("game %d: ", game_num);
    int imposible_found = 0;
    while(*line && *line != '\n') {
        reveal_t r;
        line = reveal_parse(line, &r);
        reveal_print(&r);
        if(!is_game_possible(&r)) {
            imposible_found = 1;
            break;
        }
    }

    if(!imposible_found) sum += game_num;

  }
  printf("sum=%d\n", sum);
}

void part2() {
  FILE *fptr;
  fptr = fopen("example", "r");

  char buffer[256];

  int sum = 0;
  while (fgets(buffer, 256, fptr)) {
    printf("%s", buffer);
    char* line = buffer;
    int game_num = 0;
    line = game_parse(line, &game_num);
    reveal_t r_union = {0, 0, 0};
    while(*line && *line != '\n') {
        reveal_t r;
        line = reveal_parse(line, &r);
        reveal_union_max(&r_union, &r);
    }

    sum += reveal_power(&r_union);

  }
  printf("sum=%d\n", sum);
}

int main() {
//  part1();
  part2();

  return 0;
}
