/******************************************************************************
*
* Copyright (C) 2017 - 2018 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/
/*****************************************************************************/
/**
* @file xaxiethernet_mcdma_ping_req_example.c
*
* This file contains a Axi Ethernet MCDMA Ping request example in polled mode.
* This example will generate a ping request for the specified IP address.
*
* @note
*
* The local IP address is set to 10.10.70.6. User needs to update
* LocalIpAddr variable with a free IP address based on the network on which
* this example is to be run.
*
* The Destination IP address is set to 10.10.70.3. User needs to update
* DestIpAddress variable with any valid IP address based on the network on which
* this example is to be run.
*
* The local MAC address is set to 0x000A35030201. User can update LocalMacAddr
* variable with a valid MAC address. The first three bytes contains
* the manufacture ID. 0x000A35 is XILINX manufacture ID.
*
* This program will generate the specified number of ping request packets as
* defined in "NUM_OF_PING_REQ_PKTS".
*
* This example got validated only for SGMII based design's.
*
* Functional guide to example:
*
* - SendArpReqFrame demonstrates the way to send the ARP request packets
*   in the polling mode
* - SendEchoReqFrame demonstrates the way to send the ICMP/ECHO request packets
*   in the polling mode
* - ProcessRecvFrame demonstrates the way to process the received packet.
*   This function sends the echo request packet based on the ARP reply packet.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 5.5   adk		Initial Release
* </pre>
*
*****************************************************************************/
/***************************** Include Files *********************************/
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

#include "xstatus.h"
#include "xil_io.h"
#include "xaxiethernet.h"	/* defines Axi Ethernet APIs */
#include "xaxiethernet_example.h" /* demo includes */

/*************************** Constant Definitions ****************************/

//#define PAYLOAD_SIZE		1000	/* Payload size used in examples */
//#define BD_RX_STATUS_OFFSET	2

/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define AXIETHERNET_DEVICE_ID	XPAR_AXIETHERNET_0_DEVICE_ID
#define AXIMCDMA_DEVICE_ID	XPAR_AXIDMA_0_DEVICE_ID

/*
 * Change this parameter to limit the number of ping requests sent by this
 * program.
 */
#define NUM_OF_PING_REQ_PKTS	10	/* Number of ping req it generates */

#define RXBD_CNT			1024	/* Number of RxBDs to use */
#define TXBD_CNT			1024	/* Number of TxBDs to use */
#define BD_ALIGNMENT			64	/* Byte alignment of BDs */

#define ECHO_REPLY		0x00	/* Echo reply */
#define HW_TYPE			0x01	/* Hardware type (10/100 Mbps) */
#define ARP_REQUEST 		0x01	/* ARP Request bits in Rx packet */
#define ARP_REPLY 		0x02 	/* ARP status bits indicating reply */
#define IDEN_NUM		0x02	/* ICMP identifier number */
#define IP_VERSION		0x0604	/* IP version ipv4/ipv6 */
#define BROADCAST_ADDR 		0xFFFF 	/* Broadcast Address */
#define CORRECT_CHECKSUM_VALUE	0xFFFF	/* Correct checksum value */
#define ARP_REQ_PKT_SIZE	0x2A	/* ARP request packet size */
#define ICMP_PKT_SIZE 		0x4A	/* ICMP packet length 74 Bytes
					including Src and dest MAC Add */
#define IP_ADDR_SIZE		4	/* IP Address size in Bytes */
#define NUM_RX_PACK_CHECK_REQ	10	/* Max num of Rx pack to be checked
					before sending another request */
#define NUM_PACK_CHECK_RX_PACK	8000000	/* Max number of pack to be checked
					before to identify a Rx packet */
#define DELAY			1000000 /* Used to introduce delay */

/*
 * Definitions for the locations and length of some of the fields in a
 * IP packet. The lengths are defined in Half-Words (2 bytes).
 */

#define SRC_MAC_ADDR_LOC	3	/* Src MAC address location */
#define MAC_ADDR_LEN 		3	/* MAC address length */
#define ETHER_PROTO_TYPE_LOC	6	/* Ethernet Proto type loc */

#define IP_ADDR_LEN 		2	/* Size of IP address */
#define IP_START_LOC 		7	/* IP header start location */
#define IP_HEADER_INFO_LEN	7	/* IP header information length */
#define IP_HEADER_LEN 		10 	/* IP header length */
#define IP_CHECKSUM_LOC		12	/* IP header checksum location */
#define IP_REQ_SRC_IP_LOC 	13	/* Src IP add loc of ICMP req */
#define IP_REQ_DEST_IP_LOC	15	/* Dest IP add loc of ICMP req */

#define ICMP_KNOWN_DATA_LEN	16	/* ICMP known data length */
#define ICMP_ECHO_FIELD_LOC 	17	/* Echo field loc */
#define ICMP_DATA_START_LOC 	17	/* Data field start location */
#define ICMP_DATA_LEN 		18	/* ICMP data length */
#define ICMP_DATA_CHECKSUM_LOC	18	/* ICMP data checksum location */
#define ICMP_IDEN_FIELD_LOC	19	/* Identifier field loc */
#define ICMP_DATA_LOC 		19	/* ICMP data loc including
					identifier number and sequence number */
#define ICMP_SEQ_NO_LOC		20	/* sequence number location */
#define ICMP_DATA_FIELD_LEN 	20 	/* Data field length */
#define ICMP_KNOWN_DATA_LOC	21	/* ICMP known data start loc */

#define ARP_REQ_STATUS_LOC 	10	/* ARP request loc */
#define ARP_REQ_SRC_IP_LOC 	14	/* Src IP add loc of ARP req Packet */

//#define RXBD_SPACE_BYTES RXBD_CNT * 64 * 16
//#define TXBD_SPACE_BYTES TXBD_CNT * 64 * 16

/*
 * General Ethernet Definitions
 */
#define XAE_ETHER_PROTO_TYPE_IP         0x0800  /**< IP Protocol */
#define XAE_ETHER_PROTO_TYPE_ARP        0x0806  /**< ARP Protocol */
#define XAE_ETHER_PROTO_TYPE_VLAN       0x8100  /**< VLAN Tagged */
#define XAE_ARP_PACKET_SIZE             28      /**< Max ARP packet size */
#define XAE_HEADER_IP_LENGTH_OFFSET     16      /**< IP Length Offset */
#define XAE_VLAN_TAG_SIZE               4       /**< VLAN Tag Size */


#define PHY_R0_CTRL_REG		0
#define PHY_R3_PHY_IDENT_REG	3

#define PHY_R0_RESET         0x8000
#define PHY_R0_LOOPBACK      0x4000
#define PHY_R0_ANEG_ENABLE   0x1000
#define PHY_R0_DFT_SPD_MASK  0x2040
#define PHY_R0_DFT_SPD_10    0x0000
#define PHY_R0_DFT_SPD_100   0x2000
#define PHY_R0_DFT_SPD_1000  0x0040
#define PHY_R0_ISOLATE       0x0400

/* Marvel PHY 88E1111 Specific definitions */
#define PHY_R20_EXTND_CTRL_REG	20
#define PHY_R27_EXTND_STS_REG	27

#define PHY_R20_DFT_SPD_10    	0x20
#define PHY_R20_DFT_SPD_100   	0x50
#define PHY_R20_DFT_SPD_1000  	0x60
#define PHY_R20_RX_DLY		0x80

#define PHY_R27_MAC_CONFIG_GMII      0x000F
#define PHY_R27_MAC_CONFIG_MII       0x000F
#define PHY_R27_MAC_CONFIG_RGMII     0x000B
#define PHY_R27_MAC_CONFIG_SGMII     0x0004

/* Marvel PHY 88E1116R Specific definitions */
#define PHY_R22_PAGE_ADDR_REG	22
#define PHY_PG2_R21_CTRL_REG	21

#define PHY_REG21_10      0x0030
#define PHY_REG21_100     0x2030
#define PHY_REG21_1000    0x0070

/* Marvel PHY flags */
#define MARVEL_PHY_88E1111_MODEL	0xC0
#define MARVEL_PHY_88E1116R_MODEL	0x240
#define PHY_MODEL_NUM_MASK		0x3F0

/* TI PHY flags */
#define TI_PHY_IDENTIFIER		0x2000
#define TI_PHY_MODEL			0x230
#define TI_PHY_CR			0xD
#define TI_PHY_PHYCTRL			0x10
#define TI_PHY_CR_SGMII_EN		0x0800
#define TI_PHY_ADDDR			0xE
#define TI_PHY_CFGR2			0x14
#define TI_PHY_SGMIITYPE		0xD3
#define TI_PHY_CFGR2_SGMII_AUTONEG_EN	0x0080
#define TI_PHY_SGMIICLK_EN		0x4000
#define TI_PHY_CR_DEVAD_EN		0x001F
#define TI_PHY_CR_DEVAD_DATAEN		0x4000


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

#define INTC plic_instance_t
#define XPAR_AXIDMA_0_ENABLE_MULTI_CHANNEL 1 // assuming we have a multi channel DMA

// we don't have any cache, so we don't need to do anything about it
#define Xil_DCacheFlushRange(...)
#define Xil_DCacheInvalidateRange(...)

/*
 * Set up a local MAC address.
 */
static u8 LocalMacAddr[6] =
{
	0x00, 0x0A, 0x35, 0x03, 0x02, 0x01

};

/*
 * The IP address was set to 172.16.63.121. User need to set a free IP address
 * based on the network on which this example is to be run.
 */
static u8 LocalIpAddress[IP_ADDR_SIZE] =
{
	10, 88, 88, 3
};

/*
 * Set up a Destination IP address. Currently it is set to 172.16.63.61.
 */
static u8 DestIpAddress[IP_ADDR_SIZE] =
{
	10, 88, 88, 2
};

static u16 DestMacAddr[MAC_ADDR_LEN]; 	/* Destination MAC Address */

/*
 * Known data transmitted in Echo request.
 */
u16 IcmpData[ICMP_KNOWN_DATA_LEN] =
{
	0x6162,	0x6364,	0x6566, 0x6768, 0x696A,	0x6B6C, 0x6D6E,	0x6F70,
	0x7172, 0x7374, 0x7576, 0x7761, 0x6263,	0x6465, 0x6667,	0x6869
};

/*
 * IP header information -- each field has its own significance.
 * Icmp type, ipv4 typelength, packet length, identification field
 * Fragment type, time to live and ICM, checksum.
 */
u16 IpHeaderInfo[IP_HEADER_INFO_LEN] =
{
	0x0800,	0x4500, 0x003C,	0x5566,	0x0000,	0x8001, 0x0000
};

static int RxBdSetup(XAxiDma *McDmaInstPtr, XAxiEthernet *AxiEthernetInstancePtr);
static int TxBdSetup(XAxiDma *McDmaInstPtr, XAxiEthernet *AxiEthernetInstancePtr);

char RxBdSpace[RXBD_SPACE_BYTES] __attribute__ ((aligned(BD_ALIGNMENT)));
char TxBdSpace[TXBD_SPACE_BYTES] __attribute__ ((aligned(BD_ALIGNMENT)));

/*
 * Variable used to indicate the length of the received frame.
 */
volatile u32 RecvFrameLength;
UINTPTR TxBuffPtr;
UINTPTR RxBuffPtr;
volatile int TxCount;
volatile int RxCount;
volatile int Padding;	/* For 1588 Packets we need to pad 8 bytes time stamp value */

/*
 * Variable used to indicate the sequence number of the ICMP(echo) packet.
 */
int SeqNum;

/*
 * Variable used to indicate the number of ping request packets to be send.
 */
int NumOfPingReqPkts;
/* MCDMA Channel to Operate on */
int Mcdma_ChanId = 1;


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
/*
 * Interrupt setup and Callbacks for examples
 */

static int AxiEthernetSetupIntrSystem(INTC *IntcInstancePtr,
				XAxiEthernet *AxiEthernetInstancePtr,
				XAxiDma *DmaInstancePtr,
				u16 AxiEthernetIntrId,
				u16 DmaRxIntrId, u16 DmaTxIntrId);

/*
 * Called by main when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1 in
 * main.c.
 */
void main_ping( void );

/**
 * Modified ethernet_multicast_example
 */
static void prvDriverTestTask( void *pvParameters );

int AxiEthernetPingReqExample(INTC *IntcInstancePtr,
				XAxiEthernet *AxiEthernetInstancePtr,
				XAxiDma *DmaInstancePtr,
				u16 AxiEthernetDeviceId,
				u16 AxiDmaDeviceId,
				u16 AxiEthernetIntrId,
				u16 DmaRxIntrId,
				u16 DmaTxIntrId);

void SendArpReqFrame(XAxiEthernet *AxiEthernetInstancePtr,
		     XAxiDma *DmaInstancePtr, u8 ChanId);

void SendEchoReqFrame(XAxiEthernet *AxiEthernetInstancePtr,
		      XAxiDma *DmaInstancePtr, u8 ChanId);

int ProcessRecvFrame(XAxiEthernet *AxiEthernetInstancePtr,
		     XAxiDma *DmaInstancePtr, u8 ChanId);

int XAxienet_Send(XAxiEthernet *AxiEthernetInstancePtr, XAxiDma *DmaInstancePtr, u8 ChanId,
		UINTPTR TxFrame, u32 len);
int XAxiEnet_Recv(XAxiDma *DmaInstancePtr);
int PhySetup(XAxiEthernet *AxiEthernetInstancePtr);

int RxSetup(XAxiDma *McDmaInstPtr, XAxiEthernet *AxiEthernetInstancePtr,
			u8 ChanId, u32 NumPkts);

static u16 CheckSumCalculation(u16 *RxFramePtr16, int StartLoc, int Length);

static int CompareData(u16 *LhsPtr, u16 *RhsPtr, int LhsLoc, int RhsLoc,
			int Count);

/*-----------------------------------------------------------*/
void main_ping( void )
{
	xTaskCreate( prvDriverTestTask, "Driver_test_task", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL );

	/* Start the tasks and timer running. */
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

static void prvDriverTestTask( void *pvParameters ) {
    (void) pvParameters;
    int Status;

    AxiEthernetUtilErrorTrap("\r\n--- Enter main() ---");
	AxiEthernetUtilErrorTrap("This test may take several minutes to finish");

	/*
	 * Call the Axi Ethernet multicast example.
	 */
	Status = AxiEthernetPingReqExample(&Plic,
						&AxiEthernetInstance,
						&DmaInstance,
						XPAR_AXIETHERNET_0_DEVICE_ID,
						XPAR_AXIDMA_0_DEVICE_ID,
						PLIC_SOURCE_ETH,
						XPAR_AXIETHERNET_0_DMA_RX_INTR,
						XPAR_AXIETHERNET_0_DMA_TX_INTR);
	if (Status != XST_SUCCESS) {
		AxiEthernetUtilErrorTrap("Axiethernet Ping Example Failed\r\n");
		AxiEthernetUtilErrorTrap("--- Exiting main() ---");
		// TODO: handle better
        __asm volatile( "ebreak" );
	}

	AxiEthernetUtilErrorTrap("Successfully ran Axiethernet ping Example\r\n");
	AxiEthernetUtilErrorTrap("--- Exiting main() ---");

    while(1) {
        ;;
    }
}




int PhySetup(XAxiEthernet *AxiEthernetInstancePtr)
{
	u16 PhyReg0;
	signed int PhyAddr;
	u16 status;

	PhyAddr = XPAR_AXIETHERNET_0_PHYADDR;

	/* Clear the PHY of any existing bits by zeroing this out */
	PhyReg0 = 0;
	XAxiEthernet_PhyRead(AxiEthernetInstancePtr, PhyAddr,
				 PHY_R0_CTRL_REG, &PhyReg0);

	PhyReg0 &= (~PHY_R0_ANEG_ENABLE);
	PhyReg0 &= (~PHY_R0_ISOLATE);
	PhyReg0 |= PHY_R0_DFT_SPD_1000;

	sleep(1);
	XAxiEthernet_PhyWrite(AxiEthernetInstancePtr, PhyAddr,
				PHY_R0_CTRL_REG, PhyReg0);

	XAxiEthernet_PhyRead(AxiEthernetInstancePtr, PhyAddr, 1, &status);

	return XST_SUCCESS;
}

/**
*
* The entry point for the AxiEthernet driver to ping request example in polled
* mode. This function will generate specified number of request packets as
* defined in "NUM_OF_PING_REQ_PKTS.
*
* @param	AxiEthernetInstancePtr is a pointer to the instance of the
*		AxiEthernet component.
* @param	DmaInstancePtr is a pointer to the instance of the AXI MCDMA
*		component.
* @param	AxiEthernetDeviceId is Device ID of the Axi Ethernet Device ,
*		typically XPAR_<AXIETHERNET_instance>_DEVICE_ID value from
*		xparameters.h.
* @param	AxiMcDmaDeviceId is Device ID of the Axi DMAA Device ,
*		typically XPAR_<AXIMCDMA_instance>_DEVICE_ID value from
*		xparameters.h.
*
* @return	XST_FAILURE to indicate failure, otherwise it will return
*		XST_SUCCESS.
*
* @note		AxiMcdma hardware must be initialized before initializing
*		AxiEthernet. Since AxiMcdma reset line is connected to the
*		AxiEthernet reset line, a reset of AxiMcdma hardware during its
*		initialization would reset AxiEthernet.
*
******************************************************************************/
int AxiEthernetPingReqExample(INTC *IntcInstancePtr,
				XAxiEthernet *AxiEthernetInstancePtr,
				XAxiDma *DmaInstancePtr,
				u16 AxiEthernetDeviceId,
				u16 AxiDmaDeviceId,
				u16 AxiEthernetIntrId,
				u16 DmaRxIntrId,
				u16 DmaTxIntrId)
{
	int Status;
	//int Index;
	//int Count;
	//int EchoReplyStatus;
	XAxiEthernet_Config *MacCfgPtr;
	XAxiDma_Config* DmaConfig;
	SeqNum = 0;
	RecvFrameLength = 0;
	//EchoReplyStatus = XST_FAILURE;
	NumOfPingReqPkts = NUM_OF_PING_REQ_PKTS;

	XAxiDma_BdRing *RxRingPtr = XAxiDma_GetRxRing(DmaInstancePtr);
	XAxiDma_BdRing *TxRingPtr = XAxiDma_GetTxRing(DmaInstancePtr);
	XAxiDma_Bd BdTemplate;

	/*
	 *  Get the configuration of AxiEthernet hardware.
	 */
	MacCfgPtr = XAxiEthernet_LookupConfig(AxiEthernetDeviceId);

	/*
	 * Check whether MCDMA is present or not
	 */
	if(MacCfgPtr->AxiDevType != XPAR_AXI_DMA) {
		printf
			("Device HW not configured for MCDMA mode\r\n");
		return XST_FAILURE;
	}

	DmaConfig = XAxiDma_LookupConfig(AxiDmaDeviceId);

	/*
	 * Initialize AXIMCDMA engine. AXIMCDMA engine must be initialized before
	 * AxiEthernet. During AXIMCDMA engine initialization, AXIMCDMA hardware is
	 * reset, and since AXIMCDMA reset line is connected to AxiEthernet, this
	 * would ensure a reset of AxiEthernet.
	 */
	Status = XAxiDma_CfgInitialize(DmaInstancePtr, DmaConfig);
	if(Status != XST_SUCCESS) {
		printf("Error initializing DMA\r\n");
		return XST_FAILURE;
	}

	printf("DMA init success\r\n");

	/*
	 * Initialize AxiEthernet hardware.
	 */
	Status = XAxiEthernet_CfgInitialize(AxiEthernetInstancePtr, MacCfgPtr,
					MacCfgPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		printf("Error in initialize");
		return XST_FAILURE;
	}

	if (MacCfgPtr->Enable_1588)
		Padding = 8;

	printf("Cfg init success\n\r");
	TxCount = 0;
	RxCount = 0;
	TxBuffPtr = (UINTPTR) &TxFrame;
	RxBuffPtr = (UINTPTR) &RxFrame;

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
	 * Set the MAC address
	 */
	Status = XAxiEthernet_SetMacAddress(AxiEthernetInstancePtr,
					LocalMacAddr);
	if (Status != XST_SUCCESS) {
	      printf("Error setting MAC address");
	      return XST_FAILURE;
	}

	PhySetup(AxiEthernetInstancePtr);

	/*
	 * Setting the operating speed of the MAC needs a delay.  There
	 * doesn't seem to be register to poll, so please consider this
	 * during your application design.
	 */
	 sleep(2);

	/*
	 * Make sure Tx and Rx are enabled
	 */
	Status = XAxiEthernet_SetOptions(AxiEthernetInstancePtr,
				         XAE_RECEIVER_ENABLE_OPTION |
					 XAE_TRANSMITTER_ENABLE_OPTION);
	if (Status != XST_SUCCESS) {
		printf("Error setting options");
		return XST_FAILURE;
	}


	/*
	 * Connect to the interrupt controller and enable interrupts
	 */
	Status = AxiEthernetSetupIntrSystem(IntcInstancePtr,
			AxiEthernetInstancePtr, DmaInstancePtr,
			AxiEthernetIntrId, DmaRxIntrId, DmaTxIntrId);


	/*
	 * Start the Axi Ethernet and enable its ERROR interrupts
	 */
	XAxiEthernet_Start(AxiEthernetInstancePtr);

	/*
	 * Empty any existing receive frames.
	 */
	// while (NumOfPingReqPkts--) {

	// 	/*
	// 	 * Introduce delay.
	// 	 */
	// 	Count = DELAY;
	// 	while (Count--) {
	// 	}

	// 	/*
	// 	 * Send an ARP or an ICMP packet based on receive packet.
	// 	 */
	// 	if (SeqNum == 0) {
	// 		SendArpReqFrame(AxiEthernetInstancePtr, DmaInstancePtr, Mcdma_ChanId);
	// 	} else {
	// 		SendEchoReqFrame(AxiEthernetInstancePtr, DmaInstancePtr, Mcdma_ChanId);
	// 	}

	// 	/*
	// 	 * Check next 10 packets for the correct reply.
	// 	 */
	// 	Index = NUM_RX_PACK_CHECK_REQ;
	// 	while (Index--) {

	// 		/*
	// 		 * Wait for a Receive packet.
	// 		 */
	// 		Count = NUM_PACK_CHECK_RX_PACK;
	// 		while (RecvFrameLength == 0) {
	// 			RecvFrameLength = XAxiEnet_Recv(DmaInstancePtr);
	// 			/*
	// 			 * To avoid infinite loop when no packet is
	// 			 * received.
	// 			 */
	// 			if (Count-- == 0) {
	// 				break;
	// 			}
	// 		}

	// 		/*
	// 		 * Process the Receive frame.
	// 		 */
	// 		if (RecvFrameLength != 0) {
	// 			EchoReplyStatus = ProcessRecvFrame(
	// 					AxiEthernetInstancePtr, DmaInstancePtr, Mcdma_ChanId);
	// 		}
	// 		RecvFrameLength = 0;

	// 		/*
	// 		 * Comes out of loop when an echo reply packet is
	// 		 * received.
	// 		 */
	// 		if (EchoReplyStatus == XST_SUCCESS) {
	// 			break;
	// 		}
	// 	}

	// 	/*
	// 	 * If no echo reply packet is received, it reports
	// 	 * request timed out.
	// 	 */
	// 	if (EchoReplyStatus == XST_FAILURE) {
	// 		printf("Packet No: %d",
	// 			NUM_OF_PING_REQ_PKTS - NumOfPingReqPkts);
	// 		printf(" Seq NO %d Request timed out\r\n",
	// 						SeqNum);
	// 	}
	// }
	return XST_SUCCESS;
}




// int XAxiEnet_Recv(XAxiDma *InstancePtr)
// {
// 	 u16 Chan_id = 1;
// 	 u32 i, Chan_SerMask;
// 	 u32 len = 0;

// 	 Chan_SerMask = XMcdma_ReadReg(InstancePtr->Config.BaseAddress,
// 	                               XMCDMA_RX_OFFSET + XMCDMA_RXCH_SER_OFFSET);

// 	 for (i = 1, Chan_id = 1; i != 0 && i <= Chan_SerMask; i <<= 1, Chan_id++)
// 	       if (Chan_SerMask & i)
// 	            len = PollRxData(InstancePtr, Chan_id);

// 	  return len;
// }


// int XAxienet_Send(XAxiEthernet *AxiEthernetInstancePtr, XAxiDma *DmaInstancePtr,
// 		  u8 ChanId, UINTPTR TxBufPtr, u32 len)
// {
// 	XMcdma_ChanCtrl *Chan;
// 	XMcdma_Bd *BdCurPtr;
// 	int Status;

// 	Xil_DCacheFlushRange((UINTPTR)TxBufPtr, len + Padding);
// 	Chan = XMcdma_GetMcdmaTxChan(DmaInstancePtr, ChanId);
// 	BdCurPtr = (XMcdma_Bd *)XMcdma_GetChanCurBd(Chan);
// 	Status = XMcDma_ChanSubmit(Chan, TxBufPtr,
// 				   len + Padding);
// 	if (Status != XST_SUCCESS) {
// 		printf("ChanSubmit failed\n\r");
// 		return XST_FAILURE;
// 	}
// 	XMcDma_BdSetCtrl(BdCurPtr, XMCDMA_BD_CTRL_SOF_MASK |
// 			 XMCDMA_BD_CTRL_EOF_MASK);

// 	XMCDMA_CACHE_FLUSH((UINTPTR)(BdCurPtr));
// 	Status = XMcDma_ChanToHw(Chan);
// 	if (Status != XST_SUCCESS) {
// 		printf("XMcDma_ChanToHw failed for Tx\n\r");
// 		return XST_FAILURE;
// 	}

// 	return XST_SUCCESS;
// }

// int GetBufAddr()
// {
// 	UINTPTR TxBufPtr;

// 	if (TxCount !=0) {
// 		TxBufPtr = TxBuffPtr + ICMP_PKT_SIZE;
// 		TxBuffPtr += ICMP_PKT_SIZE;
// 	} else {
// 		TxBufPtr = TxBuffPtr;
// 	}

// 	TxCount++;
// 	return TxBufPtr;
// }

// int GetRxBufAddr()
// {
// 	UINTPTR RxBufPtr;

// 	if (RxCount == 0)
// 		RxBufPtr = RxBuffPtr;
// 	else {
// 		RxBuffPtr += XAE_JUMBO_MTU;
// 		RxBufPtr = RxBuffPtr;
// 	}
// 	RxCount++;

// 	return RxBufPtr;
// }

/*****************************************************************************/
/**
*
* This function will send a ARP request packet.
*
* @param	AxiEthernetInstancePtr is a pointer to the instance of the
*		Axi Ethernet component.
* @param	DmaInstancePtr is a pointer to the instance of the Dma
*		component.
* @param	ChanId is the MCDMA Channel number to be operate on.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
// void SendArpReqFrame(XAxiEthernet *AxiEthernetInstancePtr,
// 		     XAxiDma *DmaInstancePtr, u8 ChanId)
// {
// 	u16 *TempPtr;
// 	u16 *TxFramePtr;
// 	UINTPTR BufAddr;
// 	int Index, i;

// 	FramesTx = 0;
// 	TxFramePtr = (u16 *)(UINTPTR)GetBufAddr();
// 	BufAddr = (UINTPTR) TxFramePtr;

// 	if (Padding) {
// 		for (i = 0 ; i < 4; i++)
// 			*TxFramePtr++ = 0;
// 	}

// 	/*
// 	 * Add broadcast address.
// 	 */
// 	Index = MAC_ADDR_LEN;
// 	while (Index--) {
// 		*TxFramePtr++ = BROADCAST_ADDR;
// 	}

// 	/*
// 	 * Add local MAC address.
// 	 */
// 	Index = 0;
// 	TempPtr = (u16 *)LocalMacAddr;
// 	while (Index < MAC_ADDR_LEN) {
// 		*TxFramePtr++ = *(TempPtr + Index);
// 		Index++;
// 	}

// 	/*
// 	 * Add
// 	 * 	- Ethernet proto type.
// 	 *	- Hardware Type
// 	 *	- Protocol IP Type
// 	 *	- IP version (IPv6/IPv4)
// 	 *	- ARP Request
// 	 */
// 	*TxFramePtr++ = Xil_Htons(XAE_ETHER_PROTO_TYPE_ARP);
// 	*TxFramePtr++ = Xil_Htons(HW_TYPE);
// 	*TxFramePtr++ = Xil_Htons(XAE_ETHER_PROTO_TYPE_IP);
// 	*TxFramePtr++ = Xil_Htons(IP_VERSION);
// 	*TxFramePtr++ = Xil_Htons(ARP_REQUEST);

// 	/*
// 	 * Add local MAC address.
// 	 */
// 	Index = 0;
// 	TempPtr = (u16 *)LocalMacAddr;
// 	while (Index < MAC_ADDR_LEN) {
// 		*TxFramePtr++ = *(TempPtr + Index);
// 		Index++;
// 	}

// 	/*
// 	 * Add local IP address.
// 	 */
// 	Index = 0;
// 	TempPtr = (u16 *)LocalIpAddress;
// 	while (Index < IP_ADDR_LEN) {
// 		*TxFramePtr++ = *(TempPtr + Index);
// 		Index++;
// 	}

// 	/*
// 	 * Fills 6 bytes of information with zeros as per protocol.
// 	 */
// 	Index = 0;
// 	while (Index < 3) {
// 		*TxFramePtr++ = 0x0000;
// 		Index++;
// 	}

// 	/*
// 	 * Add Destination IP address.
// 	 */
// 	Index = 0;
// 	TempPtr = (u16 *)DestIpAddress;
// 	while (Index < IP_ADDR_LEN) {
// 		*TxFramePtr++ = *(TempPtr + Index);
// 		Index++;
// 	}

// 	/*
// 	 * Transmit the Frame.
// 	 */
// 	XAxienet_Send(AxiEthernetInstancePtr, DmaInstancePtr, ChanId, BufAddr, ARP_REQ_PKT_SIZE);
// 	while (1) {
// 		CheckTxDmaResult(DmaInstancePtr);
// 		if (FramesTx)
// 			break;
// 	}

// }

/*****************************************************************************/
/**
*
* This function will send a Echo request packet.
*
* @param	AxiEthernetInstancePtr is a pointer to the instance of the
*		Axi Ethernet component.
* @param	DmaInstancePtr is a pointer to the instance of the Dma
*		component.
* @param	ChanId is the MCDMA Channel number to be operate on.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
// void SendEchoReqFrame(XAxiEthernet *AxiEthernetInstancePtr,
// 	              XAxiDma *DmaInstancePtr, u8 ChanId)
// {
// 	u16 *TempPtr;
// 	u16 *TxFramePtr;
// 	UINTPTR BufAddr;
// 	u16 CheckSum;
// 	int Index, i;

// 	FramesTx = 0;
// 	TxFramePtr = (u16 *)(UINTPTR)GetBufAddr();
// 	BufAddr = (UINTPTR) TxFramePtr;

// 	if (Padding) {
// 		for (i = 0 ; i < 4; i++)
// 			*TxFramePtr++ = 0;
// 	}

// 	/*
// 	 * Add Destination MAC Address.
// 	 */
// 	Index = MAC_ADDR_LEN;
// 	while (Index--) {
// 		*(TxFramePtr + Index) = *(DestMacAddr + Index);
// 	}

// 	/*
// 	 * Add Source MAC Address.
// 	 */
// 	Index = MAC_ADDR_LEN;
// 	TempPtr = (u16 *)LocalMacAddr;
// 	while (Index--) {
// 		*(TxFramePtr + (Index + SRC_MAC_ADDR_LOC )) =
// 							*(TempPtr + Index);
// 	}

// 	/*
// 	 * Add IP header information.
// 	 */
// 	Index = IP_START_LOC;
// 	while (Index--) {
// 		*(TxFramePtr + (Index + ETHER_PROTO_TYPE_LOC )) =
// 				Xil_Htons(*(IpHeaderInfo + Index));
// 	}

// 	/*
// 	 * Add Source IP address.
// 	 */
// 	Index = IP_ADDR_LEN;
// 	TempPtr = (u16 *)LocalIpAddress;
// 	while (Index--) {
// 		*(TxFramePtr + (Index + IP_REQ_SRC_IP_LOC )) =
// 						*(TempPtr + Index);
// 	}

// 	/*
// 	 * Add Destination IP address.
// 	 */
// 	Index = IP_ADDR_LEN;
// 	TempPtr = (u16 *)DestIpAddress;
// 	while (Index--) {
// 		*(TxFramePtr + (Index + IP_REQ_DEST_IP_LOC )) =
// 						*(TempPtr + Index);
// 	}

// 	/*
// 	 * Checksum is calculated for IP field and added in the frame.
// 	 */
// 	CheckSum = CheckSumCalculation((u16 *)BufAddr, IP_START_LOC + 4,
// 							IP_HEADER_LEN);
// 	CheckSum = ~CheckSum;
// 	*(TxFramePtr + IP_CHECKSUM_LOC) = Xil_Htons(CheckSum);

// 	/*
// 	 * Add echo field information.
// 	 */
// 	*(TxFramePtr + ICMP_ECHO_FIELD_LOC) = Xil_Htons(XAE_ETHER_PROTO_TYPE_IP);

// 	/*
// 	 * Checksum value is initialized to zeros.
// 	 */
// 	*(TxFramePtr + ICMP_DATA_LEN) = 0x0000;

// 	/*
// 	 * Add identifier and sequence number to the frame.
// 	 */
// 	*(TxFramePtr + ICMP_IDEN_FIELD_LOC) = (IDEN_NUM);
// 	*(TxFramePtr + (ICMP_IDEN_FIELD_LOC + 1)) = Xil_Htons((u16)(++SeqNum));

// 	/*
// 	 * Add known data to the frame.
// 	 */
// 	Index = ICMP_KNOWN_DATA_LEN;
// 	while (Index--) {
// 		*(TxFramePtr + (Index + ICMP_KNOWN_DATA_LOC)) =
// 				Xil_Htons(*(IcmpData + Index));
// 	}

// 	/*
// 	 * Checksum is calculated for Data Field and added in the frame.
// 	 */
// 	CheckSum = CheckSumCalculation((u16 *)BufAddr, ICMP_DATA_START_LOC + 4,
// 						ICMP_DATA_FIELD_LEN );
// 	CheckSum = ~CheckSum;
// 	*(TxFramePtr + ICMP_DATA_CHECKSUM_LOC) = Xil_Htons(CheckSum);

// 	/*
// 	 * Transmit the Frame.
// 	 */
// 	XAxienet_Send(AxiEthernetInstancePtr, DmaInstancePtr, ChanId, BufAddr, ICMP_PKT_SIZE);
// 	while (1) {
// 		CheckTxDmaResult(DmaInstancePtr);
// 		if (FramesTx)
// 			break;
// 	}
// }

/*****************************************************************************/
/**
*
* This function will process the received packet. This function sends
* the echo request packet based on the ARP reply packet.
*
* @param	AxiEthernetInstancePtr is a pointer to the instance of the
*		Axi Ethernet component.
* @param	DmaInstancePtr is a pointer to the instance of the Dma
*		component.
* @param	ChanId is the MCDMA Channel number to be operate on.
*
* @return	XST_SUCCESS is returned when an echo reply is received.
*		Otherwise, XST_FAILURE is returned.
*
* @note		This assumes MAC does not strip padding or CRC.
*
******************************************************************************/
// int ProcessRecvFrame(XAxiEthernet *AxiEthernetInstancePtr,
// 	             XAxiDma *DmaInstancePtr, u8 ChanId)
// {
// 	u16 *RxFramePtr;
// 	u16 *TempPtr;
// 	u16 CheckSum;
// 	int Index;
// 	int Match = 0;
// 	int DataWrong = 0;


// 	RxFramePtr = (u16 *)(UINTPTR)GetRxBufAddr();
// 	TempPtr = (u16 *)LocalMacAddr;

// 	/*
// 	 * Check Dest Mac address of the packet with the LocalMac address.
// 	 */
// 	Match = CompareData(RxFramePtr, TempPtr, 0, 0, MAC_ADDR_LEN);
// 	if (Padding) {
// 		RxFramePtr += 4;
// 		Match = CompareData(RxFramePtr, TempPtr, 0, 0, MAC_ADDR_LEN);
// 	}
// 	if (Match == XST_SUCCESS) {
// 		/*
// 		 * Check ARP type.
// 		 */
// 		if (Xil_Ntohs(*(RxFramePtr + ETHER_PROTO_TYPE_LOC)) ==
// 				XAE_ETHER_PROTO_TYPE_ARP ) {

// 			/*
// 			 * Check ARP status.
// 			 */
// 			if (Xil_Ntohs(*(RxFramePtr + ARP_REQ_STATUS_LOC)) == ARP_REPLY) {

// 				/*
// 				 * Check destination IP address with
// 				 * packet's source IP address.
// 				 */
// 				TempPtr = (u16 *)DestIpAddress;
// 				Match = CompareData(RxFramePtr,
// 						TempPtr, ARP_REQ_SRC_IP_LOC,
// 						0, IP_ADDR_LEN);
// 				if (Match == XST_SUCCESS) {

// 					/*
// 					 * Copy src Mac address of the received
// 					 * packet.
// 					 */
// 					Index = MAC_ADDR_LEN;
// 					TempPtr = (u16 *)DestMacAddr;
// 					while (Index--) {
// 						*(TempPtr + Index) =
// 							*(RxFramePtr +
// 							(SRC_MAC_ADDR_LOC +
// 								Index));
// 					}

// 					/*
// 					 * Send Echo request packet.
// 					 */
// 					SendEchoReqFrame(AxiEthernetInstancePtr, DmaInstancePtr, ChanId);
// 				}
// 			}
// 		}

// 		/*
// 		 * Check for IP type.
// 		 */
// 		else if (Xil_Ntohs(*(RxFramePtr + ETHER_PROTO_TYPE_LOC)) ==
// 						XAE_ETHER_PROTO_TYPE_IP) {

// 			/*
// 			 * Calculate checksum.
// 			 */
// 			CheckSum = CheckSumCalculation(RxFramePtr,
// 							ICMP_DATA_START_LOC,
// 							ICMP_DATA_FIELD_LEN);

// 			/*
// 			 * Verify checksum, echo reply, identifier number and
// 			 * sequence number of the received packet.
// 			 */
// 			if ((CheckSum == CORRECT_CHECKSUM_VALUE) &&
// 			(Xil_Ntohs(*(RxFramePtr + ICMP_ECHO_FIELD_LOC)) == ECHO_REPLY) &&
// 			(Xil_Ntohs(*(RxFramePtr + ICMP_IDEN_FIELD_LOC)) == IDEN_NUM) &&
// 			(Xil_Ntohs(*(RxFramePtr + (ICMP_SEQ_NO_LOC))) == SeqNum)) {

// 				/*
// 				 * Verify data in the received packet with known
// 				 * data.
// 				 */
// 				TempPtr = IcmpData;
// 				Match = CompareData(RxFramePtr,
// 						TempPtr, ICMP_KNOWN_DATA_LOC,
// 							0, ICMP_KNOWN_DATA_LEN);
// 				if (Match == XST_FAILURE) {
// 					DataWrong = 1;
// 				}
// 			}
// 			if (DataWrong != 1) {
// 				printf("Packet No: %d ",
// 				NUM_OF_PING_REQ_PKTS - NumOfPingReqPkts);
// 				printf("Seq NO %d Echo Packet received\r\n",
// 								SeqNum);
// 				return XST_SUCCESS;
// 			}
// 		}
// 	}
// 	return XST_FAILURE;
// }
/*****************************************************************************/
/**
*
* This function calculates the checksum and returns a 16 bit result.
*
* @param 	RxFramePtr is a 16 bit pointer for the data to which checksum
* 		is to be calculated.
* @param	StartLoc is the starting location of the data from which the
*		checksum has to be calculated.
* @param	Length is the number of halfwords(16 bits) to which checksum is
* 		to be calculated.
*
* @return	It returns a 16 bit checksum value.
*
* @note		This can also be used for calculating checksum. The ones
* 		complement of this return value will give the final checksum.
*
******************************************************************************/
static u16 CheckSumCalculation(u16 *RxFramePtr, int StartLoc, int Length)
{
	u32 Sum = 0;
	u16 CheckSum = 0;
	int Index;

	/*
	 * Add all the 16 bit data.
	 */
	Index = StartLoc;
	while (Index < (StartLoc + Length)) {
		Sum = Sum + Xil_Htons(*(RxFramePtr + Index));
		Index++;
	}

	/*
	 * Add upper 16 bits to lower 16 bits.
	 */
	CheckSum = Sum;
	Sum = Sum>>16;
	CheckSum = Sum + CheckSum;
	return CheckSum;
}
/*****************************************************************************/
/**
*
* This function checks the match for the specified number of half words.
*
* @param	LhsPtr is a LHS entity pointer.
* @param 	RhsPtr is a RHS entity pointer.
* @param	LhsLoc is a LHS entity location.
* @param 	RhsLoc is a RHS entity location.
* @param 	Count is the number of location which has to compared.
*
* @return	XST_SUCCESS is returned when both the entities are same,
*		otherwise XST_FAILURE is returned.
*
* @note		None.
*
******************************************************************************/
static int CompareData(u16 *LhsPtr, u16 *RhsPtr, int LhsLoc, int RhsLoc,
								int Count)
{
	int Result;
	while (Count--) {
		if (*(LhsPtr + LhsLoc + Count) == *(RhsPtr + RhsLoc + Count)) {
			Result = XST_SUCCESS;
		} else {
			Result = XST_FAILURE;
			break;
		}
	}
	return Result;
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
