#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <drivers/gpio.h>

#define DEVICE_NAME             CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN         (sizeof(DEVICE_NAME) - 1)

#define NUM_STATIC_NODES        8
#define PAYLOAD_SIZE            16
#define PAYLOAD_BUFFER_OFFSET   13
#define NODE_QUEUE_ALIGNMENT    32

#define BT_QUEUE_LENGTH         32
#define BT_THREAD_STACK_SIZE    500
#define BT_THREAD_PRIORITY      5

#define SLEEP_TIME_MS           100

#define false                   0
#define true                    1

#define BT_LE_FASTER_ADV        BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_IDENTITY, \
                                                BT_GAP_ADV_FAST_INT_MIN_1, \
                                                BT_GAP_ADV_FAST_INT_MAX_1, \
                                                NULL)

// The devicetree node identifier for the "led2" alias.
#define LED2_NODE DT_ALIAS(led2)

#if DT_NODE_HAS_STATUS(LED2_NODE, okay)
#define LED2    DT_GPIO_LABEL(LED2_NODE, gpios)
#define PIN DT_GPIO_PIN(LED2_NODE, gpios)
#define FLAGS   DT_GPIO_FLAGS(LED2_NODE, gpios)
#else
// A build error here means board isn't set up to blink an LED.
#ifdef FAIL_CATASTROPHICALLY
#error "Unsupported board: led2 devicetree alias is not defined"
#endif // FAIL_CATASTROPHICALLY
#define LED2    ""
#define PIN 0
#define FLAGS   0
#endif //DT_NODE_HAS_STATUS


// Properties of a static node
typedef struct {
    bt_addr_t   address;
    uint8_t     hasUltrasonic;
    int8_t      rssi;
    uint8_t     ultrasonic[2];
} StaticNode;

// Static node list item, each with an index and node properties
typedef struct {
    uint8_t     index;
    StaticNode  node;
} NodeListItem;

// Items on the node message queue
typedef struct {
    uint8_t     index;
    int8_t      rssi;
    uint8_t     payload[PAYLOAD_SIZE];
} NodeQueueItem;

// Mutex for accessing list of node properties
extern struct k_mutex os_MutexNodeList;

// List of nodes and their properties
// ##### MUST USE MUTEX (os_MutexNodeList) TO ACCESS THIS LIST #####
extern NodeListItem nodeList[NUM_STATIC_NODES];

// Message queue for incoming bluetooth messages (from static nodes)
extern struct k_msgq os_QueueBtNodeMessage;

// Function prototypes - more detailed top comments in source file
uint8_t os_ledInit(void);
uint8_t addressesEqual(bt_addr_t, bt_addr_t);
void bt_mobileCallback(const bt_addr_le_t*, int8_t, uint8_t,
        struct net_buf_simple*);
void os_bluetooth_staticBeaconInit(int);
void os_bluetooth_mobileBeaconInit(int);
uint8_t os_bluetoothMobileListen(void*);
uint8_t os_bluetoothSocialDistancingDetector(void*);

#endif // BLUETOOTH_H
