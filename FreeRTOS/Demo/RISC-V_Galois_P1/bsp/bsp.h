/*****************************************************************************/
/**
*
* @file bsp.h
* @addtogroup bsp
* @{
*
* Test
* ## This is markdown example
*
* **More** markdown
*
* @note
*
* This file contains items which are architecture dependent.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date   Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a rmm  12/14/01 First release
*       rmm  05/09/03 Added "xassert always" macros to rid ourselves of diab
*                     compiler warnings
* 1.00a rpm  11/07/03 Added XNullHandler function as a stub interrupt handler
* 1.00a rpm  07/21/04 Added XExceptionHandler typedef for processor exceptions
* 1.00a xd   11/03/04 Improved support for doxygen.
* 1.00a wre  01/25/07 Added Linux style data types u32, u16, u8, TRUE, FALSE
* 1.00a rpm  04/02/07 Added ifndef KERNEL around u32, u16, u8 data types
* </pre>
*
******************************************************************************/

#ifndef RISCV_BLUESPEC_BSP_H
#define RISCV_BLUESPEC_BSP_H

#include "stdint.h"
#include "plic_driver.h"

#define PLIC_BASE_ADDR (0xC000000ULL)

#define PLIC_SOURCE_UART0 0x1
#define PLIC_SOURCE_ETH 0x2
#define PLIC_SOURCE_DMA_MM2S 0x3
#define PLIC_SOURCE_DMA_S2MM 0x4

#define PLIC_PRIORITY_UART0 0x1
#define PLIC_PRIORITY_ETH 0x2
#define PLIC_PRIORITY_DMA_MM2S 0x3
#define PLIC_PRIORITY_DMA_S2MM 0x3

// UART defines
#define XPAR_XUARTNS550_NUM_INSTANCES 1
#define XPAR_DEFAULT_BAUD_RATE 9600

#define XPAR_UARTNS550_0_DEVICE_ID 0
#define XPAR_UARTNS550_0_BASEADDR 0x62300000ULL
#define XPAR_UARTNS550_0_CLOCK_HZ (83000000ULL) // 83MHz

// DMA defines
#define XPAR_AXI_DMA 1
#define XPAR_XAXIDMA_NUM_INSTANCES 1

// TODO: make sure they are sane
#define	XPAR_AXIDMA_0_DEVICE_ID 0
// Virtual base address of DMA engine
#define	XPAR_AXIDMA_0_BASEADDR 0x62200000ULL
// Control/status stream
#define XPAR_AXIDMA_0_SG_INCLUDE_STSCNTRL_STRM 1
// AXI4 memory-mapped to stream TODO: Tx?
#define XPAR_AXIDMA_0_INCLUDE_MM2S 1
// Allow unaligned transfers
#define XPAR_AXIDMA_0_INCLUDE_MM2S_DRE 1
#define XPAR_AXIDMA_0_M_AXI_MM2S_DATA_WIDTH 32
// AXI stream to memory-mapped TODO: Rx?
#define XPAR_AXIDMA_0_INCLUDE_S2MM 1
// Allow unaligned transfers
#define XPAR_AXIDMA_0_INCLUDE_S2MM_DRE 1
#define XPAR_AXIDMA_0_M_AXI_S2MM_DATA_WIDTH 32
// Scatter-gather engine
#define XPAR_AXIDMA_0_INCLUDE_SG 1
#define XPAR_AXIDMA_0_NUM_MM2S_CHANNELS 1
#define XPAR_AXIDMA_0_NUM_S2MM_CHANNELS 1 
#define XPAR_AXI_DMA_0_MM2S_BURST_SIZE 16
#define XPAR_AXI_DMA_0_S2MM_BURST_SIZE 16
#define XPAR_AXI_DMA_0_MICRO_DMA 0
#define XPAR_AXI_DMA_0_ADDR_WIDTH 64
#define XPAR_AXIDMA_0_SG_LENGTH_WIDTH 16


// Ethernet defines
#define XPAR_AXIETHERNET_0_PHYADDR 0x03
#define XPAR_XAXIETHERNET_NUM_INSTANCES 1

#define XPAR_AXIETHERNET_0_DEVICE_ID 0
#define XPAR_AXIETHERNET_0_BASEADDR 0x62100000ULL
// 0 for SoftTemac at 10/100 Mbps, 1 for SoftTemac at 10/100/1000 Mbps and 2 for Vitex6 Hard Temac 
#define XPAR_AXIETHERNET_0_TEMAC_TYPE 2 // TODO: not sure if this is right
// TxCsum indicates that the device has checksum offload on the Tx channel or not.
#define XPAR_AXIETHERNET_0_TXCSUM 0
// RxCsum indicates that the device has checksum offload on the Rx channel or not.
#define XPAR_AXIETHERNET_0_RXCSUM 0
// PhyType indicates which type of PHY interface is used (MII, GMII, RGMII, etc.)
#define XPAR_AXIETHERNET_0_PHY_TYPE XAE_PHY_TYPE_SGMII
#define XPAR_AXIETHERNET_0_TXVLAN_TRAN 0
#define XPAR_AXIETHERNET_0_RXVLAN_TRAN 0
#define XPAR_AXIETHERNET_0_TXVLAN_TAG 0
#define XPAR_AXIETHERNET_0_RXVLAN_TAG 0
#define XPAR_AXIETHERNET_0_TXVLAN_STRP 0
#define XPAR_AXIETHERNET_0_RXVLAN_STRP 0
// Extended multicast address filtering
#define XPAR_AXIETHERNET_0_MCAST_EXTEND 1
// Statistics gathering options
#define XPAR_AXIETHERNET_0_STATS 1
// Ethernet Audio Video Bridging
#define XPAR_AXIETHERNET_0_AVB 0
// SGMII over LVDS
#define XPAR_AXIETHERNET_0_ENABLE_SGMII_OVER_LVDS 1
// Enable 1588 option. 
#define XPAR_AXIETHERNET_0_ENABLE_1588 0
// Tells whether MAC is 1G or 2p5G.
#define XPAR_AXIETHERNET_0_SPEED XAE_SPEED_1000_MBPS// TODO: not sure if this is right
// Number of table entries for the multicast address filtering
#define XPAR_AXIETHERNET_0_NUM_TABLE_ENTRIES 4
// Axi Ethernet interrupt ID.
#define XPAR_AXIETHERNET_0_INTR PLIC_SOURCE_ETH // TODO: doesn't appear to be used
// AxiDevType is the type of device attached to the Axi Ethernet's AXI4-Stream interface.
#define XPAR_AXIETHERNET_0_CONNECTED_TYPE XPAR_AXI_DMA
// AxiDevBaseAddress is the base address of the device attached to the Axi Ethernet's AXI4-Stream interface.
#define XPAR_AXIETHERNET_0_CONNECTED_BASEADDR 0x62200000ULL // DMA base address
// Unused
#define XPAR_AXIETHERNET_0_FIFO_INTR 0xFF
// Axi DMA RX interrupt ID
#define XPAR_AXIETHERNET_0_DMA_RX_INTR PLIC_SOURCE_DMA_S2MM
// Axi DMA TX interrupt ID
#define XPAR_AXIETHERNET_0_DMA_TX_INTR PLIC_SOURCE_DMA_MM2S

extern plic_instance_t Plic;

/**
 *  Prepare haredware to run the demo.
 */
void prvSetupHardware( void );

void external_interrupt_handler( uint32_t cause );

// Some xillinx drivers require to sleep for given number of seconds
void sleep(uint32_t secs);
void msleep(uint32_t msecs);


/*****************************************************************************/
/**
*
* This macro polls an address periodically until a condition is met or till the
* timeout occurs.
* The minimum timeout for calling this macro is 100us. If the timeout is less
* than 100us, it still waits for 100us. Also the unit for the timeout is 100us.
* If the timeout is not a multiple of 100us, it waits for a timeout of
* the next usec value which is a multiple of 100us.
*
* @param            IO_func - accessor function to read the register contents.
*                   Depends on the register width.
* @param            ADDR - Address to be polled
* @param            VALUE - variable to read the value
* @param            COND - Condition to checked (usually involves VALUE)
* @param            TIMEOUT_US - timeout in micro seconds
*
* @return           0 - when the condition is met
*                   -1 - when the condition is not met till the timeout period
*
* @note             none
*
*****************************************************************************/
#define Xil_poll_timeout(IO_func, ADDR, VALUE, COND, TIMEOUT_US) \
 ( {	  \
	u64 timeout = TIMEOUT_US/100;    \
	if(TIMEOUT_US%100!=0)	\
		timeout++;   \
	for(;;) { \
		VALUE = IO_func(ADDR); \
		if(COND) \
			break; \
		else {    \
			msleep(1);  \
			timeout--; \
			if(timeout==0) \
			break;  \
		}  \
	}    \
	(timeout>0) ? 0 : -1;  \
 }  )

#endif /* RISCV_BLUESPEC_BSP_H */
