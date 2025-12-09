#ifndef ACTUATOR_H
#define ACTUATOR_H

// generic actuator interface

typedef struct
{
    void (*activate)(void);
    void (*deactivate)(void);
    int (*status)(void); // 1=ON , 0=OFF
} Actuator;

#endif // ACTUATOR_H