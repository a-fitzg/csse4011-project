/**
***************************************************************
* @file myoslib/inc/os_hci.h
* @author Desmond Gan - S4526441
* @date 14032021
* @brief shell log driver
* REFERENCE: 
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* os_hci_transmit(uint8_t sid, size_t argc, char** argv, uint8_t device) - Transmit packet via SPI.
* os_hci_receive() - Receive packet via SPI
* os_hci_reg_process() - Process i2C request  
* os_hci_lsm() - Process lsm6dsl request
* os_transmit_lsm(uint8_t read_register) - Transmit response for I2C request
***************************************************************
*/

#ifndef OS_HCI_H
#define OS_HCI_H

#include <stdint.h>

extern void os_hci_transmit(uint8_t sid, size_t argc, char** argv, uint8_t device);
extern void os_hci_receive();
extern uint8_t os_hci_reg_process();
extern void os_transmit_lsm(uint8_t read_register);
extern struct Packetback os_hci_lsm();
#endif

