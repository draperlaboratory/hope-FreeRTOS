/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * NOTE 1:  This project provides two demo applications.  A simple blinky
 * style project, and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting in main.c is used to select
 * between the two.  See the notes on using mainCREATE_SIMPLE_BLINKY_DEMO_ONLY
 * in main.c.  This file implements the simply blinky style version.
 *
 * NOTE 2:  This file only contains the source code that is specific to the
 * basic demo.  Generic functions, such FreeRTOS hook functions, and functions
 * required to configure the hardware are defined in main.c.
 ******************************************************************************
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Xilinx driver includes. */
#include "xuartns550.h"
#include "bsp.h"
#include "plic_driver.h"

#define TEST_BUFFER_SIZE	3
#define INPUT_LENGTH 		10

/*-----------------------------------------------------------*/

/*
 * Called by main when PROG=main_newuart
 */
void main_newuart( void );

/*
 * The tasks as described in the comments at the top of this file.
 */
static void vUartSelfTest( void *pvParameters );

static void vUartIntrTest( void *pvParameters );

static void vInputTest( void *pvParameters );

static void vTestPrintf( void *pvParameters );

static void UartNs550StatusHandler(void *CallBackRef, u32 Event, 
	unsigned int EventData);

static int UartNs550SetupIntrSystem(XUartNs550 *UartPtr);

/*-----------------------------------------------------------*/

void main_newuart( void )
{
	/* Create UART test */
	xTaskCreate( vInputTest, "UART Printf Test", 3000, NULL, 0, NULL );

	/* Start the kernel.  From here on, only tasks and interrupts will run. */
	vTaskStartScheduler();


	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created.  See the memory management section on the
	FreeRTOS web site for more details on the FreeRTOS heap
	http://www.freertos.org/a00111.html. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Instance of UART device */
static XUartNs550 UartNs550;

u8 SendBuffer[TEST_BUFFER_SIZE];	/* Buffer for Transmitting Data */
u8 RecvBuffer[TEST_BUFFER_SIZE];	/* Buffer for Receiving Data */

/* Counters used to determine when buffer has been send and received */
static volatile int TotalReceivedCount;
static volatile int TotalSentCount;
static volatile int TotalErrorCount;

void vUartIntrTest( void *pvParameters )
{
	/* Test using interrupts with UART1 in loopback mode */

	(void) pvParameters;

	int Status_Init, Status_SetupIntr, Status_SelfTest;
	u32 Index;
	u16 Options;
	u32 BadByteCount = 0;

	/* Initialize the UartNs550 driver so that it's ready to use */
	Status_Init = XUartNs550_Initialize(&UartNs550, XPAR_UARTNS550_0_DEVICE_ID);
	configASSERT(Status_Init == XST_SUCCESS);

	/* Perform a self-test to ensure that the hardware was built correctly */
	Status_SelfTest = XUartNs550_SelfTest(&UartNs550);
	configASSERT(Status_SelfTest == XST_SUCCESS);

	/* Setup interrupt system */
	Status_SetupIntr = UartNs550SetupIntrSystem(&UartNs550);
	configASSERT(Status_SetupIntr == XST_SUCCESS);

	/* Set UART status handler to indicate that XUartNs550StatusHandler
	should be called when there is an interrupt */
	XUartNs550_SetHandler(&UartNs550, UartNs550StatusHandler, 
		&UartNs550);

	/* Enable interrupts, enable loopback mode, and enable FIFOs */
	Options = XUN_OPTION_DATA_INTR | XUN_OPTION_LOOPBACK |
			XUN_OPTION_FIFOS_ENABLE | XUN_FIFO_TX_RESET | XUN_FIFO_RX_RESET;
	XUartNs550_SetOptions(&UartNs550, Options);

	/* Fill send buffer and zero receive buffer */
	for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
		SendBuffer[Index] = Index + 'A';
		RecvBuffer[Index] = 0;
	}

	/* Receive data */
	XUartNs550_Recv(&UartNs550, RecvBuffer, TEST_BUFFER_SIZE);

	/* Send buffer */
	XUartNs550_Send(&UartNs550, SendBuffer, TEST_BUFFER_SIZE);

	/* Wait for the entire buffer to be received */
	while ((TotalReceivedCount != TEST_BUFFER_SIZE) ||
		(TotalSentCount != TEST_BUFFER_SIZE));

	/* Check that buffer was successfully received */
	for (Index = 0; Index < TEST_BUFFER_SIZE; Index++) {
		if (RecvBuffer[Index] != SendBuffer[Index]) {
			BadByteCount++;
		}
	}

	/* Check that there were no bytes wrong or missed */
	configASSERT( BadByteCount == 0 );

	/* Clear the counters */
	TotalErrorCount = 0;
	TotalReceivedCount = 0;
	TotalSentCount = 0;

	vTaskDelete(NULL);

}

/*-----------------------------------------------------------*/
void vTestPrintf( void *pvParameters )
{
	(void) pvParameters;

	int Status_Init, Status_SetupIntr, Status_SelfTest;
	u32 Index;
	u16 Options;
	u32 BadByteCount = 0;

	/* Initialize the UartNs550 driver so that it's ready to use */
	Status_Init = XUartNs550_Initialize(&UartNs550, XPAR_UARTNS550_0_DEVICE_ID);
	configASSERT(Status_Init == XST_SUCCESS);

	/* Setup interrupt system */
	Status_SetupIntr = UartNs550SetupIntrSystem(&UartNs550);
	configASSERT(Status_SetupIntr == XST_SUCCESS);

	/* Set UART status handler to indicate that UartNs550StatusHandler
	should be called when there is an interrupt */
	XUartNs550_SetHandler(&UartNs550, UartNs550StatusHandler, 
		&UartNs550);

	/* Enable interrupts and reset and enable FIFOs */
	Options = XUN_OPTION_DATA_INTR | XUN_OPTION_FIFOS_ENABLE 
				| XUN_FIFO_TX_RESET | XUN_FIFO_RX_RESET;
	XUartNs550_SetOptions(&UartNs550, Options);

	printf("hi!");

	/* Clear the counters */
	TotalErrorCount = 0;
	TotalReceivedCount = 0;
	TotalSentCount = 0;

	vTaskDelete(NULL);
}
/*-----------------------------------------------------------*/

void vInputTest( void *pvParameters )
{
	/* Test receiving chars using interrupts via UART1 */
	(void) pvParameters;

	int Status_Init, Status_SetupIntr, Status_SelfTest;
	u32 Index;
	u16 Options;
	u32 BadByteCount = 0;

	/* Initialize the UartNs550 driver so that it's ready to use */
	Status_Init = XUartNs550_Initialize(&UartNs550, XPAR_UARTNS550_0_DEVICE_ID);
	configASSERT(Status_Init == XST_SUCCESS);

	/* Setup interrupt system */
	Status_SetupIntr = UartNs550SetupIntrSystem(&UartNs550);
	configASSERT(Status_SetupIntr == XST_SUCCESS);

	/* Set UART status handler to indicate that UartNs550StatusHandler
	should be called when there is an interrupt */
	XUartNs550_SetHandler(&UartNs550, UartNs550StatusHandler, 
		&UartNs550);

	/* Enable interrupts and reset and enable FIFOs */
	Options = XUN_OPTION_DATA_INTR | XUN_OPTION_FIFOS_ENABLE 
				| XUN_FIFO_TX_RESET | XUN_FIFO_RX_RESET;
	XUartNs550_SetOptions(&UartNs550, Options);

	/* Receive data */
	XUartNs550_Recv(&UartNs550, RecvBuffer, INPUT_LENGTH);

	printf("Waiting\r\n");
	while( TotalReceivedCount < INPUT_LENGTH );
	
	printf("Rx: %s \n", RecvBuffer);

	/* Clear the counters */
	TotalErrorCount = 0;
	TotalReceivedCount = 0;
	TotalSentCount = 0;

	vTaskDelete(NULL);
}

/*-----------------------------------------------------------*/
void vUartSelfTest( void *pvParameters )
{
	/* Test UART1's self-test functionality  */

	(void) pvParameters;

	int Status_Init, Status_SelfTest;

	/* Initialize the UartNs550 driver so that it's ready to use */
	Status_Init = XUartNs550_Initialize(&UartNs550, XPAR_UARTNS550_0_DEVICE_ID);
	configASSERT(Status_Init == XST_SUCCESS);

	/* Perform a self-test to ensure that the hardware was built correctly */
	Status_SelfTest = XUartNs550_SelfTest(&UartNs550);
	configASSERT(Status_SelfTest == XST_SUCCESS);

	vTaskDelete(NULL);

}

/*-----------------------------------------------------------*/

static int UartNs550SetupIntrSystem(XUartNs550 *UartPtr)
{
	int Status;

	/*
  	* Connect a device driver handler that will be called when an interrupt
 	* for the device occurs, the device driver handler performs the
	* specific interrupt processing for the device
	*/
	Status = PLIC_register_interrupt_handler(&Plic, PLIC_SOURCE_UART0,
			XUartNs550_InterruptHandler, UartPtr);
	if (Status != PLIC_SOURCE_UART0) {
    return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*-----------------------------------------------------------*/

static void UartNs550StatusHandler(void *CallBackRef, u32 Event, 
	unsigned int EventData)
{
	u8 Errors;

	/* All of the data was sent */
	if (Event == XUN_EVENT_SENT_DATA) {
		TotalSentCount = EventData;
	}

	/* All of the data was received */
	if (Event == XUN_EVENT_RECV_DATA) {
		TotalReceivedCount = EventData;
	}

	/* Data was received, but different than expected number of bytes */
	if (Event == XUN_EVENT_RECV_TIMEOUT) {
		TotalReceivedCount = EventData;
	}

	/* Data was received with an error */
	if (Event == XUN_EVENT_RECV_ERROR) {
		TotalReceivedCount = EventData;
		TotalErrorCount++;
		Errors = XUartNs550_GetLastErrors(CallBackRef);
	}

}

/*-----------------------------------------------------------*/
// /* For API */
// bool uartInit(UartDriver *uartp)
// {
// 	/* Need a way to specify DEVICE0 or DEVICE1 */

// 	int Status;
// 	u16 Options;

// 	/* Initialize the UartNs550 driver so that it's ready to use */
// 	Status = XUartNs550_Initialize(&UartNs550, XPAR_UARTNS550_1_DEVICE_ID);
// 	if (Status != XST_SUCCESS) {
// 		return false;
// 	}

// 	/* Setup interrupt system */
// 	Status = UartNs550SetupIntrSystem(&UartNs550);
// 	if (Status != XST_SUCCESS) {
// 		return false;
// 	}

// 	/* Set UART status handler to indicate that UartNs550StatusHandler
// 	should be called when there is an interrupt */
// XUartNs550_SetHandler(&UartNs550, UartNs550StatusHandler, 
// 		&UartNs550);

// 	/* Enable interrupts and enable FIFOs */
// 	Options = XUN_OPTION_DATA_INTR | XUN_OPTION_FIFOS_ENABLE;
// 	XUartNs550_SetOptions(&UartNs550, Options);

// 	return true;
// }

/*-----------------------------------------------------------*/