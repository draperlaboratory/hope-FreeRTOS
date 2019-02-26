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
 * main_blinky() creates one queue, and two tasks.  It then starts the
 * scheduler.
 *
 * The Queue Send Task:
 * The queue send task is implemented by the prvQueueSendTask() function in
 * this file.  prvQueueSendTask() sits in a loop that causes it to repeatedly
 * block for 1000 milliseconds, before sending the value 100 to the queue that
 * was created within main_blinky().  Once the value is sent, the task loops
 * back around to block for another 1000 milliseconds...and so on.
 *
 * The Queue Receive Task:
 * The queue receive task is implemented by the prvQueueReceiveTask() function
 * in this file.  prvQueueReceiveTask() sits in a loop where it repeatedly
 * blocks on attempts to read data from the queue that was created within
 * main_blinky().  When data is received, the task checks the value of the
 * data, and if the value equals the expected 100, writes 'Blink' to the UART
 * (the UART is used in place of the LED to allow easy execution in QEMU).  The
 * 'block time' parameter passed to the queue receive function specifies that
 * the task should be held in the Blocked state indefinitely to wait for data to
 * be available on the queue.  The queue receive task will only leave the
 * Blocked state when the queue send task writes to the queue.  As the queue
 * send task writes to the queue every 1000 milliseconds, the queue receive
 * task leaves the Blocked state every 1000 milliseconds, and therefore toggles
 * the LED every 200 milliseconds.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* xillinx driver include */
#include "xaxidma.h"
#include "plic_driver.h"
#include "bsp.h"

/* demo includes */
#include "xaxiethernet_example.h"

/* Priorities used by the tasks. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
to ticks using the pdMS_TO_TICKS() macro. */
#define mainQUEUE_SEND_FREQUENCY_MS			pdMS_TO_TICKS( 1000 )

/* The maximum number items the queue can hold.  The priority of the receiving
task is above the priority of the sending task, so the receiving task will
preempt the sending task and remove the queue items each time the sending task
writes to the queue.  Therefore the queue will never have more than one item in
it at any time, and even with a queue length of 1, the sending task will never
find the queue full. */
#define mainQUEUE_LENGTH					( 1 )

/*-----------------------------------------------------------*/

/*
 * Called by main when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1 in
 * main.c.
 */
void main_drivers( void );

/**
 * Modified ethernet_multicast_example
 */
static void prvDriverTestTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/*-----------------------------------------------------------*/

void main_drivers( void )
{
	/* Create the queue. */
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );

	if( xQueue != NULL )
	{
		xTaskCreate( prvDriverTestTask, "Driver_test_task", configMINIMAL_STACK_SIZE * 2U, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );

		/* Start the tasks and timer running. */
		vTaskStartScheduler();
	}

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the Idle and/or
	timer tasks to be created.  See the memory management section on the
	FreeRTOS web site for more details on the FreeRTOS heap
	http://www.freertos.org/a00111.html. */
	for( ;; );
}
/*-----------------------------------------------------------*/


/*************************** Constant Definitions ****************************/

#define RXBD_CNT		128	/* Number of RxBDs to use */
#define TXBD_CNT		128	/* Number of TxBDs to use */
#define BD_ALIGNMENT		XAXIDMA_BD_MINIMUM_ALIGNMENT/* Byte alignment of
							     * BDs
							     */
#define PAYLOAD_SIZE		1000	/* Payload size used in examples */
#define BD_RX_STATUS_OFFSET	2

/*
 * Number of bytes to reserve for BD space for the number of BDs desired
 */
#define RXBD_SPACE_BYTES XAxiDma_BdRingMemCalc(BD_ALIGNMENT, RXBD_CNT)
#define TXBD_SPACE_BYTES XAxiDma_BdRingMemCalc(BD_ALIGNMENT, TXBD_CNT)

/*************************** Variable Definitions ****************************/

static EthernetFrame TxFrame;	/* Transmit buffer */
static EthernetFrame RxFrame;	/* Receive buffer */

XAxiEthernet AxiEthernetInstance;
XAxiDma DmaInstance;

static void AxiEthernetErrorHandler(XAxiEthernet *AxiEthernet);
static void RxIntrHandler(XAxiDma_BdRing *RxRingPtr);
static void TxIntrHandler(XAxiDma_BdRing *TxRingPtr);

/*
 * Aligned memory segments to be used for buffer descriptors
 */
char RxBdSpace[RXBD_SPACE_BYTES] __attribute__ ((aligned(BD_ALIGNMENT)));
char TxBdSpace[TXBD_SPACE_BYTES] __attribute__ ((aligned(BD_ALIGNMENT)));

/*
 * Counters to be incremented by callbacks
 */
static volatile int FramesRx;	  /* Num of frames that have been received */
static volatile int FramesTx;	  /* Num of frames that have been sent */
static volatile int DeviceErrors; /* Num of errors detected in the device */

volatile int Padding;	/* For 1588 Packets we need to pad 8 bytes time stamp value */
volatile int ExternalLoopback; /* Variable for External loopback */

/*
 * Multicast MAC address
 */
char MulticastMAC[6] = { 0x01, 0x00, 0x5E, 0x04, 0x05, 0x06 };

/*
 * Determine how much space for ALL multicast MAC addresses, there are 2**23
 * per RFC1112. Each bit represents an address. The total number of bytes are
 * (2**23)/8=2**20=1MB
 */
u32 McastAddressTable[(MAX_MULTICAST_ADDR>>3)/sizeof(u32)];

#define INTC plic_instance_t
#define XPAR_AXIDMA_0_ENABLE_MULTI_CHANNEL 1 // assuming we have a multi channel DMA

// we don't have any cache, so we don't need to do anything about it
#define Xil_DCacheFlushRange(...)
#define Xil_DCacheInvalidateRange(...)
/*************************** Function Prototypes *****************************/

/*
 * Examples
 */
int AxiEthernetMcastExample(INTC *IntcInstancePtr,
				XAxiEthernet *AxiEthernetInstancePtr,
				XAxiDma *DmaInstancePtr,
				u16 AxiEthernetDeviceId,
				u16 AxiDmaDeviceId,
				u16 AxiEthernetIntrId,
				u16 DmaRxIntrId,
				u16 DmaTxIntrId);

int AxiEthernetSgDmaIntrExtMulticastExample(XAxiEthernet
			*AxiEthernetInstancePtr, XAxiDma *DmaInstancePtr);

/*
 * Interrupt setup and Callbacks for examples
 */

static int AxiEthernetSetupIntrSystem(INTC *IntcInstancePtr,
				XAxiEthernet *AxiEthernetInstancePtr,
				XAxiDma *DmaInstancePtr,
				u16 AxiEthernetIntrId,
				u16 DmaRxIntrId, u16 DmaTxIntrId);

static void AxiEthernetDisableIntrSystem(INTC *IntcInstancePtr,
				   u16 AxiEthernetIntrId,
				   u16 DmaRxIntrId, u16 DmaTxIntrId);

/*
 * Manage functions for Multicast table in RAM
 */
static int AxiEthernet_AddExtMulticast(void *AddressPtr);
static int AxiEthernet_ClearExtMulticast(void *AddressPtr);
static int AxiEthernet_GetExtMulticast(void *AddressPtr);

/*****************************************************************************/






static void prvDriverTestTask( void *pvParameters ) {
    (void) pvParameters;
    int Status;

    AxiEthernetUtilErrorTrap("\r\n--- Enter main() ---");
	AxiEthernetUtilErrorTrap("This test may take several minutes to finish");

	/*
	 * Call the Axi Ethernet multicast example.
	 */
	Status = AxiEthernetMcastExample(&Plic,
						&AxiEthernetInstance,
						&DmaInstance,
						XPAR_AXIETHERNET_0_DEVICE_ID,
						XPAR_AXIDMA_0_DEVICE_ID,
						PLIC_SOURCE_ETH,
						XPAR_AXIETHERNET_0_DMA_RX_INTR,
						XPAR_AXIETHERNET_0_DMA_TX_INTR);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Axiethernet extmulticast Example Failed\r\n");
		AxiEthernetUtilErrorTrap("--- Exiting main() ---");
		// TODO: handle better
        __asm volatile( "ebreak" );
	}

	AxiEthernetUtilErrorTrap("Successfully ran Axiethernet extmulticast Example\r\n");
	AxiEthernetUtilErrorTrap("--- Exiting main() ---");

    while(1) {
        ;;
    }
}


/*****************************************************************************/
/**
*
* This function demonstrates the usage usage of the Axi Ethernet extended
* multicast feature. It uses AXI DMA core in interrupt mode to transfer Ethernet
* frames.
*
*
* @param	IntcInstancePtr is a pointer to the instance of the Intc
*		component.
* @param	AxiEthernetInstancePtr is a pointer to the instance of the
*		AxiEthernet component.
* @param	DmaInstancePtr is a pointer to the instance of the AXIDMA
*		component.
* @param	AxiEthernetDeviceId is Device ID of the Axi Ethernet Device ,
*		typically XPAR_<AXIETHERNET_instance>_DEVICE_ID value from
*		xparameters.h.
* @param	AxiDmaDeviceId is Device ID of the Axi DMAA Device ,
*		typically XPAR_<AXIDMA_instance>_DEVICE_ID value from
*		xparameters.h.
* @param	AxiEthernetIntrId is the Interrupt ID and is typically
*		XPAR_<INTC_instance>_<AXIETHERNET_instance>_VEC_ID
*		value from xparameters.h.
* @param	DmaRxIntrId is the interrupt id for DMA Rx and is typically
*		taken from XPAR_<AXIETHERNET_instance>_CONNECTED_DMARX_INTR
* @param	DmaTxIntrId is the interrupt id for DMA Tx and is typically
*		taken from XPAR_<AXIETHERNET_instance>_CONNECTED_DMATX_INTR
*
* @return
*		- XST_SUCCESS to indicate success.
*		- XST_FAILURE.to indicate failure.
*
* @note		AxiDma hardware must be initialized before initializing
*		AxiEthernet. Since AxiDma reset line is connected to the
*		AxiEthernet reset line, a reset of AxiDma hardware during its
*		initialization would reset AxiEthernet.
*
******************************************************************************/
int AxiEthernetMcastExample(INTC *IntcInstancePtr,
				XAxiEthernet *AxiEthernetInstancePtr,
				XAxiDma *DmaInstancePtr,
				u16 AxiEthernetDeviceId,
				u16 AxiDmaDeviceId,
				u16 AxiEthernetIntrId,
				u16 DmaRxIntrId,
				u16 DmaTxIntrId)
{
	int Status;
	int LoopbackSpeed;
	XAxiEthernet_Config *MacCfgPtr;
	XAxiDma_Config *DmaConfig;
	XAxiDma_BdRing *RxRingPtr = XAxiDma_GetRxRing(DmaInstancePtr);
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(DmaInstancePtr);
	XAxiDma_Bd BdTemplate;

	/*************************************/
	/* Setup device for first-time usage */
	/*************************************/

	/*
	 *  Get the configuration of AxiEthernet hardware.
	 */
	MacCfgPtr = XAxiEthernet_LookupConfig(AxiEthernetDeviceId);

	/*
	 * Check if DMA is present or not.
	 */
	if(MacCfgPtr->AxiDevType != XPAR_AXI_DMA) {
		AxiEthernetUtilErrorTrap
			("Device HW not configured for DMA mode\r\n");
		return XST_FAILURE;
	}

	DmaConfig = XAxiDma_LookupConfig(AxiDmaDeviceId);
	/*
	 * Initialize AXIDMA engine. AXIDMA engine must be initialized before
	 * AxiEthernet. During AXIDMA engine initialization, AXIDMA hardware is
	 * reset, and since AXIDMA reset line is connected to AxiEthernet, this
	 * would ensure a reset of AxiEthernet.
	 */
	Status = XAxiDma_CfgInitialize(DmaInstancePtr, DmaConfig);
	if(Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error initializing DMA\r\n");
		return XST_FAILURE;
	}

	/*
	 * Initialize AxiEthernet hardware.
	 */
	Status = XAxiEthernet_CfgInitialize(AxiEthernetInstancePtr, MacCfgPtr,
						MacCfgPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error in initialize");
		return XST_FAILURE;
	}

	/*
	 * Set the MAC address
	 */
	Status = XAxiEthernet_SetMacAddress(AxiEthernetInstancePtr,
							AxiEthernetMAC);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error setting MAC address");
		return XST_FAILURE;
	}

    // Get MAC address
    char MyMac[6] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
    XAxiEthernet_GetMacAddress(AxiEthernetInstancePtr,MyMac);
	printf("Got mac address: getting mac address to:0x%08x%8x%8x%8x%8x%8x\r\n",
		MyMac[0],  MyMac[1], MyMac[2], MyMac[3], MyMac[4], MyMac[5]);

	/*
	 * Setup RxBD space.
	 *
	 * We have already defined a properly aligned area of memory to store
	 * RxBDs at the beginning of this source code file so just pass its
	 * address into the function. No MMU is being used so the physical and
	 * virtual addresses are the same.
	 *
	 * Setup a BD template for the Rx channel. This template will be copied
	 * to every RxBD. We will not have to explicitly set these again.
	 */

	/* Disable all RX interrupts before RxBD space setup */
	XAxiDma_BdRingIntDisable(RxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/*
	 * Create the RxBD ring
	 */
	Status = XAxiDma_BdRingCreate(RxRingPtr, (u32) &RxBdSpace,
				     (u32) &RxBdSpace, BD_ALIGNMENT, RXBD_CNT);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error setting up RxBD space");
		return XST_FAILURE;
	}
	XAxiDma_BdClear(&BdTemplate);
	Status = XAxiDma_BdRingClone(RxRingPtr, &BdTemplate);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error initializing RxBD space");
		return XST_FAILURE;
	}

	/*
	 * Setup TxBD space.
	 *
	 * Like RxBD space, we have already defined a properly aligned area of
	 * memory to use.
	 */
	/* Disable all RX interrupts before RxBD space setup */
	XAxiDma_BdRingIntDisable(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/*
	 * Create the TxBD ring
	 */
	Status = XAxiDma_BdRingCreate(TxRingPtr, (u32) &TxBdSpace,
				     (u32) &TxBdSpace, BD_ALIGNMENT, TXBD_CNT);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error setting up TxBD space");
		return XST_FAILURE;
	}

	/*
	 * We reuse the bd template, as the same one will work for both rx and
	 * tx.
	 */
	Status = XAxiDma_BdRingClone(TxRingPtr, &BdTemplate);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error initializing TxBD space");
		return XST_FAILURE;
	}

	/*
	 * Set PHY to loopback, speed depends on phy type.
	 * MII is 100 and all others are 1000.
	 */
	if (XAxiEthernet_GetPhysicalInterface(AxiEthernetInstancePtr)
				== XAE_PHY_TYPE_MII) {
		LoopbackSpeed = AXIETHERNET_LOOPBACK_SPEED;
	} else {
		LoopbackSpeed = AXIETHERNET_LOOPBACK_SPEED_1G;
	}

	AxiEthernetUtilEnterLoopback(AxiEthernetInstancePtr, LoopbackSpeed);

	/*
	 * Set PHY<-->MAC data clock
	 */
	Status =  XAxiEthernet_SetOperatingSpeed(AxiEthernetInstancePtr,
					(u16)LoopbackSpeed);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Setting the operating speed of the MAC needs a delay.  There
	 * doesn't seem to be register to poll, so please consider this
	 * during your application design.
	 */
	AxiEthernetUtilPhyDelay(2);

	/*
	 * Connect to the interrupt controller and enable interrupts
	 */
	Status = AxiEthernetSetupIntrSystem(IntcInstancePtr,
			AxiEthernetInstancePtr, DmaInstancePtr,
			AxiEthernetIntrId, DmaRxIntrId, DmaTxIntrId);


	/****************************/
	/* Run the example 	    */
	/****************************/

	/* Run the new multicast feature. Make sure HW has the capability */
	if (XAxiEthernet_IsExtMcast(AxiEthernetInstancePtr)) {
		Status = AxiEthernetSgDmaIntrExtMulticastExample
		(AxiEthernetInstancePtr,DmaInstancePtr);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

	/*
	 * Disable the interrupts for the Axi Ethernet device
	 */
	AxiEthernetDisableIntrSystem(IntcInstancePtr, AxiEthernetIntrId,
						DmaRxIntrId, DmaTxIntrId);

	/*
	 * Stop the device
	 */
	XAxiEthernet_Stop(AxiEthernetInstancePtr);

	return XST_SUCCESS;
}


/*****************************************************************************/
/**
*
* This example sends and receives a single packet in loopback mode with
* extended multicast address support.
*
* The transmit frame will be addressed one of programed multicast addresses.
*
* On receive, HW should pass the frame to receive interrupt handler for further
* address checking.
*
* @param	AxiEthernetInstancePtr is a pointer to the instance of the
*		AxiEthernet component.
* @param	DmaInstancePtr is a pointer to the instance of the AXI DMA
*		component.
*
* @return
*		- XST_SUCCESS to indicate success.
*		- XST_FAILURE to indicate failure.
*
* @note     None.
*
******************************************************************************/
int AxiEthernetSgDmaIntrExtMulticastExample(XAxiEthernet *AxiEthernetInstancePtr
			,			XAxiDma *DmaInstancePtr)
{
	int Status;
	u32 TxFrameLength;
	u32 Reg;
	u32 RxFrameLength;
	u32 RxStatusControlWord;
	int PayloadSize = PAYLOAD_SIZE;
	char MulticastAdd[6];
	XAxiDma_BdRing *RxRingPtr = XAxiDma_GetRxRing(DmaInstancePtr);
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(DmaInstancePtr);
	XAxiDma_Bd *BdPtr;
	XAxiDma_Bd *BdCurPtr;
	u32 BdSts;

	/*
	 * Cannot run this example if extended features support is not enabled
	 */
	if (!XAxiEthernet_IsExtMcast(AxiEthernetInstancePtr)) {
		AxiEthernetUtilErrorTrap("Extended multicast not available");
		return XST_FAILURE;
	}

	/*
	 * Clear variables shared with callbacks
	 */
	FramesRx = 0;
	FramesTx = 0;
	DeviceErrors = 0;

	/*
	 * Calculate frame length (not including FCS)
	 */
	TxFrameLength = XAE_HDR_SIZE + PayloadSize;

	/*
	 * Setup the packet to be transmitted, with destination address set
	 * to provisioned multicast address.
	 */
	AxiEthernetUtilFrameMemClear(&TxFrame);
	AxiEthernetUtilFrameHdrFormatMAC(&TxFrame, MulticastMAC);
	AxiEthernetUtilFrameHdrFormatType(&TxFrame, PayloadSize);
	AxiEthernetUtilFrameSetPayloadData(&TxFrame, PayloadSize);

	/*
	 * Flush the TX frame before giving it to DMA TX channel to transmit.
	 */
	Xil_DCacheFlushRange((u32)&TxFrame, TxFrameLength);

	/*
	 * Clear out receive packet memory area
	 */
	AxiEthernetUtilFrameMemClear(&RxFrame);

	/*
	 * Invalidate the RX frame before giving it to DMA RX channel to
	 * receive data.
	 */
	Xil_DCacheInvalidateRange((u32)&RxFrame, TxFrameLength);

	/*
	 * Interrupt coalescing parameters are set to their default settings
	 * which is to interrupt the processor after every frame has been
	 * processed by the DMA engine.
	 */
	Status = XAxiDma_BdRingSetCoalesce(TxRingPtr, 1, 1);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error setting coalescing for transmit");
		return XST_FAILURE;
	}

	Status = XAxiDma_BdRingSetCoalesce(RxRingPtr, 1, 1);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error setting coalescing for recv");
		return XST_FAILURE;
	}

	AxiEthernetInstancePtr->Options = XAE_DEFAULT_OPTIONS
						| XAE_EXT_MULTICAST_OPTION;
	Status = XAxiEthernet_SetOptions(AxiEthernetInstancePtr,
					AxiEthernetInstancePtr->Options);

	Status |= XAxiEthernet_ClearOptions(AxiEthernetInstancePtr,
					~(AxiEthernetInstancePtr->Options));

	/*
	 * When extended multicasting is enabled, unicast MAC address needs
	 * to be updated in registers UAWL and UAWU. Hence call driver
	 * routine to take care of this.
	 */
	Status |= XAxiEthernet_SetMacAddress(AxiEthernetInstancePtr,
							AxiEthernetMAC);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error Options setup");
		return XST_FAILURE;
	}

	/*
	 * Setup multicast address into the Multicast table for HW to perform
	 * first layer of checking.
	 */
	Status = XAxiEthernet_AddExtMulticastGroup(AxiEthernetInstancePtr,
							MulticastMAC);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error Multicast address setup");
		return XST_FAILURE;
	}

	/*
	 * Read back the setup to make sure Multicast Table is configured with
	 * MAC address.
	 */
	if (!XAxiEthernet_GetExtMulticastGroup(AxiEthernetInstancePtr,
							MulticastMAC)) {
		AxiEthernetUtilErrorTrap("Read back Multicast address fail");
		return XST_FAILURE;
	}

	/*
	 * Setup multicast address for SW to perform final checking.
	 */
	AxiEthernet_AddExtMulticast(MulticastMAC);

	/*
	 * Make sure Tx, Rx and extended multicast are enabled.
	 */
	Status = XAxiEthernet_SetOptions(AxiEthernetInstancePtr,
						XAE_RECEIVER_ENABLE_OPTION |
						XAE_TRANSMITTER_ENABLE_OPTION);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error setting options");
		return XST_FAILURE;
	}

	/*
	 * Start the Axi Ethernet and enable its ERROR interrupts
	 */
	XAxiEthernet_Start(AxiEthernetInstancePtr);
	XAxiEthernet_IntEnable(AxiEthernetInstancePtr, XAE_INT_RECV_ERROR_MASK);

	/*
	 * Enable DMA receive related interrupts
	 */
	XAxiDma_BdRingIntEnable(RxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/*
	 * Allocate 1 RxBD.
	 */
	Status = XAxiDma_BdRingAlloc(RxRingPtr, 1, &BdPtr);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error allocating RxBD");
		return XST_FAILURE;
	}

	/*
	 * Setup the Rx BD.
	 */
	XAxiDma_BdSetBufAddr(BdPtr, (u32)&RxFrame);
#ifndef XPAR_AXIDMA_0_ENABLE_MULTI_CHANNEL
	XAxiDma_BdSetLength(BdPtr, sizeof(RxFrame));
#else
	XAxiDma_BdSetLength(BdPtr, sizeof(RxFrame),
				RxRingPtr->MaxTransferLen);
#endif
	XAxiDma_BdSetCtrl(BdPtr, 0);

	/*
	 * Enqueue to HW
	 */
	Status = XAxiDma_BdRingToHw(RxRingPtr, 1, BdPtr);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error committing RxBD to HW");
		return XST_FAILURE;
	}

	/*
	 * Start DMA RX channel. Now it's ready to receive data.
	 */
	Status = XAxiDma_BdRingStart(RxRingPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Enable DMA transmit related interrupts
	 */
	XAxiDma_BdRingIntEnable(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/*
	 * Allocate 1 TxBD
	 */
	Status = XAxiDma_BdRingAlloc(TxRingPtr, 1, &BdPtr);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error allocating TxBD");
		return XST_FAILURE;
	}

	/*
	 * Setup the TxBD
	 */
	XAxiDma_BdSetBufAddr(BdPtr, (u32)&TxFrame);

#ifndef XPAR_AXIDMA_0_ENABLE_MULTI_CHANNEL
	XAxiDma_BdSetLength(BdPtr, TxFrameLength);
#else
	XAxiDma_BdSetLength(BdPtr, TxFrameLength,
				TxRingPtr->MaxTransferLen);
#endif
	XAxiDma_BdSetCtrl(BdPtr, XAXIDMA_BD_CTRL_TXSOF_MASK |
			     XAXIDMA_BD_CTRL_TXEOF_MASK);

	/*
	 * Enqueue to HW
	 */
	Status = XAxiDma_BdRingToHw(TxRingPtr, 1, BdPtr);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error committing TxBD to HW");
		return XST_FAILURE;
	}

	/*
	 * Start DMA TX channel. Transmission starts at once.
	 */
	Status = XAxiDma_BdRingStart(TxRingPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Wait for transmission to complete
	 */
	while (!FramesTx);

	/*
	 * Now that the frame has been sent, post process our TxBDs.
	 * Since we have only submitted 2 to HW, then there should be only 2
	 * ready for post processing.
	 */
	if (XAxiDma_BdRingFromHw(TxRingPtr, 1, &BdPtr) == 0) {
		AxiEthernetUtilErrorTrap("TxBDs were not ready for post processing");
		return XST_FAILURE;
	}

	/*
	 * Examine the TxBDs.
	 *
	 * There isn't much to do. The only thing to check would be DMA
	 * exception bits. But this would also be caught in the error handler.
	 * So we just return these BDs to the free list
	 */
	Status = XAxiDma_BdRingFree(TxRingPtr, 1, BdPtr);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error freeing up TxBDs");
		return XST_FAILURE;
	}

	/*
	 * Wait for Rx indication
	 */
	while (!FramesRx);

	/*
	 * Now that the frame has been received, post process our RxBD.
	 * Since we have only submitted 1 to HW, then there should be only 1
	 * ready for post processing.
	 */
	if (XAxiDma_BdRingFromHw(RxRingPtr, 1, &BdPtr) == 0) {
		AxiEthernetUtilErrorTrap("RxBD was not ready for post processing");
		return XST_FAILURE;
	}

	BdCurPtr = BdPtr;
	BdSts = XAxiDma_BdGetSts(BdCurPtr);
	if ((BdSts & XAXIDMA_BD_STS_ALL_ERR_MASK) ||
		(!(BdSts & XAXIDMA_BD_STS_COMPLETE_MASK))) {
			AxiEthernetUtilErrorTrap("Rx Error");
			return XST_FAILURE;
	}
	else {

		RxFrameLength =
		(XAxiDma_BdRead(BdCurPtr,XAXIDMA_BD_USR4_OFFSET)) & 0x0000FFFF;
	}

	/*
	 * This word has indication about what type of frame is received.
	 * Current hardware flags Broadcast, IP multicast, and MAC multicast.
	 * IP  multicast mode, MAC address is 01:00:5E:AB:CD:EF,
	 * MAC multicast mode, MAC address is 01:QR:ST:UV:WX:YZ, e.g.
	 * the least significant bit of most significant byte is 1.
	 * Check to see if further ethernet MAC verification is required...
	 */
	RxStatusControlWord =XAxiDma_BdRead(BdCurPtr,XAXIDMA_BD_USR2_OFFSET);
	if(RxStatusControlWord & XAE_BD_RX_USR2_IP_MCAST_MASK) {
	       /*
		* The multicast MAC address is stored in status words
		* 0 and 1 in the AXI4-Stream.
		*/

		Reg = XAxiDma_BdRead(BdPtr, XAXIDMA_BD_USR0_OFFSET);
		MulticastAdd[5] = ((Reg >> 8)  & 0xFF);
		MulticastAdd[4] = (Reg & 0xFF);

		Reg = XAxiDma_BdRead(BdPtr, XAXIDMA_BD_USR1_OFFSET);
		MulticastAdd[3] = ((Reg >> 24) & 0xFF);
		MulticastAdd[2] = ((Reg >> 16) & 0xFF);
		MulticastAdd[1] = ((Reg >> 8)  & 0xFF);
		MulticastAdd[0] = (Reg & 0xFF);

		if (!AxiEthernet_GetExtMulticast(MulticastAdd)) {
			AxiEthernetUtilErrorTrap("Multicast address mismatch\n");
			return XST_FAILURE;
		}
	}
	else {
		AxiEthernetUtilErrorTrap("Not a multicast frame\n");
		return XST_FAILURE;
	}

	/*
	 * Destination address(multicast MAC address in this example) is part
	 * of received frame, comparing the Tx and Rx frame would include the
	 * multicast address verification again.
	 */
	if (AxiEthernetUtilFrameVerify(&TxFrame, &RxFrame) != 0) {
		AxiEthernetUtilErrorTrap("Data mismatch");
		return XST_FAILURE;
	}

	/*
	 * Return the RxBD back to the channel for later allocation. Free the
	 * exact number we just post processed.
	 */
	Status = XAxiDma_BdRingFree(RxRingPtr, 1, BdPtr);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Error freeing up TxBDs");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


/*****************************************************************************/
/**
* AxiEthernet_AddExtMulticast adds the multicast Ethernet address table in
* DDR. This table is mainly for software driver to perform the final address
* filtering.
*
* Once an Ethernet address is programmed, the Axi Ethernet channel will begin
* receiving data sent from that address. The way to prevent the
* Axi Ethernet channel from receiving messages from an Ethernet address in the
* DDR table is to clear it with AxiEthernet_ClearExtMulticast().
*
* @param	AddressPtr is a pointer to the 6-byte Ethernet address to set.
*
* @return 	- XST_SUCCESS,on successful completion
*		- XST_INVALID_PARAM, if input MAC address is not between
*		01:00:5E:00:00:00 and 01:00:5E:7F:FF:FF, as per RFC1112.
* @note
*
* This table is independent to BRAM table that is used for HW address
* filtering. It is user's responsiblility to maintain these tables.
*
* In the multicast table, hardware requires it to be 'indexed' with bit 22-8,
* 15 bits in total of mac address. This address filtering is done in hardware
*  and provisioned with XAxiEthernet_[Add|Clear]ExtMulticastSet() APIs.
*
* This routine consider all 2**23 possible multicast ethernet addresses to be
* 8Mx1 bit or 1M bytes memory area. All defined multicast addresses are from
* 01.00.5E.00.00.00 to 01.00.5E.7F.FF.FF
* The first 25 bit out of 48 bit are static, so they will not be part of
* calculation.
*
* In memory, software used every bit represents every defined MAC address,
* then we have this formula for table lookup.
* offset + (address >> 3) => memory location.
* address % 32            => bit location for multicast address
* Let's take 01:00:5E:00:01:FF(hex),
* memory location : 0x3F
* bit location    : 0x1F
 *****************************************************************************/
static int AxiEthernet_AddExtMulticast(void *AddressPtr)
{
	u32 MaOffset;
	u32 Loc;
	u32 Bval;
	u32 BvalNew;
	u8  *Aptr = (u8 *) AddressPtr;

	/*
	 * Verify if address is a good/valid multicast address, between
	 * 01:00:5E:00:00:00 to 01:00:5E:7F:FF:FF per RFC1112.
	 * This address is referenced to be index to BRAM table.
	*/
	if ((0x01 != Aptr[0]) || (0x00 != Aptr[1]) || (0x5e != Aptr[2]) ||
		(0x0 != (Aptr[3] & 0x80))) {
		return (XST_INVALID_PARAM);
	}

	/* Program software Multicast table in RAM */
	/* Calculate memory locations in software table */
	MaOffset  = Aptr[5];
	MaOffset |= Aptr[4] << 8;
	MaOffset |= Aptr[3] << 16;

	Loc  = ((MaOffset >> 5) << 2); /* it is not same as ">> 3" */

	/* Bit value */
	BvalNew = 1 << (MaOffset % 32);

	/* Program software Multicast table in RAM */
	Bval = XAxiEthernet_ReadReg((u32)&McastAddressTable, Loc);
	XAxiEthernet_WriteReg((u32)&McastAddressTable, Loc, Bval | BvalNew);

	return (XST_SUCCESS);
}


/*****************************************************************************/
/**
* AxiEthernet_ClearExtMulticast clears the Ethernet address in multicast
* address table in DDR.
*
* @param AddressPtr is a pointer to the 6-byte Ethernet address to set.
*
* @return 	- XST_SUCCESS,on successful completion
*		- XST_INVALID_PARAM, if input MAC address is not between
*		01:00:5E:00:00:00 and 01:00:5E:7F:FF:FF, as per RFC1112.
*
* @note
*
* Please reference AxiEthernet_AddExtMulticast for multicast ethernet address
* index and bit value calculation.
*
*****************************************************************************/
static int AxiEthernet_ClearExtMulticast(void *AddressPtr)
{
	u32 MaOffset;
	u32 Loc;
	u32 Bval;
	u32 BvalNew;
	u8 *Aptr = (u8 *) AddressPtr;

	/*
	 * Verify if address is a good/valid multicast address, between
	 * 01:00:5E:00:00:00 to 01:00:5E:7F:FF:FF per RFC1112.
	 * This address is referenced to be index to BRAM table.
	 */

	if ((0x01 != Aptr[0]) || (0x00 != Aptr[1]) || (0x5e != Aptr[2]) ||
		(0x0 != (Aptr[3] & 0x80))) {
		return (XST_INVALID_PARAM);
	}

	/* Program software Multicast table in RAM */
	/* Calculate memory locations in software table */
	MaOffset  = Aptr[5];
	MaOffset |= Aptr[4] << 8;
	MaOffset |= Aptr[3] << 16;

	Loc  = ((MaOffset >> 5) << 2);

	/* Bit value */
	BvalNew = 1 << (MaOffset % 32);

	/* Program software Multicast table in RAM */
	Bval = XAxiEthernet_ReadReg((u32)&McastAddressTable, Loc);
	XAxiEthernet_WriteReg((u32)&McastAddressTable, Loc, Bval & ~BvalNew);

	return (XST_SUCCESS);
}


/*****************************************************************************/
/**
* AxiEthernet_GetExtMulticast verifies the Ethernet address in multicast
* address table in DDR.
*
* @param	AddressPtr is a pointer to the 6-byte Ethernet address to set.
*
* @return	-TRUE, if AddressPtr is provisioned.
*		-FALSE, if AddressPtr is not provisioned.
* @note
*
* Please reference AxiEthernet_AddExtMulticast for multicast ethernet address
* index and bit value calculation.
*
*****************************************************************************/
static int AxiEthernet_GetExtMulticast(void *AddressPtr)
{
	u32 MaOffset;
	u32 Loc;
	u32 Bval;
	u8 *Aptr = (u8 *) AddressPtr;


	/*
	 * Verify if address is a good/valid multicast address, between
	 * 01:00:5E:00:00:00 to 01:00:5E:7F:FF:FF per RFC1112.
	 * This address is referenced to be index to BRAM table.
	 */
	if ((0x01 != Aptr[0]) || (0x00 != Aptr[1]) || (0x5e != Aptr[2]) ||
		(0x0 != (Aptr[3] & 0x80))) {
		return (FALSE);
	}

	/* Program software Multicast table in RAM */
	/* Calculate memory locations in software table */
	MaOffset  = Aptr[5];
	MaOffset |= Aptr[4] << 8;
	MaOffset |= Aptr[3] << 16;

	Loc  = ((MaOffset >> 5) << 2);

	/* Program software Multicast table in RAM */
	Bval = XAxiEthernet_ReadReg((u32)&McastAddressTable, Loc);

	if (Bval & (1 << (MaOffset % 32))) {
		return (TRUE);
	} else {
		return (FALSE);
	}
}


/*****************************************************************************/
/**
*
* This is the DMA TX callback function to be called by TX interrupt handler.
* This function handles BDs finished by hardware.
*
* @param	TxRingPtr is a pointer to TX channel of the DMA engine.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void TxCallBack(XAxiDma_BdRing *TxRingPtr)
{
	/*
	 * Disable the transmit related interrupts
	 */
	XAxiDma_BdRingIntDisable(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);
	/*
	 * Increment the counter so that main thread knows something happened
	 */
	FramesTx++;
}

/*****************************************************************************/
/**
*
* This is the DMA TX Interrupt handler function.
*
* @param	TxRingPtr is a pointer to TX channel of the DMA engine.
*
* @return	None.
*
* @note		This Interrupt handler MUST clear pending interrupts before
*		handling them by calling the call back. Otherwise the following
*		corner case could raise some issue:
*
*		A packet got transmitted and a TX interrupt got asserted. If
*		the interrupt handler calls the callback before clearing the
*		interrupt, a new packet may get transmitted in the callback.
*		This new packet then can assert one more TX interrupt before
*		the control comes out of the callback function. Now when
*		eventually control comes out of the callback function, it will
*		never know about the second new interrupt and hence while
*		clearing the interrupts, would clear the new interrupt as well
*		and will never process it.
*		To avoid such cases, interrupts must be cleared before calling
*		the callback.
*
******************************************************************************/
static void TxIntrHandler(XAxiDma_BdRing *TxRingPtr)
{
	u32 IrqStatus;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_BdRingGetIrq(TxRingPtr);

	/* Acknowledge pending interrupts */
	XAxiDma_BdRingAckIrq(TxRingPtr, IrqStatus);

	/*
	 * If no interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		DeviceErrors++;
		AxiEthernetUtilErrorTrap
		("AXIDma: No interrupts asserted in TX status register");
		XAxiDma_Reset(&DmaInstance);
		if(!XAxiDma_ResetIsDone(&DmaInstance)) {
			AxiEthernetUtilErrorTrap ("AxiDMA: Error: Could not reset\n");
		}
		return;
	}

	/* If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		AxiEthernetUtilErrorTrap("AXIDMA: TX Error interrupts\n");
		/*
		 * Reset should never fail for transmit channel
		 */
		XAxiDma_Reset(&DmaInstance);

		if(!XAxiDma_ResetIsDone(&DmaInstance)) {

			AxiEthernetUtilErrorTrap ("AxiDMA: Error: Could not reset\n");
		}
		return;
	}

	/*
	 * If Transmit done interrupt is asserted, call TX call back function
	 * to handle the processed BDs and raise the according flag
	 */
	if ((IrqStatus & (XAXIDMA_IRQ_DELAY_MASK | XAXIDMA_IRQ_IOC_MASK))) {
		TxCallBack(TxRingPtr);
	}
}


/*****************************************************************************/
/**
*
* This is the DMA RX callback function to be called by RX interrupt handler.
* This function handles finished BDs by hardware, attaches new buffers to those
* BDs, and give them back to hardware to receive more incoming packets
*
* @param	RxRingPtr is a pointer to RX channel of the DMA engine.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void RxCallBack(XAxiDma_BdRing *RxRingPtr)
{
	/*
	 * Disable the receive related interrupts
	 */
	XAxiDma_BdRingIntDisable(RxRingPtr, XAXIDMA_IRQ_ALL_MASK);
	/*
	 * Increment the counter so that main thread knows something
	 * happened
	 */
	FramesRx++;
}


/*****************************************************************************/
/**
*
* This is the Receive handler function for examples 1 and 2.
* It will increment a shared  counter, receive and validate the frame.
*
* @param	RxRingPtr is a pointer to the DMA ring instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void RxIntrHandler(XAxiDma_BdRing *RxRingPtr)
{
	u32 IrqStatus;

	/* Read pending interrupts */
	IrqStatus = XAxiDma_BdRingGetIrq(RxRingPtr);

	/* Acknowledge pending interrupts */
	XAxiDma_BdRingAckIrq(RxRingPtr, IrqStatus);

	/*
	 * If no interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if (!(IrqStatus & XAXIDMA_IRQ_ALL_MASK)) {
		DeviceErrors++;
		AxiEthernetUtilErrorTrap
		("AXIDma: No interrupts asserted in RX status register");
		XAxiDma_Reset(&DmaInstance);
		if(!XAxiDma_ResetIsDone(&DmaInstance)) {
			AxiEthernetUtilErrorTrap ("Could not reset\n");
		}
		return;
	}

	/* If error interrupt is asserted, raise error flag, reset the
	 * hardware to recover from the error, and return with no further
	 * processing.
	 */
	if ((IrqStatus & XAXIDMA_IRQ_ERROR_MASK)) {

		AxiEthernetUtilErrorTrap("AXIDMA: RX Error interrupts\n");

		/* Reset could fail and hang
		 * NEED a way to handle this or do not call it??
		 */
		XAxiDma_Reset(&DmaInstance);

		if(!XAxiDma_ResetIsDone(&DmaInstance)) {
			AxiEthernetUtilErrorTrap ("Could not reset\n");
		}

		return;
	}

	/*
	 * If Reception done interrupt is asserted, call RX call back function
	 * to handle the processed BDs and then raise the according flag.
	 */
	if ((IrqStatus & (XAXIDMA_IRQ_DELAY_MASK | XAXIDMA_IRQ_IOC_MASK))) {
		RxCallBack(RxRingPtr);
	}
}

/*****************************************************************************/
/**
*
* This is the Error handler callback function and this function increments the
* the error counter so that the main thread knows the number of errors.
*
* @param	AxiEthernet is a reference to the Axi Ethernet device instance.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void AxiEthernetErrorHandler(XAxiEthernet *AxiEthernet)
{
	u32 Pending = XAxiEthernet_IntPending(AxiEthernet);

	if (Pending & XAE_INT_RXRJECT_MASK) {
		AxiEthernetUtilErrorTrap("AxiEthernet: Rx packet rejected");
	}

	if (Pending & XAE_INT_RXFIFOOVR_MASK) {
		AxiEthernetUtilErrorTrap("AxiEthernet: Rx fifo over run");
	}

	XAxiEthernet_IntClear(AxiEthernet, Pending);

	/*
	 * Bump counter
	 */
	DeviceErrors++;
}



/*****************************************************************************/
/**
*
* This function setups the interrupt system so interrupts can occur for the
* Axi Ethernet.  This function is application-specific since the actual system
* may or may not have an interrupt controller.  The Axi Ethernet could be
* directly connected to a processor without an interrupt controller.  The user
* should modify this function to fit the application.
*
* @param	IntcInstancePtr is a pointer to the instance of the Intc
*		component.
* @param	AxiEthernetInstancePtr is a pointer to the instance of the
*		AxiEthernet component.
* @param	DmaInstancePtr is a pointer to the instance of the AXI DMA
*		component.
* @param	AxiEthernetDeviceId is Device ID of the Axi Ethernet Device ,
*		typically XPAR_<AXIETHERNET_instance>_DEVICE_ID value from
*		xparameters.h.
* @param	AxiEthernetIntrId is the Interrupt ID and is typically
*		XPAR_<INTC_instance>_<AXIETHERNET_instance>_VEC_ID
*		value from xparameters.h.
* @param	DmaRxIntrId is the interrupt id for DMA Rx and is typically
*		taken from XPAR_<AXIETHERNET_instance>_CONNECTED_DMARX_INTR
* @param	DmaTxIntrId is the interrupt id for DMA Tx and is typically
*		taken from XPAR_<AXIETHERNET_instance>_CONNECTED_DMATX_INTR
*
* @return	- XST_SUCCESS, if successful.
*		- XST_FAILURE, if failure condition.
*
* @note		None.
*
******************************************************************************/
static int AxiEthernetSetupIntrSystem(INTC *IntcInstancePtr,
				XAxiEthernet *AxiEthernetInstancePtr,
				XAxiDma *DmaInstancePtr,
				u16 AxiEthernetIntrId,
				u16 DmaRxIntrId,
				u16 DmaTxIntrId)
{
    printf("AxiEthernetSetupIntrSystem\r\n");
	XAxiDma_BdRing * TxRingPtr = XAxiDma_GetTxRing(DmaInstancePtr);
	XAxiDma_BdRing * RxRingPtr = XAxiDma_GetRxRing(DmaInstancePtr);
	int Status;

	/*
	 * Initialize the interrupt controller and connect the ISR
	 */
    // TODO: fixme
	Status = PLIC_register_interrupt_handler(IntcInstancePtr, AxiEthernetIntrId,
							(XInterruptHandler)
							AxiEthernetErrorHandler,
							AxiEthernetInstancePtr);
    if (Status == 0) {
        printf("Unable to connect ISR to interrupt controller: AxiEthernetIntrId %u\r\n",AxiEthernetIntrId);
        return XST_FAILURE;
    }
	Status = PLIC_register_interrupt_handler(IntcInstancePtr, DmaTxIntrId,
						(XInterruptHandler) TxIntrHandler,
									TxRingPtr);
    if (Status == 0) {
        printf("Unable to connect ISR to interrupt controller: DmaTxIntrId %u\r\n",DmaTxIntrId);
        return XST_FAILURE;
    }
	Status = PLIC_register_interrupt_handler(IntcInstancePtr, DmaRxIntrId,
						(XInterruptHandler) RxIntrHandler,
								RxRingPtr);
    if (Status == 0) {
        printf("Unable to connect ISR to interrupt controller: DmaRxIntrId %u\r\n",DmaRxIntrId);
        return XST_FAILURE;
    }

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function disables the interrupts that occur for Axi Ethernet.
*
* @param	IntcInstancePtr is a pointer to the instance of the Intc
*		component.
* @param	AxiEthernetIntrId is the Interrupt ID and is typically
*		XPAR_<INTC_instance>_<AXIETHERNET_instance>_VEC_ID
*		value from xparameters.h.
* @param	DmaRxIntrId is the interrupt id for DMA Rx and is typically
*		taken from XPAR_<AXIETHERNET_instance>_CONNECTED_DMARX_INTR
* @param	DmaTxIntrId is the interrupt id for DMA Tx and is typically
*		taken from XPAR_<AXIETHERNET_instance>_CONNECTED_DMATX_INTR
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void AxiEthernetDisableIntrSystem(INTC *IntcInstancePtr,
					u16 AxiEthernetIntrId,
					u16 DmaRxIntrId,
					u16 DmaTxIntrId)
{
	/*
	 * Disconnect the interrupts for the DMA TX and RX channels
	 */
    PLIC_unregister_interrupt_handler(IntcInstancePtr, DmaTxIntrId);
	PLIC_unregister_interrupt_handler(IntcInstancePtr, DmaRxIntrId);

	/*
	 * Disconnect and disable the interrupt for the Axi Ethernet device
	 */
    PLIC_unregister_interrupt_handler(IntcInstancePtr, AxiEthernetIntrId);
}

