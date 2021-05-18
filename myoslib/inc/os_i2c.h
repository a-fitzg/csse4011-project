/**
***************************************************************
* @file myoslib/inc/os_i2c.h
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

#ifndef OS_I2C_H
#define OS_I2C_H

#include <stdlib.h>

extern uint8_t sensors[];
extern const struct device *i2c_dev;

extern int os_i2c_write_bytes(const struct device *i2c_dev, uint16_t addr,
		       uint8_t *data, uint32_t num_bytes, uint8_t sensor_addr);
extern int os_i2c_read_bytes(const struct device *i2c_dev, uint16_t addr,
		      uint8_t *data, uint32_t num_bytes, uint8_t sensor_addr);
extern void os_i2c_init();
extern void os_i2c_queue(uint8_t i2c_addr, uint8_t reg_addr, uint8_t reg_value);
 
#endif
    