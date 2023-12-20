#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Workflow;

typedef struct Condition {
  char type;
  char part;
  int val;
} cond_t;

typedef struct Rule {
  cond_t cond;
  char send_to;
  char workflow_name[4];
  struct Workflow *workflow;
} rule_t;

typedef struct Workflow {
  char name[4];
  rule_t rules[16];
  size_t n_rules;
} workflow_t;

typedef struct XMAS {
  int x;
  int m;
  int a;
  int s;
} xmas_t;

int xmas_get_part(xmas_t *xmas, char part) {
  switch (part) {
  case 'x':
    return xmas->x;
  case 'm':
    return xmas->m;
  case 'a':
    return xmas->a;
  case 's':
    return xmas->s;
  default:
    printf("unknown part=%c\n", part);
    assert(0);
  }
  return -1;
}

int xmas_run_workflow(xmas_t *xmas, workflow_t *w) {
  size_t i = 0;
  while (i < w->n_rules) {
    rule_t *r = &w->rules[i];
    char is_valid = 0;
    switch (r->cond.type) {
    case '<':
      is_valid = xmas_get_part(xmas, r->cond.part) < r->cond.val;
      break;
    case '>':
      is_valid = xmas_get_part(xmas, r->cond.part) > r->cond.val;
      break;
    case '~':
      is_valid = 1;
      break;
    default:
      assert(0);
    }

    //   printf("%d %c %d == %d, send to %c\n", xmas_get_part(xmas,
    //   r->cond.part),
    //        r->cond.type, r->cond.val, is_valid, r->send_to);

    if (is_valid) {
      if (r->send_to == 'A')
        return 1;
      if (r->send_to == 'R')
        return 0;

      assert(r->send_to == 'W');
      i = 0;
      w = r->workflow;
      // printf("switched to workflow %s\n", w->name);
    } else {
      ++i;
    }
  }

  assert(0);
  return -1;
}

size_t workflow_read(workflow_t *workflows, workflow_t **in_workflow, FILE *f) {
  size_t n_workflows = 0;
  char buffer[256];
  while (fgets(buffer, 256, f)) {
    if (buffer[0] == '\n')
      break;

    char *line = buffer;

    workflow_t w = {.n_rules = 0};
    memset(w.name, 0, 4);
    for (size_t i = 0; *line != '{'; ++i, ++line) {
      w.name[i] = *line;
    }
    assert(*line == '{');
    ++line;
    while (1) {
      rule_t r;
      if (*(line + 1) == '<' || *(line + 1) == '>') {
        r.cond.part = *line;
        ++line;
        assert(*line == '<' || *line == '>');
        r.cond.type = *line;
        ++line;
        r.cond.val = strtol(line, &line, 10);
        assert(*line == ':');
        ++line;
      } else {
        r.cond.type = '~';
        r.cond.val = 0;
      }
      memset(r.workflow_name, 0, 4);
      if (*line == 'R' || *line == 'A') {
        r.send_to = *line;
        ++line;
      } else {
        r.send_to = 'W';
        memset(r.workflow_name, 0, 4);
        for (size_t i = 0; *line != ',' && *line != '}'; ++i, ++line) {
          r.workflow_name[i] = *line;
        }
      }

      w.rules[w.n_rules++] = r;

      if (*line == '}')
        break;

      assert(*line == ',');
      ++line;
    }
    assert(*line == '}');

    workflows[n_workflows++] = w;
  }

  *in_workflow = 0;
  for (size_t i = 0; i < n_workflows; ++i) {
    workflow_t *w = &workflows[i];
    if (strcmp(w->name, "in") == 0) {
      *in_workflow = w;
    }
    for (size_t j = 0; j < w->n_rules; ++j) {
      rule_t *r = &w->rules[j];
      if (r->send_to != 'W')
        continue;
      for (size_t k = 0; k < n_workflows; ++k) {
        if (strcmp(r->workflow_name, workflows[k].name) == 0) {
          r->workflow = &workflows[k];
          break;
        }
      }
    }
  }
  assert(*in_workflow != 0);

  return n_workflows;
}

void part1() {
  FILE *f = fopen("input", "r");

  workflow_t workflows[1024];
  workflow_t *in_workflow = 0;
  size_t n_workflows = workflow_read(workflows, &in_workflow, f);

  int part1_sum = 0;
  char buffer[256];
  while (fgets(buffer, 256, f)) {
    xmas_t xmas;

    char *line = buffer;
    assert(*line == '{');
    line += 3;
    xmas.x = strtol(line, &line, 10);
    assert(*line != '}');
    line += 3;
    xmas.m = strtol(line, &line, 10);
    assert(*line != '}');
    line += 3;
    xmas.a = strtol(line, &line, 10);
    line += 3;
    assert(*line != '}');
    xmas.s = strtol(line, &line, 10);
    assert(*line == '}');

    if (xmas_run_workflow(&xmas, in_workflow)) {
      part1_sum += xmas.x + xmas.m + xmas.a + xmas.s;
    }
  }

  printf("part1 sum=%d\n", part1_sum);
  assert(part1_sum == 319062);
}

int compare_ints(const void *v1, const void *v2) {
  int n1 = *(int *)v1;
  int n2 = *(int *)v2;

  if (n1 < n2)
    return -1;
  if (n1 > n2)
    return 1;
  return 0;
}

typedef struct Range {
  long int from;
  long int to;
} range_t;

const range_t RANGE_EMPTY = {.from = 0, .to = 0};
int range_is_empty(const range_t *r) {
  return r->from == RANGE_EMPTY.from && r->to == RANGE_EMPTY.to;
}

typedef struct XMASRange {
  range_t x;
  range_t m;
  range_t a;
  range_t s;
} xmas_range_t;

const xmas_range_t XMAS_RANGE_EMPTY = {
    .x = RANGE_EMPTY, .m = RANGE_EMPTY, .a = RANGE_EMPTY, .s = RANGE_EMPTY};

int xmas_range_is_empty(const xmas_range_t *r) {
  return range_is_empty(&r->x) || range_is_empty(&r->m) ||
         range_is_empty(&r->a) || range_is_empty(&r->s);
}

void range_split_less(range_t *r, long int val, range_t *lr, range_t *rr) {
  if (range_is_empty(r)) {
    *lr = RANGE_EMPTY;
    *rr = RANGE_EMPTY;
    return;
  }
  if (r->from <= val && r->to >= val) {
    *lr = (range_t){.from = r->from, .to = val - 1};
    *rr = (range_t){.from = val, .to = r->to};
    return;
  }
  if (r->from > val) {
    *lr = RANGE_EMPTY;
    *rr = *r;
    return;
  }
  if (r->to < val) {
    *lr = *r;
    *rr = RANGE_EMPTY;
    return;
  }

  assert(0);
}

void range_split_more(range_t *r, long int val, range_t *lr, range_t *rr) {
  if (range_is_empty(r)) {
    *lr = RANGE_EMPTY;
    *rr = RANGE_EMPTY;
    return;
  }
  if (r->from <= val && r->to >= val) {
    *lr = (range_t){.from = val + 1, .to = r->to};
    *rr = (range_t){.from = r->from, .to = val};
    return;
  }
  if (r->from > val) {
    *lr = *r;
    *rr = RANGE_EMPTY;
    return;
  }
  if (r->to < val) {
    *lr = RANGE_EMPTY;
    *rr = *r;
    return;
  }

  assert(0);
}

range_t *xmas_range_part(xmas_range_t *r, char part) {
  switch (part) {
  case 'x':
    return &r->x;
  case 'm':
    return &r->m;
  case 'a':
    return &r->a;
  case 's':
    return &r->s;
  default:
    assert(0);
  }
}

void xmas_range_split_less(xmas_range_t *r, char part, long int val,
                           xmas_range_t *lr, xmas_range_t *rr) {
  *lr = *r;
  *rr = *r;
  range_split_less(xmas_range_part(r, part), val, xmas_range_part(lr, part),
                   xmas_range_part(rr, part));
}

void xmas_range_split_more(xmas_range_t *r, char part, long int val,
                           xmas_range_t *lr, xmas_range_t *rr) {
  *lr = *r;
  *rr = *r;
  range_split_more(xmas_range_part(r, part), val, xmas_range_part(lr, part),
                   xmas_range_part(rr, part));
}

void xmas_range_print(const xmas_range_t *r) {
  printf("{x:[%ld-%ld] m:[%ld-%ld]} a:[%ld-%ld]} s:[%ld-%ld]}\n", r->x.from,
         r->x.to, r->m.from, r->m.to, r->a.from, r->a.to, r->s.from, r->s.to);
}

xmas_range_t ACCEPTED[1024];
size_t N_ACCEPTED = 0;
void find_accepted(xmas_range_t range, workflow_t *w) {
  for (size_t i = 0; i < w->n_rules; ++i) {
    rule_t *r = &w->rules[i];
    xmas_range_t left_range;
    xmas_range_t right_range;
    switch (r->cond.type) {
    case '<':
      xmas_range_split_less(&range, r->cond.part, r->cond.val, &left_range,
                            &right_range);
      break;
    case '>':
      xmas_range_split_more(&range, r->cond.part, r->cond.val, &left_range,
                            &right_range);
      break;
    case '~':
      left_range = range;
      right_range = XMAS_RANGE_EMPTY;
      break;
    default:
      assert(0);
    }

    if (r->send_to == 'A') {
      ACCEPTED[N_ACCEPTED++] = left_range;
    }
    if (r->send_to == 'R') {
    }
    if (r->send_to == 'W') {
      if (!xmas_range_is_empty(&left_range))
        find_accepted(left_range, r->workflow);
    }
    if (xmas_range_is_empty(&right_range))
      return;
    range = right_range;
  }
}

size_t calc_combinations(const xmas_range_t *r) {
  return (r->x.to - r->x.from + 1) * (r->m.to - r->m.from + 1) *
         (r->a.to - r->a.from + 1) * (r->s.to - r->s.from + 1);
}

void part2() {
  FILE *f = fopen("input", "r");

  workflow_t workflows[1024];
  workflow_t *in_workflow = 0;
  size_t n_workflows = workflow_read(workflows, &in_workflow, f);
  find_accepted((xmas_range_t){.x = (range_t){.from = 1, .to = 4000},
                               .m = (range_t){.from = 1, .to = 4000},
                               .a = (range_t){.from = 1, .to = 4000},
                               .s = (range_t){.from = 1, .to = 4000}},
                in_workflow);

  size_t n_combinations = 0;
  for (size_t i = 0; i < N_ACCEPTED; ++i) {
    n_combinations += calc_combinations(&ACCEPTED[i]);
  }

  printf("part2 combinations=%lu\n", n_combinations);
  assert(n_combinations == 118638369682135);
}

int main() {
  part1();
  part2();
  return 0;
}
