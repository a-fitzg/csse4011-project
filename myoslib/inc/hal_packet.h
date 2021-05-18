/**
***************************************************************
* @file myoslib/src/hal_packet.c
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

#ifndef HAL_PACKET_H
#define HAL_PACKET_H

#include <stdlib.h>
#include <stdint.h>

struct Packet {
    uint8_t preamb;
    uint8_t type;
    uint8_t length;
    uint8_t sid;
    uint8_t i2c_addr;
    uint8_t reg_addr;
    uint8_t reg_value;
};

struct Packetback {
     uint8_t preamb;
    uint8_t type;
    uint8_t length;
    uint8_t sid;
    uint8_t d1;
    uint8_t d2;
    uint8_t d3;
    uint8_t d4;
    uint8_t d5;
    uint8_t d6;
};

struct UsPacket {
    uint8_t preamb;
    uint8_t type;
    uint8_t length;
    uint8_t sid;
    uint8_t data[2];
};

extern void hal_parse_to_spi(uint8_t sid, size_t argc, char** argv, uint8_t device);
extern struct Packet hal_parse_from_spi(uint8_t *msg);
extern void hal_slave_to_master(uint8_t mode, uint8_t* data);
extern struct Packetback hal_master_from_slave(uint8_t *msg);
extern void hal_lsmx_slave_to_master(uint8_t mode, uint8_t* data);

extern void hal_parse_spi(uint8_t sid, uint16_t data, uint8_t type);
extern struct UsPacket hal_unparse_spi(uint8_t *msg);

#endif