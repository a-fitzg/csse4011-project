/**
***************************************************************
* @file myoslib/src/os_hci.c
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

#include <stdlib.h>
#include <zephyr.h>
#include <string.h>
#include <shell/shell.h>
#include <shell/shell_uart.h>
#include <lsm6dsl/lsm6dsl.h>
#include "hal_hci.h"
#include "hal_packet.h"
#include "os_hci.h"
#include "os_i2c.h"

/**
 * @brief Transmit packet via SPI
 * 
 * @param sid: sensor ID
 * @param argc: size of argv
 * @param argv: list of argv
 * @param device: device in which the packet is sent from
 **/
void os_hci_transmit(uint8_t sid, size_t argc, char** argv, uint8_t device) {

    hal_parse_to_spi(sid, argc, argv, device);
}

/**
 * @brief Receive packet via SPI
 **/
void os_hci_receive() {
    
    uint8_t unparsed_msg[SPI_BUFFER];
    
    hal_hci_receive();
    memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER );

    if (unparsed_msg[2] == 1) {  // check for lsm6dsl request

        if (unparsed_msg[3] == 1) { // x-axis
            int16_t x;
            uint8_t x_data[2];
            double dval;
            os_i2c_read_bytes(i2c_dev, 0x28, &x_data, sizeof(x_data), sensors[0]);
            x = ( x_data[1] << 8 | x_data[0] );
            dval = (double)((x) * (61LL / 1000.0) * SENSOR_G_DOUBLE / 1000); 
            printf("X-axis: %lf\r\n", dval);
            hal_lsmx_slave_to_master(1, &x_data);
        } else if (unparsed_msg[3] == 2) { // y-axis
            int16_t y;
            uint8_t y_data[2];
            double dval;
            os_i2c_read_bytes(i2c_dev, LSM6DSL_REG_OUTY_L_XL, &y_data, sizeof(y_data), sensors[0]);
            y = ( y_data[1] << 8 | y_data[0] );
            dval = (double)((y) * (61LL / 1000.0) * SENSOR_G_DOUBLE / 1000);
            printf("Y-axis: %lf\r\n", dval);
            hal_lsmx_slave_to_master(1, &y_data);
        }  else if (unparsed_msg[3] == 3) { // z-axis
            int16_t z;
            uint8_t z_data[2];
            double dval;
            os_i2c_read_bytes(i2c_dev, LSM6DSL_REG_OUTZ_L_XL, &z_data, sizeof(z_data), sensors[0]);
            z = ( z_data[1] << 8 | z_data[0] );
            dval = (double)((z) * (61LL / 1000.0) * SENSOR_G_DOUBLE / 1000);
            printf("Z-axis: %lf\r\n", dval);
            hal_lsmx_slave_to_master(1, &z_data);
        }
    } else if (unparsed_msg[1] == 1) { // request (master to slave)

        // process in disco
        uint8_t data;
        struct Packet recv;
        recv = hal_parse_from_spi(unparsed_msg);
        if ((recv.i2c_addr & 1) == 0) { // write to i2c reg

            data = recv.reg_value;
            os_i2c_write_bytes(i2c_dev, recv.reg_addr, &data, sizeof(data), (recv.i2c_addr >> 1));
		}else {
            
            os_i2c_read_bytes(i2c_dev, recv.reg_addr, &data, sizeof(data), (recv.i2c_addr >> 1));
            hal_slave_to_master(1, &data);
        }
    }  

    
}

/**
 * @brief Process i2C request  
 **/
uint8_t os_hci_reg_process() {

    struct Packetback recv;
    uint8_t unparsed_msg[SPI_BUFFER];
    hal_hci_receive();
    memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER );
    recv = hal_master_from_slave(unparsed_msg);
    return recv.d1;
    // shell_print(shell, "Register value:%d", recv.sid, recv.d1);
}

/**
 * @brief Process lsm6dsl request
 **/
struct Packetback os_hci_lsm() {

    struct Packetback recv;
    uint8_t unparsed_msg[SPI_BUFFER];
    hal_hci_receive();
    memcpy(unparsed_msg, rxBuffer, sizeof(uint8_t) * SPI_BUFFER );
    recv = hal_master_from_slave(unparsed_msg);
    return recv;
    // shell_print(shell, "Register value:%d", recv.sid, recv.d1);

}

/**
 * @brief Transmit response for I2C request
 * 
 * @param read_register: Register value to be sent
 * 
 **/
void os_transmit_lsm(uint8_t read_register) {
    uint8_t msg[SPI_BUFFER];
    msg[0] = 0xAA;
    msg[1] = 1;
    msg[2] = 1;
    msg[3] = read_register;

    hal_hci_transmit(msg);
}


