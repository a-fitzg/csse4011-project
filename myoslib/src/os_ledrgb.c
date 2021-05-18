/**
***************************************************************
* @file myoslib/src/os_ledRGB.c
* @author Desmond Gan - S4526441
* @date 14032021
* @brief OnBoard RGB peripheral driver
* REFERENCE: https://docs.zephyrproject.org/latest/reference/peripherals/gpio.html
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* os_ledRGB_init () - intialise LED bar
* os_ledRGB_on () - turn on RGB led
* os_ledRGB_off () - turn off RGB led
* os_ledRGB_toggle () - Toggle RGB led
***************************************************************
*/

#include "os_ledrgb.h"
#include <devicetree.h>
#include <drivers/gpio.h>
#include <zephyr.h>
#include <shell/shell.h>

#define LED1_NODE DT_ALIAS(led1)

#if DT_NODE_HAS_STATUS(LED1_NODE, okay)
#define LED1	DT_GPIO_LABEL(LED1_NODE, gpios)
#define PIN1	DT_GPIO_PIN(LED1_NODE, gpios)
#define FLAGS1	DT_GPIO_FLAGS(LED1_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led1 devicetree alias is not defined"
#define LED1	""
#define PIN1	0
#define FLAGS1	0
#endif

#define LED2_NODE DT_ALIAS(led2)
#if DT_NODE_HAS_STATUS(LED2_NODE, okay)
#define LED2	DT_GPIO_LABEL(LED2_NODE, gpios)
#define PIN2	DT_GPIO_PIN(LED2_NODE, gpios)
#define FLAGS2	DT_GPIO_FLAGS(LED2_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led2 devicetree alias is not defined"
#define LED2	""
#define PIN2	0
#define FLAGS2	0
#endif

#define LED3_NODE DT_ALIAS(led3)
#if DT_NODE_HAS_STATUS(LED3_NODE, okay)
#define LED3	DT_GPIO_LABEL(LED3_NODE, gpios)
#define PIN3	DT_GPIO_PIN(LED3_NODE, gpios)
#define FLAGS3	DT_GPIO_FLAGS(LED3_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led3 devicetree alias is not defined"
#define LED3	""
#define PIN3	0
#define FLAGS3	0
#endif

const struct device *red_led;
const struct device *green_led;
const struct device *blue_led;
int is_red_on;
int is_blue_on;
int is_green_on;

/**
 * Intialise onboard RGB led
 * 
 **/
void os_ledRGB_init() {

	red_led = device_get_binding(LED1);
	green_led = device_get_binding(LED2);
	blue_led = device_get_binding(LED3);
	gpio_pin_configure(red_led, PIN1, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure(green_led, PIN2, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure(blue_led, PIN3, GPIO_OUTPUT_ACTIVE);
	gpio_pin_set(red_led, PIN1, 1);
	gpio_pin_set(blue_led, PIN3, 1);
	gpio_pin_set(green_led, PIN2, 1);
	is_red_on = 1;
	is_blue_on = 1;
	is_green_on = 1;
}

/**
 * Turn on onboard RGB led
 * Parameter: (int) colour of led
 **/
void os_ledRGB_on(int color) {

    switch (color) {
        case 0:
            is_red_on = 0;
			gpio_pin_set(red_led, PIN1, is_red_on);
            break;
        case 1:
            is_green_on = 0;
            gpio_pin_set(green_led, PIN2, is_green_on);
            break;
        case 2:
            is_blue_on = 0;
            gpio_pin_set(blue_led, PIN3, is_blue_on);
            break;
        default:
            break;
    }
}

/**
 * Turn off onboard RGB led
 * Parameter: (int) colour of led
 **/
void os_ledRGB_off(int color) {

    switch (color) {
        case 0:
            is_red_on = 1;
			gpio_pin_set(red_led, PIN1, is_red_on);
            break;
        case 1:
            is_green_on = 1;
            gpio_pin_set(green_led, PIN2, is_green_on);
            break;
        case 2:
            is_blue_on = 1;
            gpio_pin_set(blue_led, PIN3, is_blue_on);
            break;
        default:
            break;
    }
}

/**
 * Toggle onboard RGB led
 * Parameter: (int) colour of led
 **/
void os_ledRGB_toggle(int color) {

    switch (color) {
        case 0:
            if (is_red_on != 1) {
				is_red_on = 1;
			} else {
				is_red_on = 0;
			}
			gpio_pin_set(red_led, PIN1, is_red_on);
            break;
        case 1:
            if (is_green_on != 1) {
				is_green_on = 1;
			} else {
				is_green_on = 0;
			}
            gpio_pin_set(green_led, PIN2, is_green_on);
            break;
        case 2:
            if (is_blue_on != 1) {
				is_blue_on = 1;
			} else {
				is_blue_on = 0;
			}
            gpio_pin_set(blue_led, PIN3, is_blue_on);
            break;
        default:
            break;
    }
}

