/**
 ******************************************************************************
 * @file    apps/p3/static_ahu
 * @author  Alexander FitzGerald - 45330874
 *          Desmond Gan - 45264410
 * @date    27042020
 * @brief   Static node AHU source code for prac 3
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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <drivers/gpio.h>
#include <sys_clock.h>
#include <sys/util.h>
#include <limits.h>
#include "hal_hci.h"
#include "hal_packet.h"
#include <usb/usb_device.h>
#include <os_bluetooth.h>

// Bluetooth scan response data
static const struct bt_data staticResponseData[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
}; 

void main(void) {
    
	const struct device *usb;
    int btErr;

    // Initialise USB UART for debugging
    usb = device_get_binding(CONFIG_UART_CONSOLE_ON_DEV_NAME);

#ifdef DEBUG_USB
    if (usb == NULL) {
        return;
    }

    if (usb_enable(NULL)) {
        return;
    }
#endif

    btErr = bt_enable(NULL);
    if (btErr) {

        printk("Bluetooth init failed (err: %d)\n", btErr);
    }

    k_msleep(5000);

#ifdef ULTRASONIC_NODE
    hal_hci_master_init();    
    k_msleep(100);  
#endif  // ULTRASONIC_NODE

	while(1) {
#ifdef ULTRASONIC_NODE
        // transmit request , data dont care
        hal_parse_spi(6,0,1);

        // wait for spi response
        k_msleep(30); 
        
        uint8_t unparsed_msg[SPI_BUFFER];
        struct UsPacket u_sensor;
        
        // Receive from HCI
        hal_hci_receive(); 
        memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER);
        u_sensor = hal_unparse_spi(unparsed_msg);

        // Check for SPI preamble and response type, then transfer sensor pulse 
        // time over BT
        if (u_sensor.type == 2 && u_sensor.preamb == 0xAA) { 
            uint16_t test = ((uint16_t)u_sensor.data[0] << 8) + 
                u_sensor.data[1];

            printk("Test: %d\r\n", test);
        }
        

#else
        k_msleep(30);
#endif  // ULTRASONIC_NODE
        // Now do bluetooth transmission
        btErr = bt_le_adv_stop();

        const struct bt_data tempAd[] = {
                BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | 
                            BT_LE_AD_NO_BREDR)),
                BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xAA, 0xFE),                  
                BT_DATA_BYTES(BT_DATA_SVC_DATA16,                               
                        0xAA, 0xFE,     // Eddystone UUID                       
                        0x00,           // Eddystone UID frame type             
                        0x00,           // Calibrated Tx power at 0m            
#ifdef ULTRASONIC_NODE
                        u_sensor.data[0], u_sensor.data[1], 
#else
                        0x00, 0x00,
#endif //ULTRASONIC_NODE
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,                     
                        0x00, 0x00) 

        };

        btErr = bt_le_adv_start(BT_LE_FASTER_ADV, tempAd, ARRAY_SIZE(tempAd), 
                staticResponseData, ARRAY_SIZE(staticResponseData));
        printk("SENT  \n");
    }
}

