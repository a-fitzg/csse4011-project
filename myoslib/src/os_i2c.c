/**
***************************************************************
* @file myoslib/src/os_i2c.c
* @author Desmond Gan - S4526441
* @date 14032021
* @brief shell log driver
* REFERENCE: 
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* os_i2c_write_bytes(const struct device *i2c_dev, uint16_t addr,
		       uint8_t *data, uint32_t num_bytes, uint8_t sensor_addr) - Write byte to i2c register.
* os_i2c_read_bytes(const struct device *i2c_dev, uint16_t addr,
		      uint8_t *data, uint32_t num_bytes, uint8_t sensor_addr) - Read byte to i2c register.
* os_i2c_queue() - Send i2c request to a queue
* access_i2c() - Receive from queue
***************************************************************
*/

#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/util.h>
#include <kernel.h>
#include <sys/__assert.h>
#include <sys/printk.h>
#include <lsm6dsl/lsm6dsl.h>
#include <string.h>
#include <drivers/i2c.h>
#include "os_i2c.h"
#include "hal_packet.h"
#include "os_hci.h"


#define LSM_I2C_ADDR 0b11010100
#define HTS221_I2C_ADDR 0b10111110
#define LIS3MDL_I2C_ADDR 0b00111100 
#define LPS22HB_I2C_ADDR 0b10111010
#define VL53L0X_12C_ADDR 0b01010010

uint8_t sensors[] = {LSM_I2C_ADDR, LIS3MDL_I2C_ADDR, LPS22HB_I2C_ADDR, VL53L0X_12C_ADDR, HTS221_I2C_ADDR};

const struct device *i2c_dev;

struct I2cData {
	void *fifo_reserved;
	uint8_t i2c_addr;
	uint8_t reg_addr;
	uint8_t reg_value;
} I2cData;

/* size of stack area used by each thread */
#define STACKSIZE 1024
/* scheduling priority used by each thread */
#define PRIORITY 7

K_FIFO_DEFINE(queuefifo); // Initialise the queue

/**
 * @brief Write byte to i2c register
 * 
 * @param i2c_dev: Pointer to i2c device struct
 * @param addr: register address to be written to
 * @param data: register value to write
 * @param sensor_addr: I2C Address    
 **/
int os_i2c_write_bytes(const struct device *i2c_dev, uint16_t addr,
		       uint8_t *data, uint32_t num_bytes, uint8_t sensor_addr)
{
	struct i2c_msg msgs[2];

	/* Setup I2C messages */

	/* Send the address to write to */
	msgs[0].buf = &addr;
	msgs[0].len = 1U;
	msgs[0].flags = I2C_MSG_WRITE;

	/* Data to be written, and STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_WRITE | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], 2, sensor_addr >> 1);
}

/**
 * @brief Read byte from i2c register
 * 
 * @param i2c_dev: Pointer to i2c device struct
 * @param addr: register address to be written to
 * @param data: register value to write
 * @param sensor_addr: I2C Address    
 **/
int os_i2c_read_bytes(const struct device *i2c_dev, uint16_t addr,
		      uint8_t *data, uint32_t num_bytes, uint8_t sensor_addr)
{
	struct i2c_msg msgs[2];
    
	/* Setup I2C messages */
	/* Send the address to read from */
	msgs[0].buf = &addr;
	msgs[0].len = 1U;
	msgs[0].flags = I2C_MSG_WRITE;

	/* Read from device. STOP after this. */
	msgs[1].buf = data;
	msgs[1].len = num_bytes;
	msgs[1].flags = I2C_MSG_RESTART | I2C_MSG_READ | I2C_MSG_STOP;

	return i2c_transfer(i2c_dev, &msgs[0], 2, sensor_addr >> 1);
}

/**
 * @brief Send i2c request to a queue
 * 
 * @param i2c_addr: I2C Address 
 * @param reg_addr: register address 
 * @param reg_value: register value 
 **/
void os_i2c_queue(uint8_t i2c_addr, uint8_t reg_addr, uint8_t reg_value) {

	struct I2cData tx_data = {
		.i2c_addr = i2c_addr,
		.reg_addr = reg_addr,
		.reg_value = reg_value
	};
	
	size_t size = sizeof(struct I2cData);
    char *mem_ptr = k_malloc(size);
    __ASSERT_NO_MSG(mem_ptr != 0);
    memcpy(mem_ptr, &tx_data, size);
    k_fifo_put(&queuefifo, mem_ptr);
    k_msleep(100);  // allow time for data transfer to the queue
}

/**
 * @brief Receive from queue
 * 
 **/
void access_i2c() {

	struct I2cData *recv = k_fifo_get(&queuefifo, K_FOREVER);
	while (1) {
		if ((recv->i2c_addr & 1) == 0) { // write to i2c reg
		os_i2c_write_bytes(i2c_dev, recv->reg_addr, &(recv->reg_value), sizeof(recv->reg_value), (recv->i2c_addr >> 1));
		k_free(recv);
		} else {
		uint8_t data;
		os_i2c_read_bytes(i2c_dev, recv->reg_addr, &data, sizeof(data), (recv->i2c_addr >> 1));
		k_free(recv);
		}
	}

}

/**
 * @brief Initialise I2C connection
 * 
 **/
void os_i2c_init() {
    uint8_t wdata;
    i2c_dev = device_get_binding("I2C_2");
    if (i2c_dev == NULL) {
		printk("Could not get I2C device\n");
		return;
	}
    wdata = 0b01000000;
    os_i2c_write_bytes(i2c_dev, LSM6DSL_REG_CTRL1_XL, &wdata, sizeof(wdata), sensors[0]);
}

K_THREAD_DEFINE(access_i2c_id, STACKSIZE, access_i2c, NULL, NULL, NULL, PRIORITY, 0, 0); // Create a log task.