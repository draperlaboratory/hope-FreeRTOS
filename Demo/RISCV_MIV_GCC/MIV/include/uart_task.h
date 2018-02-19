#ifndef UART_TASK_H
#define UART_TASK_H

/*
   Rough interface bottling up uart support in FreeRTOS.  We access the uart directly only
   from a dedicated task.  Read/Write operations go through queues that block the accessing
   tasks.  The uart task runs with high frequency, polling the uart, hopefully sufficiently
   quickly to drain any incoming data from the uart.  An alternative would be to use
   interrupt driven notifications, with a very high priority uart task to read the uart when
   an interrupt is noticed.
*/

/**
   Initializes the uart task.  Must be called before any write operations via uart_write
   are attempted.  <code>uart_task_priority</code> should be higher than tasks using the uart
   to allow the uart task to clear data sufficiently quickly.
*/
void init_uart(int uart_task_priority);

/**
   Writes <code>len</code> bytes to the uart queue.  The data will be written to the uart
   at some future time, which should be within a few milliseconds.  This function may block
   the writing process if the output queue on the uart is full.
*/
void uart_write(void const *buff, int len);

/**
   Reads from the uart.  Blocks until 'len' bytes are read.  Returns the length read.
   It should only be possible to have a return value that doesn't match the input
   'len' in the case of some unnatural error.
 */
int uart_read(void *buff, int len);

/**
   Returns a count of how many bytes are available on the uart to be read by
   uart_read.  If you pass this value into uart_read, you will be guaranteed
   that the read will not block for lack of input.  It is possible that you
   will block briefly waiting for the uart service task to add more data
   to the queue if data is incoming while you are reading.
 */
int uart_bytes_available(void);

#define UART_TASK_DEFAULT_PRIORITY 2

// For debugging only!
void uart_print(const char *p);

#endif
