/**
 ******************************************************************************
 * @file    apps/p3/mobile
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
#include <zephyr/types.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <logging/log.h>
#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <kernel.h>
#include <drivers/uart.h>
#include <usb/usb_device.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#include <os_bluetooth.h>

#define SLEEP_TIME_MS   100

// The devicetree node identifier for the "led2" alias.                      
#define LED2_NODE DT_ALIAS(led2)                                                


#if DT_NODE_HAS_STATUS(LED2_NODE, okay)
#define LED2    DT_GPIO_LABEL(LED2_NODE, gpios)
#define PIN DT_GPIO_PIN(LED2_NODE, gpios)
#define FLAGS   DT_GPIO_FLAGS(LED2_NODE, gpios)
#else
// A build error here means board isn't set up to blink an L    ED.
#error "Unsupported board: led2 devicetree alias is not defi    ned"
#define LED2    ""
#define PIN 0
#define FLAGS   0
#endif


LOG_MODULE_REGISTER(main);

// Each static node is represented by a struct, and this is a list of them
NodeListItem nodeList[NUM_STATIC_NODES];

// Mutex, to ensure that only one thread can access the Node items at once
struct k_mutex os_MutexNodeList;

// Initialise message queue (bluetooth messages from static nodes)
K_MSGQ_DEFINE(os_QueueBtNodeMessage, sizeof(NodeQueueItem), 
        BT_QUEUE_LENGTH, NODE_QUEUE_ALIGNMENT);

// Initialise thread for processing incoming messages
K_THREAD_DEFINE(os_TaskBtNodeMessage, BT_THREAD_STACK_SIZE, 
        os_bluetoothMobileListen, NULL, NULL, NULL, BT_THREAD_PRIORITY, 0, 0);

// Initialise thread for detecting social distancing violation
K_THREAD_DEFINE(os_TaskBtDtstanceDetect, BT_THREAD_STACK_SIZE, 
        os_bluetoothSocialDistancingDetector, NULL, NULL, NULL, 
        BT_THREAD_PRIORITY, 0, 0);


static const struct bt_data mobileResponseData[] = {                                   
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),               
}; 


void main(void) {

    os_ledInit();

    // Scan parameters
	struct bt_le_scan_param scanParams = {
		.type       = BT_HCI_LE_SCAN_PASSIVE,
		.options    = BT_LE_SCAN_OPT_NONE,
		.interval   = 0x0005,
		.window     = 0x0005,
	};
	int err;
    int btErr;

#ifdef USB_DEBUG
    device_get_binding(CONFIG_UART_CONSOLE_ON_DEV_NAME);

    if (usb_enable(NULL)) {
        return;
    }
#endif  // USB_DEBUG

    // Initialise node list mutex
    k_mutex_init(&os_MutexNodeList);

    // Initialise known bluetooth static nodes
    // Xander's disco
    NodeListItem node1 = {.index = 0, .node = 
            {.address = {.val = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, 
            .hasUltrasonic = 1, .rssi = -128, .ultrasonic = 0}};
    // Desmond's disco
    NodeListItem node2 = {.index = 1, .node = 
            {.address = {.val = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}, 
            .hasUltrasonic = 1, .rssi = -128, .ultrasonic = 0}};
    // Static 3 dongle
    NodeListItem node3 = {.index = 2, .node = 
            {.address = {.val = {0x8A, 0x14, 0x8F, 0x07, 0xFA, 0xF7}}, 
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};
    // Static 0
    NodeListItem node4 = {.index = 3, .node = 
            {.address = {.val = {0x80, 0x17, 0xF3, 0x5A, 0x73, 0xD8}},
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};
    // Static 1
    NodeListItem node5 = {.index = 4, .node = 
            {.address = {.val = {0x78, 0x8B, 0x23, 0xD3, 0x34, 0xF0}},
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};
    // Aiden Argon
    NodeListItem node6 = {.index = 5, .node = 
            {.address = {.val = {0x68, 0x70, 0x89, 0x63, 0xB1, 0xF4}}, 
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};
    // ------------------------  BEACONS  ----------------------------
    // Beacon 2
    NodeListItem node7 = {.index = 6, .node = 
            {.address = {.val = {0xFE, 0xFF, 0x82, 0x89, 0x1B, 0xCB}}, 
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};
    // Beacon 3
    NodeListItem node8 = {.index = 7, .node = 
            {.address = {.val = {0x60, 0xCE, 0xDB, 0xE0, 0x0C, 0xCA}}, 
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};
    // Beacon 4
    NodeListItem node9 = {.index = 8, .node = 
            {.address = {.val = {0x4A, 0x3E, 0xFA, 0x8D, 0xE0, 0xFD}}, 
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};
    // Beacon 5
    NodeListItem node10 = {.index = 9, .node = 
            {.address = {.val = {0x04, 0x25, 0xFF, 0x57, 0xBD, 0xF9}}, 
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};
    // Beacon 6
    NodeListItem node11 = {.index = 10, .node = 
            {.address = {.val = {0x58, 0xC4, 0x30, 0xDA, 0xEB, 0xEC}}, 
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};
    // Beacon 7
    NodeListItem node12 = {.index = 11, .node = 
            {.address = {.val = {0x13, 0x20, 0x7C, 0xD4, 0x7F, 0xD4}}, 
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};
    // Beacon 8
    NodeListItem node13 = {.index = 12, .node = 
            {.address = {.val = {0x0A, 0x80, 0x5C, 0xBA, 0x59, 0xE6}}, 
            .hasUltrasonic = 0, .rssi = -128, .ultrasonic = 0}};

    nodeList[0] = node1;
    nodeList[1] = node2;
    nodeList[2] = node3;
    nodeList[3] = node4;
    nodeList[4] = node5;
    nodeList[5] = node6;
    nodeList[6] = node7;
    nodeList[7] = node8;
    nodeList[8] = node9;
    nodeList[9] = node10;
    nodeList[10] = node11;
    nodeList[11] = node12;
    nodeList[12] = node13;

    k_sleep(K_MSEC(1000));

	// Initialize the Bluetooth Subsystem
	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	err = bt_le_scan_start(&scanParams, bt_mobileCallback);
	if (err) {
		printk("Starting scanning failed (err %d)\n", err);
		return;
	}

    // We have started scanning, now send off static node values every ~30ms
    while (1) {

        // RSSI values
        int8_t rssi0, rssi1, rssi2, rssi3, rssi4, rssi5, rssi6, rssi7, rssi8, 
               rssi9, rssi10, rssi11, rssi12;
        // Ultrasonic values
        uint8_t us0;
        uint8_t us1;

        // Now get the values
        // Accessing variable shared across multiple threads, need to ensure
        // only 1 thread access the variables at once
        k_mutex_lock(&os_MutexNodeList, K_FOREVER);

        rssi0  = nodeList[0].node.rssi;
        rssi1  = nodeList[1].node.rssi;
        rssi2  = nodeList[2].node.rssi;
        rssi3  = nodeList[3].node.rssi;
        rssi4  = nodeList[4].node.rssi;
        rssi5  = nodeList[5].node.rssi;
        rssi6  = nodeList[6].node.rssi;
        rssi7  = nodeList[7].node.rssi;
        rssi8  = nodeList[8].node.rssi;
        rssi9  = nodeList[9].node.rssi;
        rssi10 = nodeList[10].node.rssi;
        rssi11 = nodeList[11].node.rssi;
        rssi12 = nodeList[12].node.rssi;

        // Define node list items 0 and 1 ultrasonic node
        us0 = nodeList[0].node.ultrasonic;
        us1 = nodeList[1].node.ultrasonic;

        // Leaving critical section
        k_mutex_unlock(&os_MutexNodeList);

        // Now we have the necessary values, send them off to the base node
        btErr = bt_le_adv_stop();

        const struct bt_data tempAd[] = {
                BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
                BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xAA, 0xFE),
                BT_DATA_BYTES(BT_DATA_SVC_DATA16,
                        0xAA, 0xFE,     // Eddystone UUID
                        0x00,           // Eddystone UID frame type
                        0x00,           // Calibrated Tx power at 0m
                        rssi0,  rssi1,  rssi2,  rssi3, 
                        rssi4,  rssi5,  rssi6,  rssi7,
                        rssi8,  rssi9,  rssi10, rssi11,
                        rssi12, us0,
                        us1,    0x00,
                        0x00,   0x00)
        };

#ifdef DEBUG_PRINT
        printk("RSSI:  [%04d] [%04d] [%04d] [%04d]  ###   ", rssi0, rssi1, rssi2, rssi3);
        printk("US  :  [%04d | %04d] [%04d | %04d]  ###   ", us0[0], us0[1], us1[0], us1[1]);
        printk("TIME:  [%04d] [%04d] [%04d] [%04d]  \n", timeConverter.bytes[0], timeConverter.bytes[1], 
                timeConverter.bytes[2], timeConverter.bytes[3]);
#endif  // DEBUG_PRINT

        err = bt_le_adv_start(BT_LE_FASTER_ADV, tempAd, ARRAY_SIZE(tempAd), mobileResponseData, ARRAY_SIZE(mobileResponseData));

        k_sleep(K_MSEC(30));

    } 
}

