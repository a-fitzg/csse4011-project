/**
***************************************************************
* @file myoslib/src/cli_hci.c
* @author Desmond Gan - S4526441
* @date 14032021
* @brief Custom shell commands for HCI
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* cli_hci_init - Initialize shell command for i2creg and lsm6dsl. Allows sub-commands for i2creg and lsm6dsl commands.
***************************************************************
* INTERNAL FUNCTIONS
***************************************************************
* process_lsm(size_t argc, char **argv) - Transmit and display LSM6DSL sensor data
* cmd_i2c_read(const struct shell *shell, size_t argc, char **argv) - Shell command to read i2c selected register
* cmd_i2c_write(const struct shell *shell, size_t argc, char **argv) - Shell command to write i2c selected register
* cmd_lsm_read(const struct shell *shell, size_t argc, char **argv) - Shell command to read LSM6DSL sensor data 
***************************************************************
*/

#include <drivers/gpio.h>
#include <shell/shell.h>
#include <sys/printk.h>
#include <lsm6dsl/lsm6dsl.h>
#include <shell/shell_uart.h>
#include <stdio.h>
#include <stdlib.h>
#include <zephyr.h>
#include <string.h>
#include "os_hci.h"
#include "hal_packet.h"
#include "hal_hci.h"

/**
 * @brief Transmit and display LSM6DSL sensor data
 * 
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command used to read different axis value
 * 
 **/
void process_lsm(size_t argc, char **argv) {
    struct Packetback x_data, y_data, z_data;
    int16_t temp_x,temp_y,temp_z;
    double xval, yval, zval;
    if (strcmp(argv[1], "x") == 0) {

        os_transmit_lsm(1);
        k_msleep(500);
        // x_data = os_hci_lsm();
        uint8_t unparsed_msg[SPI_BUFFER];
        hal_hci_receive();
        memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER);
        temp_x = ( unparsed_msg[3] << 8 | unparsed_msg[4] );
        xval = (double)(temp_x) * (61LL / 1000.0) * SENSOR_G_DOUBLE / 1000;
        shell_print(shell_backend_uart_get_ptr(), "X-axis: %lf", xval);

    } else if (strcmp(argv[1], "y") == 0) {

        os_transmit_lsm(2);
        k_msleep(500);
        // x_data = os_hci_lsm();
        uint8_t unparsed_msg[SPI_BUFFER];
        hal_hci_receive();
        memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER);
        temp_y = ( unparsed_msg[3] << 8 | unparsed_msg[4] );
        yval = (double)(temp_y) * (61LL / 1000.0) * SENSOR_G_DOUBLE / 1000;
        shell_print(shell_backend_uart_get_ptr(), "Y-axis: %lf", yval);
    }  else if (strcmp(argv[1], "z") == 0) {

        os_transmit_lsm(3);
        k_msleep(500);
        uint8_t unparsed_msg[SPI_BUFFER];
        hal_hci_receive();
        memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER);
        temp_z = ( unparsed_msg[3] << 8 | unparsed_msg[4] );
        zval = (double)(temp_z) * (61LL / 1000.0) * SENSOR_G_DOUBLE / 1000;
        shell_print(shell_backend_uart_get_ptr(), "Z-axis: %lf", zval);
    } else if (strcmp(argv[1], "a") == 0) {
        os_transmit_lsm(1);
        k_msleep(200);
        uint8_t unparsed_msg[SPI_BUFFER];
        hal_hci_receive();
        memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER);
        temp_x = ( unparsed_msg[3] << 8 | unparsed_msg[4] );
        xval = (double)(temp_x) * (61LL / 1000.0) * SENSOR_G_DOUBLE / 1000;
        shell_print(shell_backend_uart_get_ptr(), "X-axis: %lf", xval);

         os_transmit_lsm(2);
        k_msleep(200);
        // x_data = os_hci_lsm();
        hal_hci_receive();
        memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER);
        temp_y = ( unparsed_msg[3] << 8 | unparsed_msg[4] );
        yval = (double)(temp_y) * (61LL / 1000.0) * SENSOR_G_DOUBLE / 1000;
        shell_print(shell_backend_uart_get_ptr(), "Y-axis: %lf", yval);

        os_transmit_lsm(3);
        k_msleep(200);
        hal_hci_receive();
        memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER);
        temp_z = ( unparsed_msg[3] << 8 | unparsed_msg[4] );
        zval = (double)(temp_z) * (61LL / 1000.0) * SENSOR_G_DOUBLE / 1000;
        shell_print(shell_backend_uart_get_ptr(), "Z-axis: %lf", zval);

    }
}


/**
 * @brief Shell command to read i2c selected register
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command used to selected register
 * 
 **/
static int cmd_i2c_read(const struct shell *shell, size_t argc, char **argv) {

    struct Packetback data;
    uint8_t regvalue;
    int sid = atoi(argv[1]);
    os_hci_transmit(sid, argc, argv, 1);
    k_msleep(500);
    regvalue = os_hci_reg_process();
    shell_print(shell, "Register value: %d", regvalue);
}

/**
 * @brief Shell command to write i2c selected register
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command used to write i2c selected register
 * 
 **/
static int cmd_i2c_write(const struct shell *shell, size_t argc, char **argv) {

    int sid = atoi(argv[1]);
    os_hci_transmit(sid, argc, argv, 1);
}

/**
 * @brief Shell command to read LSM6DSL sensor data 
 * 
 * @param shell: Pointer to the shell struct, which time will print on.
 * @param argc: Number of arguments of shell command
 * @param argv: List of string of arguments of shell command used to read LSM6DSL sensor data 
 * 
 **/
static int cmd_lsm_read(const struct shell *shell, size_t argc, char **argv) {
    
    process_lsm(argc, argv);
}

/**
 * @brief Initialize shell command for i2creg and lsm6dsl. Allows sub-commands for i2creg and lsm6dsl commands.
 **/
void cli_hci_init() {
    SHELL_STATIC_SUBCMD_SET_CREATE(sub_i2reg,
		SHELL_CMD_ARG(r, NULL, "Read from i2c register", cmd_i2c_read, 3, 0),
		SHELL_CMD_ARG(w, NULL, "Write to i2c register", cmd_i2c_write, 4, 0),
		SHELL_SUBCMD_SET_END);
    SHELL_CMD_REGISTER(i2creg, &sub_i2reg, "Read or write to i2c registers", NULL);
    SHELL_STATIC_SUBCMD_SET_CREATE(sub_lsm,
		SHELL_CMD_ARG(r, NULL, "Read from i2c register", cmd_lsm_read, 2, 0),
		SHELL_SUBCMD_SET_END);
    SHELL_CMD_REGISTER(lsm6dsl, &sub_lsm, "Get data from LSM6DSL sensor.", NULL);
}




