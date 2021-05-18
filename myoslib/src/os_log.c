/**
***************************************************************
* @file myoslib/src/os_ledRGB.c
* @author Desmond Gan - S4526441
* @date 14032021
* @brief OnBoard RGB peripheral driver
* REFERENCE: 
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
* os_log_transmit(char *msg, char type, const struct shell *shell) - Send message from shell to a queue.
* os_log_filter(char filter) - Select the flag for the message filter.
***************************************************************
* INTERNAL FUNCTIONS
***************************************************************
* print_log() - Retrive message from queue and print message onto the shell in their respective colour.
***************************************************************
*/

#include <device.h>
#include <shell/shell.h>
#include <string.h>
#include <sys/__assert.h>
#include <sys/printk.h>
#include <zephyr.h>
#include "os_log.h"

struct Log {
    void *fifo_reserved;
    char *log_msg;
    char type;
    const struct shell *shell;
} Log;

/* size of stack area used by each thread */
#define STACKSIZE 1024
/* scheduling priority used by each thread */
#define PRIORITY 7

int filter_flag = 0; // global message flag (0 - ALL (DEFAULT), 1 - ERROR, 2 - LOG, 3 - DEBUG)
K_FIFO_DEFINE(queuefifo); // Initialise the queue

/** 
 *  Select the flag for the message filter.
 **/
void os_log_filter(char filter) {

    switch (filter) {
    case 'e':
       filter_flag = 1;
        break;
    case 'l':
       filter_flag = 2;
        break;
    case 'd':
        filter_flag = 3;
        break;
    case 'a':
       filter_flag = 0;
        break;
    default:
        break;
    }
}

/** 
 *  Retrive message from queue and print message onto the shell in their respective colour.
 **/
void print_log() {
    int msg_color = 0; // set a default color;

    while (1) {

        struct Log *recv = k_fifo_get(&queuefifo, K_FOREVER);

        switch (recv->type) {
        case 'e':

            msg_color = SHELL_ERROR;
            break;
        case 'l':

            msg_color = SHELL_INFO;
            break;
        case 'd':

            msg_color = SHELL_OPTION;
            break;
        default:
            break;
        }
        
        switch (filter_flag)
        {
        case 0:
            shell_fprintf(recv->shell, msg_color, "%s", recv->log_msg);
            k_free(recv);
            break;
        case 1:
            if (msg_color == SHELL_ERROR) {

                shell_fprintf(recv->shell, msg_color, "%s", recv->log_msg);     
            }
            k_free(recv);
            break;
        case 2:
            if (msg_color == SHELL_INFO) {
                
                shell_fprintf(recv->shell, msg_color, "%s", recv->log_msg);     
            }
            k_free(recv);
            break;
        case 3:
            if (msg_color == SHELL_OPTION) {
                
                shell_fprintf(recv->shell, msg_color, "%s", recv->log_msg);     
            }
            k_free(recv);
            break;
        default:
            break;
        }
    }
}

/** 
 *  Send message from shell to a queue.
 **/
void os_log_transmit(char *msg, char type, const struct shell *shell) {

    struct Log tx_data = {
        .log_msg = msg,
        .type = type,
        .shell = shell
    };

    size_t size = sizeof(struct Log);
    char *mem_ptr = k_malloc(size);
    __ASSERT_NO_MSG(mem_ptr != 0);
    memcpy(mem_ptr, &tx_data, size);
    k_fifo_put(&queuefifo, mem_ptr);
    k_msleep(100);  // allow time for data transfer to the queue
}

K_THREAD_DEFINE(print_log_id, STACKSIZE, print_log, NULL, NULL, NULL, PRIORITY, 0, 0); // Create a log task.