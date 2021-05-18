#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <devicetree.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <drivers/gpio.h>
#include <sys_clock.h>
#include <sys/util.h>
#include <limits.h>
#include "hal_ultrasonic.h"

#define     US_CONVERT  35
#define     TIMEOUT     50000

#define TRIGGER_NODE DT_ALIAS(trigger)
#if DT_NODE_HAS_STATUS(TRIGGER_NODE, okay)
#define TRIGGER	DT_GPIO_LABEL(TRIGGER_NODE, gpios)
#define TRIGGER_PIN	DT_GPIO_PIN(TRIGGER_NODE, gpios)
#define TRIGGER_FLAGS	DT_GPIO_FLAGS(TRIGGER_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Trigger pin not set up"
#define TRIGGER	""
#define TRIGGER_PIN	0
#define TRIGGER_FLAGS	0
#endif

#define ECHO_NODE DT_ALIAS(echo)
#if DT_NODE_HAS_STATUS(ECHO_NODE, okay)
#define ECHO	DT_GPIO_LABEL(ECHO_NODE, gpios)
#define ECHO_PIN	DT_GPIO_PIN(ECHO_NODE, gpios)
#define ECHO_FLAG	DT_GPIO_FLAGS(ECHO_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Echo pin not set up"
#define ECHO	""
#define ECHO_PIN	0
#define ECHO_FLAG	0
#endif

const struct device *trig;
const struct device *ech;


void hal_ultrasonic_init() {
    trig = device_get_binding(TRIGGER);
    ech = device_get_binding(ECHO);

    gpio_config(trig, TRIGGER_PIN, GPIO_OUTPUT_ACTIVE);
    gpio_config(ech, ECHO_PIN, GPIO_INPUT);
}

uint16_t hal_ultrasonic_read() {
    uint16_t pulseTime;
    double dist;

    gpio_pin_set(trig, TRIGGER_PIN, 1);
    k_usleep(10);
    gpio_pin_set(trig, TRIGGER_PIN, 0);

    while(!(gpio_pin_get(ech, ECHO_PIN))) {
    }
    pulseTime = 0;
    while (gpio_pin_get(ech, ECHO_PIN)) {
      pulseTime++;
      k_usleep(1);
    }

    // dist = (((double) pulseTime) / US_CONVERT);
    return pulseTime;
}