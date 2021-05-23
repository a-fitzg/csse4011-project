/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <drivers/gpio.h>
#include <sys_clock.h>
#include <limits.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define BT_LE_FASTER_ADV    BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_IDENTITY, \
                                            BT_GAP_ADV_FAST_INT_MIN_1, \
						                    BT_GAP_ADV_FAST_INT_MAX_1, \
						                    NULL)
#ifdef ULTRASONIC_ID

#define     US_CONVERT  35                                                      
#define     TIMEOUT     50000                                                   
                                                                                
#define TRIGGER_NODE DT_ALIAS(trigger)                                          
#if DT_NODE_HAS_STATUS(TRIGGER_NODE, okay)                                      
#define TRIGGER DT_GPIO_LABEL(TRIGGER_NODE, gpios)                              
#define TRIGGER_PIN DT_GPIO_PIN(TRIGGER_NODE, gpios)                            
#define TRIGGER_FLAGS   DT_GPIO_FLAGS(TRIGGER_NODE, gpios)                      
#else                                                                           
/* A build error here means your board isn't set up to blink an LED. */         
#ifdef FAIL_CATASTROPHICALLY
#error "Trigger pin not set up"                                                 
#endif // FAIL_CATASTROPHICALLY
#define TRIGGER ""                                                              
#define TRIGGER_PIN 0                                                           
#define TRIGGER_FLAGS   0                                                       
#endif                                                                          
                                                                                
#define ECHO_NODE DT_ALIAS(echo)                                                
#if DT_NODE_HAS_STATUS(ECHO_NODE, okay)                                         
#define ECHO    DT_GPIO_LABEL(ECHO_NODE, gpios)                                 
#define ECHO_PIN    DT_GPIO_PIN(ECHO_NODE, gpios)                               
#define ECHO_FLAG   DT_GPIO_FLAGS(ECHO_NODE, gpios)                             
#else                                                                           
/* A build error here means your board isn't set up to blink an LED. */         
#ifdef FAIL_CATASTROPHICALLY
#error "Echo pin not set up"                                                    
#endif // FAIL_CATASTROPHICALLY
#define ECHO    ""                                                              
#define ECHO_PIN    0                                                           
#define ECHO_FLAG   0                                                           
#endif

#endif // ULTRASONIC_ID

// Device pointers corresponding to ultrasonic sensor
const struct device *trig;
const struct device *ech;

// Initial advertising structure, will be overriden in cyclic executive
static const struct bt_data new_ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xAA, 0xFE),
    BT_DATA_BYTES(BT_DATA_SVC_DATA16,
            0xAA, 0xFE,     // Eddystone UUID
            0x00,           // Eddystone UID frame type
            0x00,           // Calibrated Tx power at 0m
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00)
};

/* Set Scan Response data */
static const struct bt_data sd[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

static void bt_ready(int err)
{
	char addr_s[BT_ADDR_LE_STR_LEN];
	bt_addr_le_t addr = {0};
	size_t count = 1;

	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	// Start advertising
    // We have to use default parameters for the disco board since it doesn't 
    // support the faster advertising frequency
#ifndef ULTRASONIC_ID
	err = bt_le_adv_start(BT_LE_FASTER_ADV, new_ad, ARRAY_SIZE(new_ad),
	        sd, ARRAY_SIZE(sd));
#else
    err = bt_le_adv_start(BT_LE_ADV_NCONN_IDENTITY, new_ad, ARRAY_SIZE(new_ad),
            sd, ARRAY_SIZE(sd));
#endif  // ULTRASONIC_ID
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}


	/* For connectable advertising you would use
	 * bt_le_oob_get_local().  For non-connectable non-identity
	 * advertising an non-resolvable private address is used;
	 * there is no API to retrieve that.
	 */

	bt_id_get(&addr, &count);
	bt_addr_le_to_str(&addr, addr_s, sizeof(addr_s));

	printk("Beacon started, advertising as %s\n", addr_s);
}

void main(void)
{
	int err;

	printk("Starting Beacon Demo\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}

#ifdef ULTRASONIC_ID
    trig = device_get_binding(TRIGGER);                                           
    ech = device_get_binding(ECHO);                                               
    double dist;                                                                  
    gpio_config(trig, TRIGGER_PIN, GPIO_OUTPUT_ACTIVE);                           
    gpio_config(ech, ECHO_PIN, GPIO_INPUT);                                       
    uint16_t pulseTime;                                                           
    gpio_pin_set(trig, TRIGGER_PIN, 0);                                           
    k_usleep(5);
#endif // ULTRASONIC_ID

    while (1) {

#ifdef ULTRASONIC_ID

    gpio_pin_set(trig, TRIGGER_PIN, 1);                                         
    k_usleep(10);                                                               
    gpio_pin_set(trig, TRIGGER_PIN, 0);                                         
                                                                                
    while(!(gpio_pin_get(ech, ECHO_PIN))) {                                     
    }                                                                           
    pulseTime = 0;                                                              
    while (gpio_pin_get(ech, ECHO_PIN)) {                                       
      pulseTime++;                                                              
      k_usleep(1);                                                              
    }                                                                           
    dist = (((double) pulseTime) / US_CONVERT);

#endif // ULTRASONIC_ID

        err = bt_le_adv_stop();

        const struct bt_data temp_ad[] = {
                BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
                BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xAA, 0xFE),    
                BT_DATA_BYTES(BT_DATA_SVC_DATA16,
                        0xAA, 0xFE,     // Eddystone UUID
                        0x00,           // Eddystone UID frame type
                        0x00,           // Calibrated Tx power at 0m
#ifdef ULTRASONIC_ID
                        0xF0, 0xBA, ULTRASONIC_ID, (uint8_t)((pulseTime & 0xFF00) >> 8), (uint8_t)pulseTime & 0xFF, 
#else
                        0x00, 0x00, 0x00, 0x00, 0x00,
#endif  // ULTRASONIC_ID
                        0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00)
        };



#ifndef ULTRASONIC_ID
        err = bt_le_adv_start(BT_LE_FASTER_ADV, temp_ad, ARRAY_SIZE(temp_ad), sd, ARRAY_SIZE(sd));
#else
        err = bt_le_adv_start(BT_LE_ADV_NCONN_IDENTITY, temp_ad, 
                ARRAY_SIZE(temp_ad), sd, ARRAY_SIZE(sd));
#endif  // ULTRASONIC_ID
        k_sleep(K_MSEC(30));
    }
}
