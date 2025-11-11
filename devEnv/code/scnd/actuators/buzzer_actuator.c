#include "actuator.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char label[ACTUATOR_LABEL_MAX];
    bool is_on;
} buzzer_state_t;

static buzzer_state_t *state_from(const actuator_t *act) {
    return (buzzer_state_t *)act->params;
}

static void buzzer_activate(actuator_t *act) {
    buzzer_state_t *state = state_from(act);
    if (!state) {
        return;
    }
    state->is_on = true;
}

static void buzzer_deactivate(actuator_t *act) {
    buzzer_state_t *state = state_from(act);
    if (!state) {
        return;
    }
    state->is_on = false;
}

static bool buzzer_status(const actuator_t *act) {
    const buzzer_state_t *state = state_from(act);
    return state ? state->is_on : false;
}

actuator_t buzzer_actuator_create(const char *label) {
    actuator_t act = {0};
    buzzer_state_t *state = calloc(1, sizeof(*state));
    if (!state) {
        fprintf(stderr, "buzzer_actuator: out of memory\n");
        return act;
    }

    const char *default_label = label ? label : "BUZZER";
    strncpy(state->label, default_label, sizeof(state->label) - 1U);
    state->label[sizeof(state->label) - 1U] = '\0';

    act.params = state;
    act.activate = buzzer_activate;
    act.deactivate = buzzer_deactivate;
    act.status = buzzer_status;
    return act;
}

void buzzer_actuator_destroy(actuator_t *act) {
    if (!act || !act->params) {
        return;
    }
    free(act->params);
    act->params = NULL;
    act->activate = NULL;
    act->deactivate = NULL;
    act->status = NULL;
}
