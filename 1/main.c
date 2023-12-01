#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define N_SYMBOLS ('z' - 'a' + 1)
#define N_STATES 64
#define RULE_FOR_SYMBOL(c) (c - 'a')

typedef struct DFA_Rule {
    size_t states_to[N_STATES];
    size_t n_states_to;
    int emit_digit;
}
dfa_rule_t;

typedef struct DFA {
    dfa_rule_t rules_table[N_STATES][N_SYMBOLS];
    char alphabet[N_SYMBOLS];
    size_t current_states[N_STATES];
    size_t new_state;
}
dfa_t;

void dfa_init(dfa_t * dfa) {
    for (size_t state = 0; state < N_STATES; ++state) {
        for (char c = 'a'; c <= 'z'; ++c) {
            dfa_rule_t * rule = & dfa -> rules_table[state][RULE_FOR_SYMBOL(c)];
            rule -> n_states_to = 0;
            rule -> emit_digit = 0;
        }
    }
    dfa -> new_state = 1;
}

void dfa_reset(dfa_t * dfa) {
    dfa -> current_states[0] = 1;
    dfa -> current_states[1] = 0;
}

void dfa_print(dfa_t * dfa) {
    printf("alphabet = {");
    for (char c = 'a'; c <= 'z'; ++c) {
        if (dfa -> alphabet[RULE_FOR_SYMBOL(c)]) {
            printf("%c, ", dfa -> alphabet[RULE_FOR_SYMBOL(c)]);
        }
    }
    printf("}\n");

    for (size_t state = 0; state < N_STATES; ++state) {
        for (char c = 'a'; c <= 'z'; ++c) {
            dfa_rule_t * rule = & dfa -> rules_table[state][(size_t)(RULE_FOR_SYMBOL(c))];

            if (rule -> n_states_to == 0) continue;

            printf("s=%ld -'%c'-> {", state, c);
            for (size_t i = 0; i < rule -> n_states_to; ++i) {
                printf("%ld, ", rule -> states_to[i]);
            }
            printf("}");
            if (rule -> emit_digit > 0) {
                printf(" emit %d ", rule -> emit_digit);
            }
            printf("\n");
        }
    }

    printf("current_states={");
    for (size_t i = 0; dfa -> current_states[i]; ++i) {
        printf("%ld, ", dfa -> current_states[i]);
    }
    printf("}\n");
}

void dfa_add_word(dfa_t * dfa, char * word, int digit) {
    if (!word || ! * word) return;

    size_t states[N_STATES];
    size_t n_states = 1;
    states[0] = 1;
    while ( * word) {
        size_t next_states[N_STATES];
        size_t n_next_states = 0;
        for (size_t i = 0; i < n_states; ++i) {
            dfa_rule_t * rule = & dfa -> rules_table[states[i]][RULE_FOR_SYMBOL( * word)];
            if (rule -> n_states_to == 0) {
                rule -> states_to[rule -> n_states_to++] = ++(dfa -> new_state);
            }

            for (size_t n = 0; n < rule -> n_states_to; ++n) {
                next_states[n_next_states++] = rule -> states_to[n];
            }

            if (! * (word + 1)) rule -> emit_digit = digit;
        }

        for (size_t i = 0; i < n_next_states; ++i) {
            states[i] = next_states[i];
        }
        n_states = n_next_states;
        n_next_states = 0;

        dfa -> alphabet[ * word] = 1;

        ++word;
    }
}

void dfa_build(dfa_t * dfa) {
    for (char c = 'a'; c <= 'z'; ++c) {
        dfa_rule_t * start_rule = & dfa -> rules_table[1][RULE_FOR_SYMBOL(c)];
        if (start_rule -> n_states_to == 0) continue;
        for (size_t i = 2; i < dfa -> new_state; ++i) {
            dfa_rule_t * r = & dfa -> rules_table[i][RULE_FOR_SYMBOL(c)];
            r -> states_to[r -> n_states_to++] = start_rule -> states_to[0];
        }
    }
    dfa_reset(dfa);
}

int dfa_step(dfa_t * dfa, char c) {
    size_t new_states[N_STATES];
    size_t n_new_states = 0;
    for (size_t * state = dfa -> current_states;* state; state++) {
        dfa_rule_t * r = & dfa -> rules_table[ * state][RULE_FOR_SYMBOL(c)];
        if (r -> emit_digit > 0) return r -> emit_digit;
        for (size_t n = 0; n < r -> n_states_to; ++n) {
            new_states[n_new_states++] = r -> states_to[n];
        }
    }
    for (size_t n = 0; n < n_new_states; ++n) {
        dfa -> current_states[n] = new_states[n];
    }
    if (n_new_states > 0) {
        dfa -> current_states[n_new_states] = 0;
    } else {
        dfa_reset(dfa);
    }
    return 0;
}

int dfa_find_digit(dfa_t * dfa, char * s) {
    while (s) {
        if ( * s >= '1' && * s <= '9') return * s - '0';
        int r = dfa_step(dfa, * s);
        if (r > 0) return r;
        ++s;
    }

    return 0;
}

int dfa_find_digit_reversed(dfa_t * dfa, char * s) {
    char * b = s + strlen(s) - 2;
    while (b >= s) {
        if ( * b >= '1' && * b <= '9') return * b - '0';
        int r = dfa_step(dfa, * b);
        if (r > 0) return r;
        --b;
    }

    return 0;
}

void part1() {
    FILE * fptr;
    fptr = fopen("input", "r");

    char buffer[256];

    int d1 = 0, d2 = 0, sum = 0;
    while (fgets(buffer, 256, fptr)) {
        printf("%s", buffer);

        for (char * c = buffer; c; ++c) {
            if ( * c >= '0' && * c <= '9') {
                d1 = * c - '0';
                break;
            }
        }

        for (char * c = buffer + strlen(buffer) - 1; c; --c) {
            if ( * c >= '0' && * c <= '9') {
                d2 = * c - '0';
                break;
            }
        }

        printf("d1=%d d2=%d\n", d1, d2);

        sum += d1 * 10 + d2;
    }

    printf("sum=%d\n", sum);
}

void part2() {
    char * digits[] = {
        "one",
        "two",
        "three",
        "four",
        "five",
        "six",
        "seven",
        "eight",
        "nine"
    };
    char reversed_digit[16];

    dfa_t * dfa = malloc(sizeof(dfa_t));
    dfa_init(dfa);
    dfa_t * reversed_dfa = malloc(sizeof(dfa_t));
    dfa_init(reversed_dfa);

    for (size_t i = 0; i < 9; ++i) {
        dfa_add_word(dfa, digits[i], i + 1);

        size_t n_digit = strlen(digits[i]);
        for (size_t r = n_digit, l = 0; r > 0; --r, ++l) {
            reversed_digit[l] = digits[i][r - 1];
        }
        reversed_digit[n_digit] = '\0';
        dfa_add_word(reversed_dfa, reversed_digit, i + 1);
    }

    dfa_build(dfa);
    dfa_build(reversed_dfa);

    FILE * fptr;
    fptr = fopen("input", "r");

    char buffer[256];

    int d1 = 0, d2 = 0, sum = 0;
    while (fgets(buffer, 256, fptr)) {
        dfa_reset(dfa);
        d1 = dfa_find_digit(dfa, buffer);
        dfa_reset(reversed_dfa);
        d2 = dfa_find_digit_reversed(reversed_dfa, buffer);
        sum += d1 * 10 + d2;
    }

    free(dfa);
    free(reversed_dfa);

    printf("sum=%d\n", sum);
}

int main() {
    part1();
    part2();

    return 0;
}
