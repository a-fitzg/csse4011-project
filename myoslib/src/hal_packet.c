/**
***************************************************************
* @file myoslib/inc/hal_packet.c
* @author Desmond Gan - S4526441
* @date 14032021
* @brief shell log driver
* REFERENCE: 
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* hal_parse_to_spi(uint8_t sid, size_t argc, char** argv, uint8_t pac_type) - Parse data into packet for HCI request
* hal_parse_from_spi(uint8_t *msg) - Parse data from request packet 
* hal_slave_to_master(uint8_t mode, uint8_t* data) - Parse data into packet for HCI I2C response
* hal_lsmx_slave_to_master(uint8_t mode, uint8_t* data) - Parse data into packet for HCI LSM6DSL response
* hal_master_from_slave(uint8_t *msg) - Parse data from response packet 
***************************************************************
*/

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <shell/shell_uart.h>
#include <shell/shell.h>
#include "hal_packet.h"
#include "os_i2c.h"
#include "hal_hci.h"


/**
 * @brief Parse data into packet for HCI request
 * 
 * @param sid: Sensor ID
 * @param argc: Size of argument
 * @param argv: List of argument
 * @param pac_tpye: Type of packet
 * 
 **/
void hal_parse_to_spi(uint8_t sid, size_t argc, char** argv, uint8_t pac_type) {

    uint8_t msg[8];
        msg[0] = 0xAA;
    // read from register
    if (argc == 3) {
        msg[1] = pac_type;
        msg[2] = 3;
        msg[3] = sid;
        msg[4] = sensors[sid - 1] | (1<<0); // i2c register addr
        msg[5] = atoi(argv[2]); // regaddr to read from
    } else if (argc == 4) { // write to register
        msg[1] = pac_type;
        msg[2] = 4;
        msg[3] = sid;
        msg[4] = sensors[sid - 1]; // i2c register addr
        msg[5] = atoi(argv[2]); // regaddr to write to
        msg[6] = atoi(argv[3]); // regval to write
    } 

    hal_hci_transmit(msg);
}    

/**
 * @brief Parse data from request packet 
 * 
 * @param msg: Request packet data
 * 
 **/
struct Packet hal_parse_from_spi(uint8_t *msg) {
    
    struct Packet i2creg;
    i2creg.preamb = msg[0];
    i2creg.type = msg[1];
    i2creg.length = msg[2];
    i2creg.sid = msg[3];
    i2creg.i2c_addr = msg[4];
    i2creg.reg_addr = msg[5];
    i2creg.reg_value = msg[6];

    return i2creg;
}

/**
 * @brief Parse data into packet for HCI I2C response
 * 
 * @param mode: sensor ID
 * @param data: data to sent for response
 * 
 **/
void hal_slave_to_master(uint8_t mode, uint8_t* data) {

    uint8_t msg[SPI_BUFFER];
    msg[0] = 0xAA;
    msg[1] = 2;
    msg[2] = sizeof(data);
    msg[3] = 1;
    for (int i = 0; i < sizeof(data); i++) {
        msg[4+i] = data[i];
    }
    printf("Sending H:%d  L:%d\r\n", msg[5], msg[4]);
    hal_hci_transmit(msg);
}

/**
 * @brief Parse data into packet for HCI LSM6DSL response
 * 
 * @param mode: sensor ID
 * @param data: data to sent for response
 * 
 **/
void hal_lsmx_slave_to_master(uint8_t mode, uint8_t* data) {

    uint8_t msg[SPI_BUFFER];
    msg[0] = 0xAA;
    msg[1] = 2;
    msg[2] = sizeof(data);
    msg[3] = data[1];
    msg[4] = data[0];
    printf("Sending H:%d  L:%d\r\n", msg[3], msg[4]);
    hal_hci_transmit(msg);
}

/**
 * @brief Parse data from response packet 
 * 
 * @param msg: Response packet data
 * 
 **/
struct Packetback hal_master_from_slave(uint8_t *msg) {
    
    struct Packetback data_back;
    data_back.preamb = msg[0];
    data_back.type = msg[1];
    data_back.length = msg[2];
    data_back.sid = msg[3];
    data_back.d1 = msg[4];
    data_back.d2 = msg[5];
    data_back.d3 = msg[6];
    data_back.d4 = msg[7];
    data_back.d5 = msg[8];
    data_back.d6 = msg[9];

    return data_back;
}

void hal_parse_spi(uint8_t sid, uint16_t data, uint8_t type) {
    
    uint8_t msg[SPI_BUFFER];
    if (sid == 6) {
        msg[0] = 0xAA; // spi preamb
        msg[2] = 3; // sid and data size
        msg[3] = 6;
        if (type == 2) {
            msg[1] = 2;
            msg[4] = data >> 8;// upper 8 bit data
            msg[5] = data & 0xFF; // lower 8 bit data
        } else {
            msg[1] = 1;
        }
    }
    
    hal_hci_transmit(msg);

}


struct UsPacket hal_unparse_spi(uint8_t *msg) {
    struct UsPacket packet;

    if (msg[0] == 0xAA) { // SPI preamb
        packet.preamb = msg[0];
        packet.type = msg[1];
        packet.length = msg[2];
        packet.sid = msg[3];
        packet.data[0] = msg[4];
        packet.data[1] = msg[5];
    }

    return packet;
}

