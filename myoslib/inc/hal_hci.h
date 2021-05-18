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


#ifndef HAL_HCI_H
#define HAL_HCI_H

#include <stdlib.h>
#include <stdint.h>

#define SPI_BUFFER 10

extern uint8_t rxBuffer[SPI_BUFFER];
extern uint8_t tx_buffer[SPI_BUFFER];

extern void hal_hci_master_init();
extern void hal_hci_slave_init();
extern void hal_hci_transmit(uint8_t *trsmtMsg);
extern void hal_hci_receive();

#endif