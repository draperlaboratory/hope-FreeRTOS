/*
FreeRTOS+TCP V2.0.11
Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 http://aws.amazon.com/freertos
 http://www.FreeRTOS.org
*/

/* Standard includes. */
#include <stdint.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "FreeRTOS_IP.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_UDP_IP.h"
#include "FreeRTOS_Sockets.h"
#include "NetworkBufferManagement.h"

/* FreeRTOS+TCP includes. */
#include "NetworkInterface.h"

/* Board specific includes */
#include "riscv_hal_eth.h"


/* xTXDescriptorSemaphore is a counting semaphore with
a maximum count of ETH_TXBUFNB, which is the number of
DMA TX descriptors. */
static SemaphoreHandle_t xTXDescriptorSemaphore = NULL;

/* First statically allocate the buffers, ensuring an additional ipBUFFER_PADDING
bytes are allocated to each buffer.  This example makes no effort to align
the start of the buffers, but most hardware will have an alignment requirement.
If an alignment is required then the size of each buffer must be adjusted to
ensure it also ends on an alignment boundary.  Below shows an example assuming
the buffers must also end on an 8-byte boundary. */
#define BUFFER_SIZE ( ipTOTAL_ETHERNET_FRAME_SIZE + ipBUFFER_PADDING )
#define BUFFER_SIZE_ROUNDED_UP ( ( BUFFER_SIZE + 7 ) & ~0x07UL )
static uint8_t ucBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ][ BUFFER_SIZE_ROUNDED_UP ] __attribute__ ((section(".uncached"))) = {0};

BaseType_t xNetworkInterfaceInitialise( void )
{
	FreeRTOS_debug_printf( ("xNetworkInterfaceInitialise\r\n") );

	/* Init counting semaphore*/
	if( xTXDescriptorSemaphore == NULL )
	{
		xTXDescriptorSemaphore = xSemaphoreCreateCounting( ( UBaseType_t ) TXBD_CNT, ( UBaseType_t ) TXBD_CNT );
		configASSERT( xTXDescriptorSemaphore );
	}

	// Init DMA
	configASSERT(DmaSetup(&AxiDmaInstance, XPAR_AXIDMA_0_DEVICE_ID) == 0);

	// Init Ethernet
	configASSERT(PhySetup(&AxiEthernetInstance, XPAR_AXIETHERNET_0_DEVICE_ID) == 0);

	// Connect to PLIC
	configASSERT(AxiEthernetSetupIntrSystem(&Plic, &AxiEthernetInstance, &AxiDmaInstance, PLIC_SOURCE_ETH, XPAR_AXIETHERNET_0_DMA_RX_INTR,
						XPAR_AXIETHERNET_0_DMA_TX_INTR) == 0);
	
	uint16_t Speed;
	configASSERT( XAxiEthernet_GetSgmiiStatus(&AxiEthernetInstance, &Speed) == 0);
    FreeRTOS_debug_printf( ("xNetworkInterfaceInitialise: XAxiEthernet_GetSgmiiStatus returned %u\r\n",Speed) );

	/*
	 * Start the Axi Ethernet and enable its ERROR interrupts
	 */
	XAxiEthernet_Start(&AxiEthernetInstance);
	XAxiEthernet_IntEnable(&AxiEthernetInstance, XAE_INT_RECV_ERROR_MASK);

	FreeRTOS_debug_printf( ("xNetworkInterfaceInitialise: Going to sleep for %u seconds\r\n", AXIETHERNET_PHY_DELAY_SEC) );
	vTaskDelay(pdMS_TO_TICKS(AXIETHERNET_PHY_DELAY_SEC*1000)); // sleep for 5 s
	FreeRTOS_debug_printf( ("xNetworkInterfaceInitialise: Woken up\r\n") );

	return pdPASS;
}
/*-----------------------------------------------------------*/

BaseType_t xNetworkInterfaceOutput( NetworkBufferDescriptor_t * const pxNetworkBuffer, BaseType_t xReleaseAfterSend )
{
	FreeRTOS_debug_printf( ("xNetworkInterfaceOutput: FramesTX = %i\r\n", FramesTx) );
	FreeRTOS_debug_printf( ("xNetworkInterfaceOutput: FramesRX: %i\r\n", FramesRx));

	/* unused, we always release after send */
	(void) xReleaseAfterSend;

	/* get BD ring descriptor */
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(&AxiDmaInstance);
	XAxiDma_Bd * BdPtr;

	//XAxiDma_BdRingIntEnable(TxRingPtr, XAXIDMA_IRQ_ALL_MASK);

	/* allocate next BD from the BD ring */
	configASSERT( XAxiDma_BdRingAlloc(TxRingPtr, 1, &BdPtr) == 0);

	/* configure BD */
	XAxiDma_BdSetBufAddr(BdPtr, (u32)pxNetworkBuffer->pucEthernetBuffer);
	XAxiDma_BdSetLength(BdPtr, pxNetworkBuffer->xDataLength,TxRingPtr->MaxTransferLen);
	XAxiDma_BdSetCtrl(BdPtr, XAXIDMA_BD_CTRL_TXSOF_MASK |
			     XAXIDMA_BD_CTRL_TXEOF_MASK);
	
	/* pass BD to HW */
	configASSERT( XAxiDma_BdRingToHw(TxRingPtr, 1, BdPtr) == 0 );

	/* start transaction */
	configASSERT( XAxiDma_BdRingStart(TxRingPtr) == 0);

	/* Call the standard trace macro to log the send event. */
    iptraceNETWORK_INTERFACE_TRANSMIT();

	return pdPASS;
}
/*-----------------------------------------------------------*/

/* Next provide the vNetworkInterfaceAllocateRAMToBuffers() function, which
simply fills in the pucEthernetBuffer member of each descriptor. */
void vNetworkInterfaceAllocateRAMToBuffers( NetworkBufferDescriptor_t pxNetworkBuffers[ ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS ] )
{
	FreeRTOS_debug_printf( ("vNetworkInterfaceAllocateRAMToBuffers\r\n") );
	BaseType_t x;
    for( x = 0; x < ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS; x++ )
    {
        /* pucEthernetBuffer is set to point ipBUFFER_PADDING bytes in from the
        beginning of the allocated buffer. */
        pxNetworkBuffers[ x ].pucEthernetBuffer = &( ucBuffers[ x ][ ipBUFFER_PADDING ] );

        /* The following line is also required, but will not be required in
        future versions. */
		// TODO: if this works, add "#pragma GCC diagnostic ignored "-Wcast-align" to supress the warning
        *( ( uint32_t * ) &ucBuffers[ x ][ 0 ] ) = ( uint32_t ) &( pxNetworkBuffers[ x ] );
    }
}
/*-----------------------------------------------------------*/

BaseType_t xGetPhyLinkStatus( void )
{
	FreeRTOS_debug_printf( ("xGetPhyLinkStatus\r\n") );

	if (PhyLinkStatus(&AxiEthernetInstance)) {
		FreeRTOS_debug_printf( ("xGetPhyLinkStatus: Link is UP\r\n") );
		return pdPASS;
	} else {
		FreeRTOS_debug_printf( ("xGetPhyLinkStatus: Link is DOWN\r\n") );
		return pdFAIL;
	}
}
/*-----------------------------------------------------------*/