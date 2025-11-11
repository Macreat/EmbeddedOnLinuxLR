#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <stdbool.h>

#define ACTUATOR_LABEL_MAX 32U

typedef struct actuator actuator_t;

typedef void (*actuator_action_fn)(actuator_t *act);
typedef bool (*actuator_status_fn)(const actuator_t *act);

struct actuator {
    void *params;
    actuator_action_fn activate;
    actuator_action_fn deactivate;
    actuator_status_fn status;
};

actuator_t led_actuator_create(const char *label);
void led_actuator_destroy(actuator_t *act);

actuator_t buzzer_actuator_create(const char *label);
void buzzer_actuator_destroy(actuator_t *act);

#endif /* ACTUATOR_H */
