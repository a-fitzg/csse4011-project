/**
***************************************************************
* @file myoslib/src/ledRGB.h
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
* os_ledRGB_toggle () - turn toggle RGB led
***************************************************************
*/

#ifndef OS_LEDRGB_H
#define OS_LEDRGB_H

extern void os_ledRGB_init();
extern void os_ledRGB_on();
extern void os_ledRGB_off();
extern void os_ledRGB_toggle();

#endif