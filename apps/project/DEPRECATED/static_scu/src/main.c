/**
 ****************************************************************************** 
 * @file    apps/p3/static_scu
 * @author  Alexander FitzGerald - 45330874
 * @date    27042020
 * @brief   Mobile node source code for prac 3
 ****************************************************************************** 
 * EXTERNAL FUNCTIONS
 ****************************************************************************** 
 *
 ****************************************************************************** 
 */

#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/uart.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <drivers/gpio.h>
#include <sys_clock.h>
#include <sys/util.h>
#include <limits.h>
#include "hal_ultrasonic.h"
#include "hal_hci.h"
#include "hal_packet.h"

void main(void) {

    // Initialise hal peripherals
    hal_hci_slave_init();
    hal_ultrasonic_init();

    uint16_t pulse_reading;
    uint8_t data[2];
    uint8_t unparsed_msg[SPI_BUFFER];
    struct UsPacket u_sensor;
    uint8_t* parsed_msg;

    device_get_binding(CONFIG_UART_CONSOLE_ON_DEV_NAME);
    usb_enable(NULL);


    while(1) {

        struct UsPacket u_sensor;

        // Receive messages
        hal_hci_receive(); 
        memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER );

        // Get ultrasonic sensor values
        u_sensor = hal_unparse_spi(unparsed_msg);

        // Request:
        if (u_sensor.type == 1 && u_sensor.sid == 6) {

            pulse_reading = hal_ultrasonic_read();

            // Most significant byte
            data[0] = pulse_reading >> 8;

            // Least significant byte
            data[1] = pulse_reading & 0xFF;

            // Send reading over HAL
            hal_parse_spi(6, pulse_reading, 2);

            printk("VALUE: %d\n", pulse_reading);
        }
    }
}

