#include <math.h>
#include "adc_mcp3008.h"
#include "mq135.h"
#include "../pubsub/pubsub.h"

static int ch_mq135;
static float Ro135;

// Fórmula oficial MQ-135 Winsen
static const float A = 116.6020682;
static const float B = -2.769034857;

void mq135_init(int adc_channel, float ro_value) {
    ch_mq135 = adc_channel;
    Ro135 = ro_value;
}

void mq135_read_publish() {
    int adc = adc_read(ch_mq135);
    float v = adc * (5.0 / 1023.0);

    float Rs = (5.0 - v) * 1000 / v;  // RL=1k
    float ratio = Rs / Ro135;

    MQ135_Data d;
    d.rs_ro = ratio;

    d.ppm = A * pow(ratio, B);

    pubsub_publish("MQ135", &d);
}
