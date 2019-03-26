#include <stdint.h>
#include <string.h> // for `memset`
#include "uart.h"

#include "FreeRTOS.h"
#include "semphr.h"

/* Xilinx driver includes. */
#include "xuartns550.h"
#include "bsp.h"
#include "plic_driver.h"

/*****************************************************************************/
/* Defines */

#define UART_TX_DELAY pdMS_TO_TICKS(500)
#define UART_RX_DELAY pdMS_TO_TICKS(10000)
#define UART_BUFFER_SIZE	64

/*****************************************************************************/
/* Structs */

/* Device driver for UART peripheral */
struct UartDriver {
  XUartNs550 Device; /* Xilinx Uart driver */
  uint8_t SendBuffer[UART_BUFFER_SIZE];	/* Buffer for Transmitting Data */
  uint8_t RecvBuffer[UART_BUFFER_SIZE];	/* Buffer for Receiving Data */
  /* Counters used to determine when buffer has been send and received */
  volatile int TotalReceivedCount;
  volatile int TotalSentCount;
  volatile int TotalErrorCount;
  volatile uint8_t Errors;
  int tx_len; /* Length of TX transaction */
  int rx_len; /* Length of RX transaction */
  SemaphoreHandle_t tx_mutex; /* Mutex for TX transmissions */
  SemaphoreHandle_t rx_mutex; /* Mutex for RX transmissions */
  TaskHandle_t tx_task; /* handle for task that called TX */
  TaskHandle_t rx_task; /* handle for task that called RX */
};

/*****************************************************************************/
/* Static functions, macros etc */

static uint8_t uart_rxchar(struct UartDriver* Uart);
static uint8_t uart_txchar(struct UartDriver* Uart, uint8_t c);
static int uart_rxbuffer(struct UartDriver* Uart, uint8_t *ptr, int len);
static int uart_txbuffer(struct UartDriver* Uart, uint8_t *ptr, int len);
static void uart_init(struct UartDriver* Uart, uint8_t device_id, uint8_t plic_source_id);
static void UartNs550StatusHandler(void *CallBackRef, u32 Event, unsigned int EventData);

/*****************************************************************************/
/* Global defines */

/* Instance of UART device */
static XUartNs550 UartNs550_0;
struct UartDriver Uart0;

/*****************************************************************************/
/* Peripheral specific definitions */

/**
 * Initialize UART 0 peripheral
 */
void uart0_init(void) {
  uart_init(&Uart0, XPAR_UARTNS550_0_DEVICE_ID, PLIC_SOURCE_UART0);
}

/**
 * Return 1 if UART0 has at leas 1 byte in the RX FIFO
 */
int uart0_rxready(void)
{
  // TODO: Fixme
  return 0;
}

/**
 * Receive a single byte. Polling mode, waits until finished
 */
char uart0_rxchar(void)
{
  return (char)uart_rxchar(&Uart0);
}

/**
 * Transmit a buffer. Waits indefinitely for a UART TX mutex,
 * returns number of transferred bytes or -1 in case of an error.
 * Synchronous API.
 */
int uart0_txbuffer(char *ptr, int len) {
  return uart_txbuffer(&Uart0, (uint8_t *)ptr, len);
}

/**
 * Transmit a single byte. Polling mode, waits until finished
 */
char uart0_txchar(char c)
{
  return (char)uart_txchar(&Uart0, (uint8_t)c);
}

int uart0_rxbuffer(char *ptr, int len)
{
  return uart_rxbuffer(&Uart0, (uint8_t *)ptr, len);
}

/*****************************************************************************/
/* Driver specific defintions */

/**
 * Initialize UART peripheral
 */
static void uart_init(struct UartDriver* Uart, uint8_t device_id, uint8_t plic_source_id)
{
  // Initialize struct
  Uart->tx_mutex = xSemaphoreCreateMutex();
  Uart->rx_mutex = xSemaphoreCreateMutex();
  Uart->Device = UartNs550_0;
  Uart->tx_task = NULL;

  /* Initialize the UartNs550 driver so that it's ready to use */
	configASSERT( XUartNs550_Initialize(&Uart->Device, device_id) == XST_SUCCESS);

  /* Setup interrupt system */
  configASSERT( PLIC_register_interrupt_handler(&Plic, plic_source_id,
    (XInterruptHandler)XUartNs550_InterruptHandler, Uart) != 0);

  /* Set UART status handler to indicate that UartNs550StatusHandler
	should be called when there is an interrupt */
	XUartNs550_SetHandler(&Uart->Device, UartNs550StatusHandler, Uart);

  /* Enable interrupts, and enable FIFOs */
	uint16_t Options = XUN_OPTION_DATA_INTR |
			XUN_OPTION_FIFOS_ENABLE | XUN_FIFO_TX_RESET | XUN_FIFO_RX_RESET;
	XUartNs550_SetOptions(&Uart->Device, Options);

  /* Zero transmit and receive buffer */
  memset(Uart->SendBuffer, 0, UART_BUFFER_SIZE);
  memset(Uart->RecvBuffer, 0, UART_BUFFER_SIZE);
}

/**
 * Receive a single byte from UART peripheral. Polling mode,
 * waits until finished.
 */
static uint8_t uart_rxchar(struct UartDriver* Uart) {
  #if XPAR_UART_USE_POLLING_MODE
    return XUartNs550_RecvByte(Uart->Device.BaseAddress);
  #else
    uint8_t buf = 0;
    uart_rxbuffer(Uart, &buf, 1);
    return buf;
  #endif
}

/**
 * Transmit a single byte from UART peripheral. Polling mode,
 * waits until finished.
 */
static uint8_t uart_txchar(struct UartDriver* Uart, uint8_t c) {
  #if XPAR_UART_USE_POLLING_MODE
    XUartNs550_SendByte(Uart->Device.BaseAddress, c);
    return c;
  #else
    return uart_txchar(Uart, c);
  #endif
}

/**
 * Transmit a buffer. Waits for @UART_TX_DELAY ms. Synchronous API.
 * Returns number of transmitted bytes or -1 in case of a timeout.
 */
static int uart_txbuffer(struct UartDriver* Uart, uint8_t *ptr, int len) {
  static int returnval;
  /* First acquire mutex */
  configASSERT( Uart->tx_mutex != NULL);
  configASSERT( xSemaphoreTake(Uart->tx_mutex, portMAX_DELAY) == pdTRUE);
  /* Get current task handle */
  Uart->tx_task = xTaskGetCurrentTaskHandle();
  Uart->tx_len = len;
  /* Send buffer */
	XUartNs550_Send(&Uart->Device, ptr, len);
  /* wait for notification */
	if (xTaskNotifyWait( 0, 0, NULL, UART_TX_DELAY )) {
    /* TX succesfull, return number of transmitted bytes */
    returnval = Uart->TotalSentCount;
  } else {
    /* timeout occured */
    returnval = -1;
  }
  /* Release mutex and return */
  xSemaphoreGive( Uart->tx_mutex );
  return returnval;
}

/**
 * Receive a buffer of data. Synchronous API. Note that right now it waits
 * indefinitely until the buffer is filled.
 */
static int uart_rxbuffer(struct UartDriver* Uart, uint8_t *ptr, int len) {
  static int returnval;
  /* First acquire mutex */
  configASSERT( Uart->rx_mutex != NULL);
  configASSERT( xSemaphoreTake(Uart->rx_mutex, portMAX_DELAY) == pdTRUE);
  /* Get current task handle */
  Uart->rx_task = xTaskGetCurrentTaskHandle();
  Uart->rx_len = len;
  /* Send buffer */
	XUartNs550_Recv(&Uart->Device, ptr, len);
  /* wait for notification */
	if (xTaskNotifyWait( 0, 0, NULL, portMAX_DELAY )) {
    /* TX succesfull, return number of transmitted bytes */
    returnval = Uart->TotalReceivedCount;
  } else {
    /* timeout occured */
    returnval = -1;
  }
  /* Release mutex and return */
  xSemaphoreGive( Uart->rx_mutex );
  return returnval;
}

/**
 * UART Interrupt handler. Handles RX, TX, Timeouts and Errors.
 */
static void UartNs550StatusHandler(void *CallBackRef, u32 Event, 
	unsigned int EventData)
{
  struct UartDriver* Uart = (struct UartDriver*)CallBackRef;

	/* All of the data was sent */
	if (Event == XUN_EVENT_SENT_DATA) {
		Uart->TotalSentCount = EventData;
    if (Uart->tx_len == Uart->TotalSentCount) {
      // data was sent, notify the task
      Uart->tx_len = 0;
      // Allow mixing of uart_txbuf and uart_txbyte calls
      //configASSERT(Uart->tx_task != NULL);
      if (Uart->tx_task != NULL) {
        // call task handler only if it was registered
        static BaseType_t askForContextSwitch = pdFALSE;
	      vTaskNotifyGiveFromISR( Uart->tx_task, &askForContextSwitch);
      }
    }
	}

	/* All of the data was received */
	if (Event == XUN_EVENT_RECV_DATA) {
		Uart->TotalReceivedCount = EventData;
    if (Uart->rx_len == Uart->TotalReceivedCount) {
      // data was sent, notify the task
      Uart->rx_len = 0;
      // Allow mixing of uart_txbuf and uart_txbyte calls
      //configASSERT(Uart->rx_task != NULL);
      if ( Uart->rx_task != NULL) {
        // call task handler only if it was registered
        static BaseType_t askForContextSwitch = pdFALSE;
	      vTaskNotifyGiveFromISR( Uart->rx_task, &askForContextSwitch);
      }
    }
	}

	/* Data was received, but different than expected number of bytes */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		Uart->TotalReceivedCount = EventData;
	}

	/* Data was received with an error */
	if (Event == XUN_EVENT_RECV_ERROR) {
		Uart->TotalReceivedCount = EventData;
		Uart->TotalErrorCount++;
		Uart->Errors = XUartNs550_GetLastErrors(&Uart->Device);
	}
}