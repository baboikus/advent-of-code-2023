#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ModuleState {
  size_t n_inputs;

  char is_on;
  char recent_pulses[16];
} module_state_t;

typedef struct ModuleOutput {
  size_t module_idx;
  size_t input_idx;
  char pulse;
} module_output_t;

typedef struct Module {
  char name[16];
  size_t idx;

  char type;
  module_output_t outputs[16];

  size_t n_inputs;
  size_t n_outputs;
} module_t;


char module_any_recent_low_pulses(module_state_t *s) {
  for (size_t i = 0; i < s->n_inputs; ++i) {
    if (s->recent_pulses[i] == 'l')
      return 1;
    assert(s->recent_pulses[i] == 'H');
  }
  return 0;
}

size_t LOW_COUNTER = 0;
size_t HIGHT_COUNTER = 0;
int push_button(module_t *modules, size_t n_modules,
                 const size_t broadcaster_idx, module_state_t *init_state,
                 module_state_t *next_state) {
  module_state_t *current_state = init_state;

  size_t steps_counter = 0;
  char has_any_alive_pulses = 1;

  module_output_t pulse_qs[2][256];
  module_output_t *current_pulse_q = pulse_qs[0];
  size_t n_current_pulse_q = 0;
  module_output_t *next_pulse_q = pulse_qs[1];
  size_t n_next_pulse_q = 0;

  current_pulse_q[n_current_pulse_q++] = (module_output_t){
      .module_idx = broadcaster_idx, .input_idx = 0, .pulse = 'l'};
  assert(n_current_pulse_q == 1);

  int vf_pulse_index = -1;

  while (has_any_alive_pulses) {
    has_any_alive_pulses = 0;
    memcpy(next_state, current_state, sizeof(module_state_t) * n_modules);
    for (size_t i = 0; i < n_current_pulse_q; ++i) {
      size_t module_idx = current_pulse_q[i].module_idx;
      module_t *m = &modules[module_idx];
      module_state_t *s = &current_state[module_idx];

      size_t pulse_input_idx = current_pulse_q[i].input_idx;
      char *pulse_input = &current_pulse_q[i].pulse;
      if (pulse_input == 0)
        continue;
      else
        has_any_alive_pulses = 1;
      if (*pulse_input == 'H' && m->name[0] == 'v' && m->name[1] == 'f') {
        vf_pulse_index = pulse_input_idx;
      }
      if (*pulse_input == 'l') {
        LOW_COUNTER++;
      }
      if (*pulse_input == 'H') {
        HIGHT_COUNTER++;
      }

      char output = 0;
      switch (m->type) {
      case '%':
        if (*pulse_input == 'l') {
          if (s->is_on) {
            output = 'l';
          } else {
            output = 'H';
          }
          next_state[module_idx].is_on = !s->is_on;
        } else {
        }
        break;
      case '&':
        next_state[module_idx].recent_pulses[pulse_input_idx] = *pulse_input;
        if (module_any_recent_low_pulses(&next_state[module_idx])) {
          output = 'H';
        } else {
          output = 'l';
        }
        break;
      case 'b':
        if (*pulse_input == 'l') {
          output = 'l';
        }
        break;
      default:
        break;
      }

      if (output == 0)
        continue;

      for (size_t j = 0; j < m->n_outputs; ++j) {
        module_output_t o = m->outputs[j];
        o.pulse = output;
        next_pulse_q[n_next_pulse_q++] = o;
      }
    }
    module_state_t *tmp_s = current_state;
    current_state = next_state;
    next_state = tmp_s;

    module_output_t *tmp_o = current_pulse_q;
    current_pulse_q = next_pulse_q;
    next_pulse_q = tmp_o;

    n_current_pulse_q = n_next_pulse_q;
    n_next_pulse_q = 0;
  }

  return vf_pulse_index;
}

module_t *register_module(char *name, module_t *modules, size_t *n_modules) {
  for (size_t i = 0; i < *n_modules; ++i) {
    if (strcmp(name, modules[i].name) == 0)
      return &modules[i];
  }
  module_t *new_m = &modules[*n_modules];
  memcpy(new_m->name, name, 16);
  new_m->idx = *n_modules;
  new_m->type = '?';
  new_m->n_inputs = 0;
  new_m->n_outputs = 0;
  *n_modules += 1;
  return new_m;
}

void part1_part2() {
  FILE *f = fopen("input", "r");
  char buffer[256];
  char registered_module_names[64][4];
  size_t n_registered_module_names = 0;
  module_t modules[64];
  size_t n_modules = 0;
  size_t broadcaster_idx = 0;
  while (fgets(buffer, 256, f)) {
    char *line = buffer;
    char type;
    if (*line == '%') {
      type = '%';
      ++line;
    } else if (*line == '&') {
      type = '&';
      ++line;
    } else {
      type = 'b';
    }

    char name[16];
    memset(name, 0, 16);
    for (char *n = name; *line != ' '; ++line, ++n) {
      *n = *line;
    }
    module_t *m = register_module(name, modules, &n_modules);
    assert(m != 0);
    assert(m->type == '?');
    m->type = type;
    line += 4;
    while (*line && *line != '\n') {
      memset(name, 0, 16);
      for (char *n = name; *line != ',' && *line != '\n'; ++line, ++n) {
        *n = *line;
      }
      module_t *output_module = register_module(name, modules, &n_modules);
      m->outputs[m->n_outputs++] =
          (module_output_t){.module_idx = output_module->idx,
                            .input_idx = output_module->n_inputs};
      output_module->n_inputs++;

      if (*line == ',')
        line += 2;
    }

    if (m->type == 'b') {
      broadcaster_idx = m->idx;
    }
  }

  module_state_t init_state[64];
  module_state_t next_state[64];
  for (size_t i = 0; i < n_modules; ++i) {
    module_state_t *s = &init_state[i];
    s->n_inputs = modules[i].n_inputs;
    memset(s->recent_pulses, 'l', s->n_inputs);
    s->is_on = 0;
  }
  assert(init_state[broadcaster_idx].n_inputs == 0);
  init_state[broadcaster_idx].n_inputs++;

  size_t button_pushes_counter = 0;
  for (; button_pushes_counter < 1000; ++button_pushes_counter) {
    push_button(modules, n_modules, broadcaster_idx, init_state, next_state);
  }

  printf("part1 product=%lu\n", LOW_COUNTER * HIGHT_COUNTER);
  assert(LOW_COUNTER * HIGHT_COUNTER == 680278040);

  size_t total_button_pushes_needed = 1;
  size_t vf_cycle_counters[4] = {0, 0, 0, 0};
  size_t n_vf_inputs = 4;
  while (n_vf_inputs) {
    button_pushes_counter++;
    int vf_pulse_idx = push_button(modules, n_modules, broadcaster_idx, init_state, next_state);
    if(vf_pulse_idx >= 0 && vf_cycle_counters[vf_pulse_idx] == 0) {
        total_button_pushes_needed *= button_pushes_counter;
        --n_vf_inputs;
        vf_cycle_counters[vf_pulse_idx] = button_pushes_counter;
    }
  }

  printf("part2 total button pushes needed %lu times\n", total_button_pushes_needed);
  assert(total_button_pushes_needed == (size_t)4013 * 4021 * 3889 * 3881);
}

int main() {
  part1_part2();
  return 0;
}
