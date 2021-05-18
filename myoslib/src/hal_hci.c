/**
***************************************************************
* @file myoslib/src/hal_hci.c
* @author Desmond Gan - S4526441
* @date 14042021
* @brief shell log driver
* REFERENCE: 
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* hal_hci_master_init()  - Initialise HCI master
* hal_hci_slave_init() - Initialise HCI slave
* hal_hci_transmit(uint8_t* trsmtMsg) -  Write via SPI
* hal_hci_receive() - Receive from SPI
***************************************************************
*/

#include <ctype.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <version.h>
#include <zephyr.h>
#include "hal_hci.h"

#define SPI_BUFFER 10

uint8_t rxBuffer[SPI_BUFFER];
uint8_t tx_buffer[SPI_BUFFER];
uint8_t tx_buffer_d[SPI_BUFFER];
uint8_t dumb_rxBuf[SPI_BUFFER];

const struct device *spi;
struct spi_config spi_cfg = { 0 };
struct spi_cs_control spi_cs;

struct spi_buf rx_buf = {
	.buf = rxBuffer,
	.len = sizeof(rxBuffer),
};

struct spi_buf rx_buf_d = {
	.buf = dumb_rxBuf,
	.len = sizeof(dumb_rxBuf),
};

struct spi_buf tx_buf = { .buf = tx_buffer, .len = sizeof(tx_buffer) };
struct spi_buf tx_buf_d = { .buf = tx_buffer_d, .len = sizeof(tx_buffer) };

struct spi_buf_set tx = { .buffers = &tx_buf, .count = 1 };
struct spi_buf_set tx_d = { .buffers = &tx_buf_d, .count = 1 };

struct spi_buf_set rx = { .buffers = &rx_buf, .count = 1 };
struct spi_buf_set rx_d = { .buffers = &rx_buf_d, .count = 1 };

/**
 * @brief Initialise HCI master
 * 
 **/
void hal_hci_master_init() {

    spi = device_get_binding("SPI_1");

	if (spi == NULL) {
		return;
	} 

	spi_cfg.operation =
		SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER;
	spi_cfg.frequency = 2000000;
	spi_cfg.slave = 0;

	spi_cs.gpio_dev = device_get_binding("GPIO_0");
	spi_cs.gpio_pin = 31;
	spi_cs.delay = 0;
	spi_cs.gpio_dt_flags = GPIO_ACTIVE_LOW;
	spi_cfg.cs = &spi_cs;
}

/**
 * @brief Initialise HCI slave
 * 
 **/
void hal_hci_slave_init() {

    spi = device_get_binding("SPI_1");

	if (spi == NULL) {
		// printk("Could not find SPI driver\n");
		return;
	}

	spi_cfg.operation =
		SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_SLAVE;
	spi_cfg.frequency = 2000000;
	spi_cfg.slave = 1;

	spi_cs.gpio_dev = device_get_binding("GPIO_0");
	spi_cs.gpio_pin = 15;
	spi_cs.delay = 0;
	spi_cs.gpio_dt_flags = GPIO_ACTIVE_LOW;
	spi_cfg.cs = &spi_cs;
}

/**
 * @brief Write via SPI
 * 
 * @param trsmtMsg: message to transmit
 * 
 **/
void hal_hci_transmit(uint8_t* trsmtMsg) {

    memcpy(tx_buffer, trsmtMsg, (sizeof(uint8_t) * SPI_BUFFER));
	
    if(spi_transceive(spi, &spi_cfg, &tx, &rx_d) == 0) {
		// spi_transceive(spi, &spi_cfg, &tx_d, &rx);
    }   
}

/**
 * @brief Recevie from SPI
 * 
 **/
void hal_hci_receive() {
    if (spi_read(spi, &spi_cfg, &rx) == 0) {
		
	}  
}