/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "utils.h"
#include "uart_task.h"

#define MIV_BOARD

#ifdef MIV_BOARD
#include "miv_core_uart.h"

#define MIV_COREUART_BASE 0x70001000

static miv_core_uart_t * pio = (miv_core_uart_t *)(void*)MIV_COREUART_BASE;
#define uart_init miv_core_uart_init
#define txchar miv_core_uart_txchar
#define rxchar miv_core_uart_rxchar
#define rxready miv_core_uart_rxready
#else
#include "ns16550.h"

#define uart_init ns16550_init
#define txchar ns16550_txchar
#define rxchar ns16550_rxchar
#define rxready ns16550_rxready
static ns16550_pio_t * pio = (ns16550_pio_t *)(void*)NS16550_AP_BASE;
#endif

#define WRITE_QUEUE_DEPTH ((UBaseType_t)10)
#define READ_QUEUE_DEPTH ((UBaseType_t)10)

static QueueHandle_t uart_write_queue;
static QueueHandle_t uart_read_queue;

// For debugging only!
void uart_print(const char *p) {
  while (*p) {
    txchar(pio, *p++);
  }
}

static void uart_polling_task(void *p) {
  TickType_t last_wake_time;
  
  (void)p;

  last_wake_time = xTaskGetTickCount();
  while (1) {
    uint8_t byte;

    /*
      First we look to see if there is input waiting on the uart.  That we
      want to clear first, to prevent bits from being dropped.  We peek the queue
      to make sure we wouldn't block if we wrote input to it, and then query the uart.
     */
    while (rxready(pio) && uxQueueMessagesWaiting(uart_read_queue) < READ_QUEUE_DEPTH) {
      byte = (uint8_t)rxchar(pio);
      /*
	We have to block here: another task could be reading the queue when we
	were sliced back in here.  If they have the queue locked, we have to wait for
	their read to finish.  If we don't, we'll drop this character, so we block
	indefinitely.  Their read will take a small amount of time, so we'll be
	guaranteed to return before very long.

	There's something to be said for timing out, here, after, say, 5 seconds, so we
	don't run into some wierd unforseen case where this task gets blocked on some other
	task that is going to go away?  Can't see how that could happen.  The result would be
	that we'd drop this character in that case, but by then we'd have dropped lots of
	other input on the uart because we couldn't keep up.
       */
      xQueueSend(uart_read_queue, &byte, portMAX_DELAY);
    }
    
    UBaseType_t count = uxQueueMessagesWaiting(uart_write_queue);
    if (count)
      txchar(pio, '|');
    while (count-- > 0) {
      if (xQueueReceive(uart_write_queue, &byte, 1) == errQUEUE_EMPTY)
	kpanic("uart write queue empty\n");
      txchar(pio, byte);
    }
    vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(10));
  }
}

void init_uart(int uart_task_priority) {
#ifdef MIV_BOARD
  miv_core_uart_init(pio, 0, 0); //NS16550_DEFAULT_BAUD_RATE, NS16550_CLOCK_RATE);
#else
  ns16550_init(pio, NS16550_DEFAULT_BAUD_RATE, NS16550_CLOCK_RATE);
#endif
  xTaskCreate(uart_polling_task, "uart task", 1000, NULL, uart_task_priority, NULL);
  uart_write_queue = xQueueCreate(WRITE_QUEUE_DEPTH, 1);
  uart_read_queue = xQueueCreate(READ_QUEUE_DEPTH, 1);
}

void uart_write(void const *buff, int len) {
  uint8_t const *p = (uint8_t const *)buff;
  while (len-- > 0) {
    if (xQueueSend(uart_write_queue, p, portMAX_DELAY) == errQUEUE_FULL) {
      kpanic("uart write queue full\n");
    }
    p++;
  }
//uart_print("uart_write done\n");
}

int uart_bytes_available(void) {
  UBaseType_t available_space = uxQueueSpacesAvailable(uart_read_queue);
  return (int)(READ_QUEUE_DEPTH - available_space);
}

int uart_read(void *buff, int len) {
  int i;
  uint8_t *p = (uint8_t *)buff;
  for (i = 1; i <= len; i++) {
    if (xQueueReceive(uart_read_queue, p, portMAX_DELAY) == pdFALSE)
      return i;
    p++;
  }
  return len;
}

/*
  This doesn't belong in this file, but because it uses the uart directly, I stuffed it
  here temporarily.
*/
__attribute__((noreturn)) void kpanic(const char *p) {
  while (*p) {
    txchar(pio, *p++);
  }
  while (1);
}
