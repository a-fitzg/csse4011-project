/**
***************************************************************
* @file myoslib/src/cli_ledrgb.c
* @author Desmond Gan - S4526441
* @date 14032021
* @brief Custom shell commands for led controls.(Turn on, turn off, toggle)
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* cli_led_init - Initialize shell command for leds. Allows sub-commands for turning on and off as well as toggle led.
***************************************************************
* INTERNAL FUNCTIONS
***************************************************************
* cmd_led_off(const struct shell *shell, size_t argc, char **argv) - Shell command to turn off led. Turn off red, green or blue leds only.
* cmd_led_on(const struct shell *shell, size_t argc, char **argv) - Shell command to turn on led. Turn on red, green or blue leds only.
* cmd_led_toggle(const struct shell *shell, size_t argc, char **argv) - Shell command to toggle led. Toggle red, green or blue leds only.
***************************************************************
*/

#include "os_ledrgb.h"
#include "os_log.h"
#include <drivers/gpio.h>
#include <shell/shell.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr.h>
#include <string.h>

/**
 * @brief Shell command to turn off led. Turn off red, green or blue leds only.
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command used to turn off selected leds
 * 
 **/
static int cmd_led_off(const struct shell *shell, size_t argc, char **argv) {
	char logmsg[50];
	if (argc == 2 ) {

		if (strcmp(argv[1], "r") == 0) {

			os_ledRGB_off(0);
			sprintf(logmsg, "Red led is off\r\n");
			os_log_transmit(logmsg, 'l', shell);
		} else if (strcmp(argv[1], "g") == 0) {

			os_ledRGB_off(1);
			sprintf(logmsg, "Green led is off\r\n");
			os_log_transmit(logmsg, 'l', shell);
		} else if (strcmp(argv[1], "b") == 0) {

			os_ledRGB_off(2);
			sprintf(logmsg, "Blue led is off\r\n");
			os_log_transmit(logmsg, 'l', shell);
		} else if ((strcmp(argv[0], "f") == 0) && (strcmp(argv[1], "o") == 0)) {

			sprintf(logmsg, "DEBUG: debugging message\r\n");
			os_log_transmit(logmsg, 'd', shell);
		} else {
			sprintf(logmsg, "Invalid led off command\r\n");
			os_log_transmit(logmsg, 'e', shell);
		}
	} else {

		sprintf(logmsg, "ERROR: Invalid format\r\n");
		os_log_transmit(logmsg, 'e', shell);
	}	
}

/**
 * @brief Shell command to turn on led. Turn on red, green or blue leds only.
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command used to turn on selected leds
 * 
 **/
static int cmd_led_on(const struct shell *shell, size_t argc, char **argv) {
	char logmsg[50];
	if (argc == 2 ) {

		if (strcmp(argv[1], "r") == 0) {

			os_ledRGB_on(0);
			sprintf(logmsg, "Red led is on\r\n");
			os_log_transmit(logmsg, 'l', shell);
		} else if (strcmp(argv[1], "g") == 0) {

			os_ledRGB_on(1);
			sprintf(logmsg, "Green led is on\r\n");
			os_log_transmit(logmsg, 'l', shell);
		} else if (strcmp(argv[1], "b") == 0) {

			os_ledRGB_on(2);
			sprintf(logmsg, "Blue led is on\r\n");
			os_log_transmit(logmsg, 'l', shell);
		} else if ((strcmp(argv[0], "f") == 0) && (strcmp(argv[1], "o") == 0)) {

			sprintf(logmsg, "DEBUG: debugging message\r\n");
			os_log_transmit(logmsg, 'd', shell);
		} else {

			sprintf(logmsg, "ERROR: Invalid led on command\r\n");
			os_log_transmit(logmsg, 'e', shell);
		}
	} else {

		sprintf(logmsg, "ERROR: Invalid format\r\n");
		os_log_transmit(logmsg, 'e', shell);
	}	
}

/**
 * @brief Shell command to toggle led. Toggle red, green or blue leds only.
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command used to toggle selected leds
 * 
 **/
static int cmd_led_toggle(const struct shell *shell, size_t argc, char **argv) {
	char logmsg[50];
	if (argc == 2 ) {

		if (strcmp(argv[1], "r") == 0) {

			os_ledRGB_toggle(0);
			sprintf(logmsg, "Red led is toggled\r\n");
			os_log_transmit(logmsg, 'l', shell);
		} else if (strcmp(argv[1], "g") == 0) {

			os_ledRGB_toggle(1);
			sprintf(logmsg, "Green led is toggled\r\n");
			os_log_transmit(logmsg, 'l', shell);
		} else if (strcmp(argv[1], "b") == 0) {
            
			os_ledRGB_toggle(2);
			sprintf(logmsg, "Blue led is toggled\r\n");
			os_log_transmit(logmsg, 'l', shell);
		} else if ((strcmp(argv[0], "f") == 0) && (strcmp(argv[1], "o") == 0)) {

			sprintf(logmsg, "DEBUG: debugging message\r\n");
			os_log_transmit(logmsg, 'd', shell);
		}
		else {

			sprintf(logmsg, "ERROR: Invalid led toggle command\r\n");
			os_log_transmit(logmsg, 'e', shell);
		}
	} else {

		sprintf(logmsg, "ERROR: Invalid format\r\n");
		os_log_transmit(logmsg, 'e', shell);
	}	
}

/**
 * @brief Initialize shell command for leds. Allows sub-commands for turning on and off as well as toggle led.
 **/
void cli_ledrgb_init() {
    SHELL_STATIC_SUBCMD_SET_CREATE(sub_led,
		SHELL_CMD_ARG(o, NULL, "Led On", cmd_led_on, 2, 0),
		SHELL_CMD_ARG(f, NULL, "Led Off", cmd_led_off, 2, 0),
		SHELL_CMD_ARG(t, NULL, "Led Toggle", cmd_led_toggle, 2, 0),
		SHELL_SUBCMD_SET_END);
SHELL_CMD_REGISTER(led, &sub_led, "LED controls", NULL);
}
