/**
***************************************************************
* @file myoslib/src/cli_log.c
* @author Desmond Gan - S4526441
* @date 14032021
* @brief Custom shell log commands. Allows for message log filtering  
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* cli_time_init() - Intialise shell command line for log filtering
***************************************************************
* INTERNAL FUNCTIONS
***************************************************************
* cmd_error_msg(const struct shell *shell, size_t argc, char **argv) - Only allow error messages on shell
* cmd_log_msg(const struct shell *shell, size_t argc, char **argv) - Only allow log messages on shell
* cmd_debug_msg(const struct shell *shell, size_t argc, char **argv) - Only allow debug messages on shell
* cmd_all_msg(const struct shell *shell, size_t argc, char **argv) - Allow all messages on shell
***************************************************************
*/

#include "cli_log.h"
#include "os_log.h"
#include <zephyr.h>
#include <sys/printk.h>
#include <shell/shell.h>
#include <ctype.h>
#include <string.h>

/**
 * @brief Only allow error messages on shell
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command
 * 
 **/	
static int cmd_error_msg(const struct shell *shell, size_t argc, char **argv) {

    os_log_filter(argv[0][0]);
}

/**
 * @brief Only allow log messages on shell
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command
 * 
 **/	
static int cmd_log_msg(const struct shell *shell, size_t argc, char **argv) {

    os_log_filter(argv[0][0]);
}

/**
 * @brief Only allow debug messages on shell
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command
 * 
 **/	
static int cmd_debug_msg(const struct shell *shell, size_t argc, char **argv) {

    os_log_filter(argv[0][0]);
}

/**
 * @brief Allow all messages on shell
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command
 * 
 **/	
static int cmd_all_msg(const struct shell *shell, size_t argc, char **argv) {

    os_log_filter(argv[0][0]);
}

/**
 * @brief Intialise shell command line for log filtering
 **/	
void cli_log_init() {
    SHELL_STATIC_SUBCMD_SET_CREATE(sub_log,
		SHELL_CMD_ARG(e, NULL, "Error messages only", cmd_error_msg, 1, 0),
		SHELL_CMD_ARG(l, NULL, "Log messages only", cmd_log_msg, 1, 0),
		SHELL_CMD_ARG(d, NULL, "Debug messages only", cmd_debug_msg, 1, 0),
        SHELL_CMD_ARG(a, NULL, "All messages only", cmd_all_msg, 1, 0),
		SHELL_SUBCMD_SET_END);
    SHELL_CMD_REGISTER(log, &sub_log, "Filter log messages.", NULL);
}
