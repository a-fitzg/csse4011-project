/**
***************************************************************
* @file myoslib/src/os_log.h
* @author Desmond Gan - S4526441
* @date 14032021
* @brief shell log driver
* REFERENCE: 
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* os_log_transmit () - Send message from shell to a queue.
* os_log_filter () - Select the flag for the message filter
***************************************************************
*/

#ifndef OS_LOG_H
#define OS_LOG_H

#include <device.h>
#include <drivers/gpio.h>
#include <shell/shell.h>
#include <string.h>
#include <sys/__assert.h>
#include <sys/printk.h>
#include <zephyr.h>

extern void os_log_transmit(char *msg, char type, const struct shell *shell);
extern void os_log_filter(char filter);

#endif