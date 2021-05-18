/**
***************************************************************
* @file myoslib/src/cli_time.c
* @author Desmond Gan - S4526441
* @date 14032021
* @brief CLI system time 
* REFERENCE: 
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* cli_time_init() - Intialise shell command line for system time display
***************************************************************
*/

#include "cli_time.h"
#include <zephyr.h>
#include <sys/printk.h>
#include <shell/shell.h>
#include <ctype.h>
#include <string.h>

struct Time {
	int sec;
	int min;
	int hr;
};

/**
 * @brief Print time on shell when given the correct command. There are two modes of time:
 * 		  time in seconds  
 * 		  time in hours,minutes and seconds
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command
 * 
 **/		
static int show_time(const struct shell *shell, size_t argc, char **argv) {
	
	struct Time clock;

	if (argc == 1) {

		long time_sec;
		time_sec = k_uptime_get() / 1000;
		shell_print(shell, "Time: %lds", time_sec);
	} else if (argc == 2 && strcmp(argv[1],"f") == 0) {
		
		int temp_clock = k_uptime_get() / 1000;
		clock.sec = temp_clock % 60;
		clock.min = (temp_clock / 60) % 60;
		clock.hr = temp_clock / 3600; 
		shell_print(shell, "Time: %d:%d:%d", clock.hr, clock.min, clock.sec);
	} else {

		shell_print(shell, "Invalid time cmd.");
	}
}

/**
 * @brief  Intialise shell command line for system time display.
 * */
void cli_time_init() {
	SHELL_CMD_ARG_REGISTER(time, NULL, "Show time", show_time, 1, 1);
}

