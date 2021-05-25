/**
 ****************************************************************************** 
 * @file    apps/p3/base
 * @author  Alexander FitzGerald - 45330874
 * @date    27042020
 * @brief   Base node source code for prac 3
 ****************************************************************************** 
 * EXTERNAL FUNCTIONS
 ****************************************************************************** 
 *
 ****************************************************************************** 
 */

#include <zephyr.h>
#include <zephyr/types.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <kernel.h>
#include <drivers/uart.h>
#include <usb/usb_device.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#include <os_bluetooth.h>

#define FILTER_MOBILE
//#define FILTER_STATIC2
#define FILTER_DONGLE


// Bluetooth address of mobile node
bt_addr_t mobileAddrMobile  = {.val = {0xAF, 0xDE, 0xCD, 0xD4, 0x38, 0xE1}};
bt_addr_t mobileAddrStatic2 = {.val = {0x4D, 0x5F, 0x62, 0xD7, 0x95, 0xCF}};
bt_addr_t mobileAddrDongle  = {.val = {0x1A, 0xDA, 0x64, 0xAA, 0x6C, 0xDC}};

//bt_addr_t mobileAddrSecondary = {.val = {0xB7, 0x14, 0x80, 0xB8, 0xB9, 0xEF}};

/**
 * @brief   Callback for bluetooth scan
 * @param   addr:       Bluetooth address of scan result
 * @param   rssi:       RSSI strength of response
 * @param   adv_type:   Type of data sent
 * @param   vif:        Data buffer
 */
static void staticCallback(const bt_addr_le_t* addr, int8_t rssi, 
        uint8_t adv_type, struct net_buf_simple* buf) {

#ifdef FILTER_MOBILE
    if (addressesEqual(addr->a, mobileAddrMobile)) {

        // We have a message from our mobile node...
        int8_t payload[PAYLOAD_SIZE];
        for (uint8_t i = 0; i < PAYLOAD_SIZE; i++) {

            payload[i] = buf->data[i + PAYLOAD_BUFFER_OFFSET];
        }

        // Make a message with the packet information, then send it over UART
        char message[80];
        sprintf(message, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", 
                payload[0],   payload[1],   payload[2],   payload[3], 
                payload[4],   payload[5],   payload[6],   payload[7],
                payload[8],   payload[9],   payload[10],  payload[11],
                payload[12],  payload[13],  payload[14],  0x00);
        printf("%s", message);
    }
#endif
#ifdef FILTER_STATIC2
    if (addressesEqual(addr->a, mobileAddrStatic2)) {

        // We have a message from our mobile node...
        int8_t payload[PAYLOAD_SIZE];
        for (uint8_t i = 0; i < PAYLOAD_SIZE; i++) {

            payload[i] = buf->data[i + PAYLOAD_BUFFER_OFFSET];
        }

        // Make a message with the packet information, then send it over UART
        char message[80];
        sprintf(message, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", 
                payload[0],   payload[1],   payload[2],   payload[3], 
                payload[4],   payload[5],   payload[6],   payload[7],
                payload[8],   payload[9],   payload[10],  payload[11],
                payload[12],  payload[13],  payload[14],  0x01);
        printf("%s", message);
    }
#endif
#ifdef FILTER_DONGLE
    if (addressesEqual(addr->a, mobileAddrDongle)) {

        // We have a message from our mobile node...
        int8_t payload[PAYLOAD_SIZE];
        for (uint8_t i = 0; i < PAYLOAD_SIZE; i++) {

            payload[i] = buf->data[i + PAYLOAD_BUFFER_OFFSET];
        }

        // Make a message with the packet information, then send it over UART
        char message[80];
        sprintf(message, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", 
                payload[0],   payload[1],   payload[2],   payload[3], 
                payload[4],   payload[5],   payload[6],   payload[7],
                payload[8],   payload[9],   payload[10],  payload[11],
                payload[12],  payload[13],  payload[14],  0x02);
        printf("%s", message);
    }
#endif

/*
    if (addressesEqual(addr->a, mobileAddrSecondary)) {

        // We have a message from the other mobile node...
        uint8_t payload[PAYLOAD_SIZE];
        for (uint8_t i = 0; i < PAYLOAD_SIZE; i++) {

            payload[i] = buf->data[i + PAYLOAD_BUFFER_OFFSET - 4];
        }

        int8_t rssiList[] = {payload[8], payload[9], payload[10], payload[11],
                             payload[4], payload[5], payload[6],  payload[7]};

        char message[80];
        sprintf(message, "%i,%i,%i,%i,%i,%i,%i,%i,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                rssiList[4],  rssiList[5],  rssiList[6],  rssiList[7], 
                rssiList[0],  rssiList[1],  rssiList[2],  rssiList[3],
                payload[12],  payload[13],  payload[14],  payload[15],
                payload[8],   payload[9],   payload[10],  payload[11],
                1);
        printf("%s", message);

    }
*/

}

void main(void) {

    // Set up parameters for scan
	struct bt_le_scan_param scanParams = {
		.type       = BT_HCI_LE_SCAN_PASSIVE,
		.options    = BT_LE_SCAN_OPT_NONE,
		.interval   = 0x0005,
		.window     = 0x0005,
	};
	int err;

    // Bind USB UART
    device_get_binding(CONFIG_UART_CONSOLE_ON_DEV_NAME);

    if (usb_enable(NULL)) {

        return;
    }

    k_sleep(K_MSEC(5000));

	// Initialize the Bluetooth Subsystem
	err = bt_enable(NULL);
	if (err) {

		return;
	}

	err = bt_le_scan_start(&scanParams, staticCallback);
	if (err) {

		return;
	}

    // We have started scanning, now sit and do nothing forever :)
    while (1) {

        k_sleep(K_MSEC(1));

    }
}
