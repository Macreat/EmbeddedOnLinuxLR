#include "adc.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

static const char *SPI_DEV = "/dev/spidev0.0";
static int spi_fd;

void adc_init() {
    spi_fd = open(SPI_DEV, O_RDWR);
    if (spi_fd < 0) {
        perror("SPI open failed");
    }

    uint8_t mode = SPI_MODE_0;
    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);

    uint32_t speed = 1000000;
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
}

int adc_read_channel(uint8_t channel)
{
    uint8_t tx[] = {1, (8 + channel) << 4, 0};
    uint8_t rx[3] = {0};

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 3,
        .speed_hz = 1000000,
        .bits_per_word = 8,
    };

    ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);

    int val = ((rx[1] & 3) << 8) + rx[2];
    return val;
}
