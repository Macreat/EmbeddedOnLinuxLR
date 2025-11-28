#include "mq2.h"
#include "adc.h"
#include <math.h>

static int channel;
static double R0 = 10.0;
static double RL = 5.0;

void mq2_init(int adc_ch, double r0)
{
    channel = adc_ch;
    R0 = r0;
}

static double mq2_get_ppm(double ratio, double a, double b)
{
    return pow(10, ((log10(ratio) - b) / a));
}

MQ2_Data mq2_read()
{
    MQ2_Data d;
    int val = adc_read_channel(channel);

    double vout = (val / 1023.0) * 5.0;
    double rs = ((5.0 * RL) / vout) - RL;

    d.rs = rs;
    d.ratio = rs / R0;

    d.ppm_smoke = mq2_get_ppm(d.ratio, -0.42, 1.6);
    d.ppm_lpg   = mq2_get_ppm(d.ratio, -0.47, 1.46);
    d.ppm_methane = mq2_get_ppm(d.ratio, -0.38, 1.62);

    return d;
}
