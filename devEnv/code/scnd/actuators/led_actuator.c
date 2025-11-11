#include "actuator.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char label[ACTUATOR_LABEL_MAX];
    bool is_on;
} led_state_t;

static led_state_t *state_from(const actuator_t *act) {
    return (led_state_t *)act->params;
}

static void led_activate(actuator_t *act) {
    led_state_t *state = state_from(act);
    if (!state) {
        return;
    }
    state->is_on = true;
}

static void led_deactivate(actuator_t *act) {
    led_state_t *state = state_from(act);
    if (!state) {
        return;
    }
    state->is_on = false;
}

static bool led_status(const actuator_t *act) {
    const led_state_t *state = state_from(act);
    return state ? state->is_on : false;
}

actuator_t led_actuator_create(const char *label) {
    actuator_t act = {0};
    led_state_t *state = calloc(1, sizeof(*state));
    if (!state) {
        fprintf(stderr, "led_actuator: out of memory\n");
        return act;
    }

    const char *default_label = label ? label : "LED";
    strncpy(state->label, default_label, sizeof(state->label) - 1U);
    state->label[sizeof(state->label) - 1U] = '\0';

    act.params = state;
    act.activate = led_activate;
    act.deactivate = led_deactivate;
    act.status = led_status;
    return act;
}

void led_actuator_destroy(actuator_t *act) {
    if (!act || !act->params) {
        return;
    }
    free(act->params);
    act->params = NULL;
    act->activate = NULL;
    act->deactivate = NULL;
    act->status = NULL;
}
