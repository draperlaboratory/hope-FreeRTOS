/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
 * Instructions for using this project are provided on:
 * http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCPIP_FAT_Examples_Xilinx_Zynq.html
 */


/* Standard includes. */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+TCP includes. */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_TCP_server.h"
#include "FreeRTOS_DHCP.h"

/* FreeRTOS+FAT includes. */
#include "ff_stdio.h"
#include "ff_ramdisk.h"
//#include "ff_sddisk.h"

/* Freedom SDK includes. */
#include "plic/plic_driver.h"

/* Demo application includes. */
#include "hr_gettime.h"
#include "UDPLoggingPrintf.h"
//#include "UDPCommandConsole.h"
//#include "TCPEchoClient_SingleTasks.h"
//#include "SimpleTCPEchoServer.h"
#include "cgi.h"
#include "database.h"
#include "auth.h"

/* Xilinx includes. */
//#include "xil_cache.h"

/* Set the following constants to 1 or 0 to define which tasks to include and
exclude.  A WIDER RANGE OF EXAMPLES ARE AVAILABLE IN THE WIN32 DEMO:
http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/examples_FreeRTOS_simulator.html

mainCREATE_FTP_SERVER:  When set to 1 the TCP server task will include an FTP
server.
See http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/FTP_Server.html

mainCREATE_HTTP_SERVER:  When set to 1 the TCP server task will include a basic
HTTP server.
See http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/HTTP_web_Server.html

mainCREATE_UDP_CLI_TASKS:  When set to 1 a command console that uses a UDP port
for input and output is created using FreeRTOS+CLI.  The port number used is set
by the mainUDP_CLI_PORT_NUMBER constant above.  A dumb UDP terminal such as YAT
can be used to connect.
See http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/UDP_CLI.html

mainCREATE_TCP_ECHO_TASKS_SINGLE:  When set to 1 a set of tasks are created that
send TCP echo requests to the standard echo port (port 7), then wait for and
verify the echo reply, from within the same task (Tx and Rx are performed in the
same RTOS task).  The IP address of the echo server must be configured using the
configECHO_SERVER_ADDR0 to configECHO_SERVER_ADDR3 constants in
FreeRTOSConfig.h.
See http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Clients.html

mainCREATE_SIMPLE_TCP_ECHO_SERVER:  When set to 1 FreeRTOS tasks are used with
FreeRTOS+TCP to create a TCP echo server.  Echo requests can be sent to the
FreeRTOS+TCP using a tool such as EchoTool https://github.com/PavelBansky/EchoTool
(a pre-built binary is available).
See http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Server.html

mainCREATE_UDP_LOGGING_TASK: Set to 1 to enabled UDP logging as described on the
documentation page for this demo
http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCPIP_FAT_Examples_Xilinx_Zynq.html
*/
#define mainCREATE_FTP_SERVER					0
#define mainCREATE_HTTP_SERVER 					1
#define mainCREATE_UDP_CLI_TASKS				0
#define mainCREATE_TCP_ECHO_CLIENT_TASKS_SINGLE	0
#define mainCREATE_SIMPLE_TCP_ECHO_SERVER		0
#define mainCREATE_UDP_LOGGING_TASK 			1

/* UDP CLI task parameters --------------------------------------------------
See http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/UDP_CLI.html */
#define mainUDP_CLI_TASK_PRIORITY		( tskIDLE_PRIORITY )
#define mainUDP_CLI_PORT_NUMBER			( 5001UL )
#define mainUDP_CLI_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE * 4 )

/* Simple echo client task parameters ---------------------------------------
See http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Clients.html */
#define mainECHO_CLIENT_TASK_STACK_SIZE 	( configMINIMAL_STACK_SIZE * 3 )
#define mainECHO_CLIENT_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

/* Simple echo server task parameters ---------------------------------------
See http://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCP_Echo_Server.html */
#define mainECHO_SERVER_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define	mainECHO_SERVER_STACK_SIZE			( configMINIMAL_STACK_SIZE * 3 )

/* Other configuration parameters -------------------------------------------*/

/* FTP and HTTP servers execute in the TCP server work task.  */
#define mainTCP_SERVER_TASK_PRIORITY	( tskIDLE_PRIORITY + 2 )
#define	mainTCP_SERVER_STACK_SIZE		1600 /* Not used in the Win32 simulator. */

/* The number and size of sectors that will make up the RAM disk. */
#define mainRAM_DISK_SECTOR_SIZE		512UL /* Currently fixed! */
#define mainRAM_DISK_SECTORS			( ( 5UL * 1024UL * 1024UL ) / mainRAM_DISK_SECTOR_SIZE ) /* 5M bytes. */
#define mainIO_MANAGER_CACHE_SIZE		( 15UL * mainRAM_DISK_SECTOR_SIZE )

/* Where the SD and RAM disks are mounted.  As set here the SD card disk is the
root directory, and the RAM disk appears as a directory off the root. */
#define mainSD_CARD_DISK_NAME			"/"
#define mainRAM_DISK_NAME				"/ram"

/* Define names that will be used for SDN, LLMNR and NBNS searches. */
#define mainHOST_NAME					"RTOSDemo"
#define mainDEVICE_NICK_NAME			"zynq"

/* Set to 0 to run the STDIO examples once only, or 1 to create multiple tasks
that run the tests continuously. */
#define mainRUN_STDIO_TESTS_IN_MULTIPLE_TASK 0

/* #define NEGATIVE_TEST_DEBUG */
#ifdef NEGATIVE_TEST_DEBUG
#include "negative_tests.h"
#endif

/*-----------------------------------------------------------*/

/*
 * Just seeds the simple pseudo random number generator.
 */
static void prvSRand( UBaseType_t ulSeed );

/*
 * Miscellaneous initialisation including preparing the logging and seeding the
 * random number generator.
 */
static void prvMiscInitialisation( void );

/*
 * Creates a RAM disk, then creates files on the RAM disk.  The files can then
 * be viewed via the FTP server and the command line interface.
 */
static void prvCreateDiskAndExampleFiles( void );

/*
 * The task that runs the FTP and HTTP servers.
 */
static void prvServerWorkTask( void *pvParameters );

/*
 * Register commands that can be used with FreeRTOS+CLI through the UDP socket.
 * The commands are defined in CLI-commands.c and File-related-CLI-commands.c
 * respectively.
 */
extern void vRegisterSampleCLICommands( void );
extern void vRegisterFileSystemCLICommands( void );
extern void vRegisterTCPCLICommands( void );

/* The default IP and MAC address used by the demo.  The address configuration
defined here will be used if ipconfigUSE_DHCP is 0, or if ipconfigUSE_DHCP is
1 but a DHCP server could not be contacted.  See the online documentation for
more information. */
static const uint8_t ucIPAddress[ 4 ] = { configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3 };
static const uint8_t ucNetMask[ 4 ] = { configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3 };
static const uint8_t ucGatewayAddress[ 4 ] = { configGATEWAY_ADDR0, configGATEWAY_ADDR1, configGATEWAY_ADDR2, configGATEWAY_ADDR3 };
static const uint8_t ucDNSServerAddress[ 4 ] = { configDNS_SERVER_ADDR0, configDNS_SERVER_ADDR1, configDNS_SERVER_ADDR2, configDNS_SERVER_ADDR3 };

/* Default MAC address configuration. */
const uint8_t ucMACAddress[ 6 ] = { configMAC_ADDR0, configMAC_ADDR1, configMAC_ADDR2, configMAC_ADDR3, configMAC_ADDR4, configMAC_ADDR5 };

/* Use by the pseudo random number generator. */
static UBaseType_t ulNextRand;

/* Handle of the task that runs the FTP and HTTP servers. */
static TaskHandle_t xServerWorkTaskHandle = NULL;

/* FreeRTOS+FAT disks for the SD card and RAM disk. */
//static FF_Disk_t *pxSDDisk;
static FF_Disk_t *pxRAMDisk;

/*-----------------------------------------------------------*/

plic_instance_t g_plic;

static void prvSetupHardware( void )
{
	PLIC_init(&g_plic, PLIC_CTRL_ADDR, PLIC_NUM_INTERRUPTS, PLIC_NUM_PRIORITIES);
}
/*-----------------------------------------------------------*/

static void prvSetupEthIRQ( void )
{
	/*Enable Eth Interrupt*/
	PLIC_enable_interrupt(&g_plic, 5);

   PLIC_set_priority(&g_plic, 5, 1);

	/* Make sure timers don't interrupt until the scheduler is running. */
	portDISABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

const char * const pcPassMessage = "Blink\r\n";

#if 0
void main( void )
{
int i, n;
unsigned int *ver = (unsigned int*)GEM0_CTRL_ADDR;
char vstr[64];

   for (i=0;i<256;++i,++ver)
   {
      n = sprintf(vstr, "Version%d @ 0x%x:  0x%x\n", i,(unsigned int)ver, *ver);
      printf("Test.\n");
      write(STDOUT_FILENO, vstr, n);
   }

   ver = (unsigned int*)(MEM_END_ADDR - (256 * sizeof(unsigned int)));
   for (i=0;i<256;++i,++ver)
   {
      n = sprintf(vstr, "Memory%d @ 0x%x:  0x%x\n", i,(unsigned int)ver, *ver);
      printf("Test.\n");
      write(STDOUT_FILENO, vstr, n);
   }

   for( ;; )
	{
   }
}
#else
/* See http://www.FreeRTOS.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/TCPIP_FAT_Examples_Xilinx_Zynq.html */
int main( void )
{
   printf("Test.\n");
   write( STDOUT_FILENO, pcPassMessage, strlen( pcPassMessage ) );
   write( STDOUT_FILENO, pcPassMessage, strlen( pcPassMessage ) );
   write( STDOUT_FILENO, pcPassMessage, strlen( pcPassMessage ) );
   write( STDOUT_FILENO, pcPassMessage, strlen( pcPassMessage ) );

    #ifdef NEGATIVE_TEST_DEBUG
    negative_test();
    #endif
    
   
   printf("Init time.\n");
	/* Miscellaneous initialisation including preparing the logging and seeding
	the random number generator. */
	prvMiscInitialisation();

   printf("Hardware time.\n");

   prvSetupHardware();
   prvSetupEthIRQ();

   printf("IP time.\n");

	/* Initialise the network interface.

	***NOTE*** Tasks that use the network are created in the network event hook
	when the network is connected and ready for use (see the definition of
	vApplicationIPNetworkEventHook() below).  The address values passed in here
	are used if ipconfigUSE_DHCP is set to 0, or if ipconfigUSE_DHCP is set to 1
	but a DHCP server cannot be	contacted. */
	FreeRTOS_printf( ( "FreeRTOS_IPInit\n" ) );
	FreeRTOS_IPInit( ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress );

   printf("Server time.\n");

	/* Create the task that handles the FTP and HTTP servers.  This will
	initialise the file system then wait for a notification from the network
	event hook before creating the servers.  The task is created at the idle
	priority, and sets itself to mainTCP_SERVER_TASK_PRIORITY after the file
	system has initialised. */
	xTaskCreate( prvServerWorkTask, "SvrWork", mainTCP_SERVER_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xServerWorkTaskHandle );

	/* Start the RTOS scheduler. */
	FreeRTOS_debug_printf( ("vTaskStartScheduler\n") );

  printf("App time.\n");
  DatabaseInit();
  AuthInit();

  user_t *user = UserCreate("user1", "password1", "aaaa", "aaaaa", "aaaaaaaaaa");
  MedicalSetPatient(user);
  DatabaseAddUser(user);

  printf("Everything Running.\n");
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following
	line will never be reached.  If the following line does execute, then
	there was insufficient FreeRTOS heap memory available for the idle and/or
	timer tasks	to be created.  See the memory management section on the
	FreeRTOS web site for more details.  http://www.freertos.org/a00111.html */
	for( ;; )
	{
	}
}
#endif
/*-----------------------------------------------------------*/

static void prvCreateDiskAndExampleFiles( void )
{
static uint8_t ucRAMDisk[ mainRAM_DISK_SECTORS * mainRAM_DISK_SECTOR_SIZE ];
extern void vCreateAndVerifyExampleFiles( char *pcMountPath );
extern void vStdioWithCWDTest( const char *pcMountPath );
extern void vMultiTaskStdioWithCWDTest( const char *const pcMountPath, uint16_t usStackSizeWords );

	/* Create the RAM disk. */
	pxRAMDisk = FF_RAMDiskInit( mainRAM_DISK_NAME, ucRAMDisk, mainRAM_DISK_SECTORS, mainIO_MANAGER_CACHE_SIZE );
	configASSERT( pxRAMDisk );

	/* Print out information on the RAM disk. */
	FF_RAMDiskShowPartition( pxRAMDisk );

	/* Create the SD card disk. */
//	pxSDDisk = FF_SDDiskInit( mainSD_CARD_DISK_NAME );

	/* Create a few example files on the disk.  These are not deleted again. */
	vCreateAndVerifyExampleFiles( mainRAM_DISK_NAME );
}
/*-----------------------------------------------------------*/

static void prvServerWorkTask( void *pvParameters )
{
/* A structure that defines the servers to be created. */
static const struct xSERVER_CONFIG xServerConfiguration[] =
{
	#if( mainCREATE_HTTP_SERVER == 1 )
		/* Server type,		port number,	backlog, 	root dir. */
		{ eSERVER_HTTP, 	80, 			12, 		configHTTP_ROOT },
	#endif /* mainCREATE_HTTP_SERVER */

	#if( mainCREATE_FTP_SERVER == 1 )
		/* Server type,		port number,	backlog, 	root dir. */
		{ eSERVER_FTP,  	21, 			12, 		"" }
	#endif /* mainCREATE_FTP_SERVER */
};

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;
	( void ) xServerConfiguration;

	/* Create the RAM disk used by the FTP and HTTP servers. */
  printf("Creating ramdisk...\n");
	prvCreateDiskAndExampleFiles();
  printf("Ramdisk created\n");

	/* The priority of this task can be raised now the disk has been
	initialised. */
	vTaskPrioritySet( NULL, mainTCP_SERVER_TASK_PRIORITY );

	#if( ( mainCREATE_FTP_SERVER == 1 ) || ( mainCREATE_HTTP_SERVER == 1 ) )
	{
	const TickType_t xInitialBlockTime = pdMS_TO_TICKS( 200UL );
	TCPServer_t *pxTCPServer = NULL;

		/* Setup CGI functions */
    printf("Starting CGI setup...\n");
		CgiSetup();
    printf("CGI setup complete\n");

		/* Wait until the network is up before creating the servers.  The
		notification is given from the network event hook. */
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

		/* Create the servers defined by the xServerConfiguration array above. */
		pxTCPServer = FreeRTOS_CreateTCPServer( xServerConfiguration, sizeof( xServerConfiguration ) / sizeof( xServerConfiguration[ 0 ] ) );
		configASSERT( pxTCPServer );

		for( ;; )
		{
			FreeRTOS_TCPServerWork( pxTCPServer, xInitialBlockTime );
		}
	}
	#else
	{
		/* Neither the HTTP nor the FTP server is being used - nothing for
		this task to do.  Set xServerWorkTaskHandle to NULL so nothing tries
		signalling this task, then delete the task. */
		xServerWorkTaskHandle = NULL;
		vTaskDelete( NULL );
	}
	#endif /* ( ( mainCREATE_FTP_SERVER == 1 ) || ( mainCREATE_HTTP_SERVER == 1 ) ) */
}
/*-----------------------------------------------------------*/

void vAssertCalled( const char *pcFile, uint32_t ulLine )
{
volatile uint32_t ulBlockVariable = 0UL;
volatile char *pcFileName = ( volatile char *  ) pcFile;
volatile uint32_t ulLineNumber = ulLine;

	( void ) pcFileName;
	( void ) ulLineNumber;

	FreeRTOS_debug_printf( ( "vAssertCalled( %s, %ld\n", pcFile, ulLine ) );

	/* Setting ulBlockVariable to a non-zero value in the debugger will allow
	this function to be exited. */
	taskDISABLE_INTERRUPTS();
	{
		while( ulBlockVariable == 0UL )
		{
			__asm volatile( "NOP" );
		}
	}
	taskENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

/* Called by FreeRTOS+TCP when the network connects or disconnects.  Disconnect
events are only received if implemented in the MAC driver. */
void vApplicationIPNetworkEventHook( eIPCallbackEvent_t eNetworkEvent )
{
uint32_t ulIPAddress, ulNetMask, ulGatewayAddress, ulDNSServerAddress;
char cBuffer[ 16 ];
static BaseType_t xTasksAlreadyCreated = pdFALSE;

	/* If the network has just come up...*/
	if( eNetworkEvent == eNetworkUp )
	{
		/* Create the tasks that use the IP stack if they have not already been
		created. */
		if( xTasksAlreadyCreated == pdFALSE )
		{
			/* See the comments above the definitions of these pre-processor
			macros at the top of this file for a description of the individual
			demo tasks. */

			/* Let the server work task now it can now create the servers. */
			if( xServerWorkTaskHandle != NULL )
			{
				xTaskNotifyGive( xServerWorkTaskHandle );
			}

			/* Register example commands with the FreeRTOS+CLI command
			interpreter via the UDP port specified by the
			mainUDP_CLI_PORT_NUMBER constant. */
			#if( mainCREATE_UDP_CLI_TASKS == 1 )
			{
				vRegisterSampleCLICommands();
				vRegisterFileSystemCLICommands();
				vRegisterTCPCLICommands();
				vStartUDPCommandInterpreterTask( mainUDP_CLI_TASK_STACK_SIZE, mainUDP_CLI_PORT_NUMBER, mainUDP_CLI_TASK_PRIORITY );
			}
			#endif

			#if( mainCREATE_TCP_ECHO_CLIENT_TASKS_SINGLE == 1 )
			{
				vStartTCPEchoClientTasks_SingleTasks( mainECHO_CLIENT_TASK_STACK_SIZE, mainECHO_CLIENT_TASK_PRIORITY );
			}
			#endif

			#if( mainCREATE_SIMPLE_TCP_ECHO_SERVER == 1 )
			{
				vStartSimpleTCPServerTasks( mainECHO_SERVER_STACK_SIZE, mainECHO_SERVER_TASK_PRIORITY );
			}
			#endif

			/* Start a new task to fetch logging lines and send them out. */
			#if( mainCREATE_UDP_LOGGING_TASK == 1 )
			{
				vUDPLoggingTaskCreate();
			}
			#endif

			xTasksAlreadyCreated = pdTRUE;
		}

		/* Print out the network configuration, which may have come from a DHCP
		server. */
		FreeRTOS_GetAddressConfiguration( &ulIPAddress, &ulNetMask, &ulGatewayAddress, &ulDNSServerAddress );
		FreeRTOS_inet_ntoa( ulIPAddress, cBuffer );
		FreeRTOS_printf( ( "IP Address: %s\r\n", cBuffer ) );
      printf( ( "IP Address: %s\r\n", cBuffer ) );

		FreeRTOS_inet_ntoa( ulNetMask, cBuffer );
		FreeRTOS_printf( ( "Subnet Mask: %s\r\n", cBuffer ) );
      printf( ( "Subnet Mask: %s\r\n", cBuffer ) );

		FreeRTOS_inet_ntoa( ulGatewayAddress, cBuffer );
		FreeRTOS_printf( ( "Gateway Address: %s\r\n", cBuffer ) );
      printf( ( "Gateway Address: %s\r\n", cBuffer ) );

		FreeRTOS_inet_ntoa( ulDNSServerAddress, cBuffer );
		FreeRTOS_printf( ( "DNS Server Address: %s\r\n\r\n\r\n", cBuffer ) );
      printf( ( "DNS Server Address: %s\r\n\r\n\r\n", cBuffer ) );
	}
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	vAssertCalled( __FILE__, __LINE__ );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

UBaseType_t uxRand( void )
{
const uint32_t ulMultiplier = 0x015a4e35UL, ulIncrement = 1UL;

	/* Utility function to generate a pseudo random number. */

	ulNextRand = ( ulMultiplier * ulNextRand ) + ulIncrement;
	return( ( int ) ( ulNextRand >> 16UL ) & 0x7fffUL );
}
/*-----------------------------------------------------------*/

static void prvSRand( UBaseType_t ulSeed )
{
	/* Utility function to seed the pseudo random number generator. */
	ulNextRand = ulSeed;
}
/*-----------------------------------------------------------*/

static void prvMiscInitialisation( void )
{
time_t xTimeNow;

	/* Seed the random number generator. */
	time( &xTimeNow );
	FreeRTOS_debug_printf( ( "Seed for randomiser: %lu\n", xTimeNow ) );
	prvSRand( ( uint32_t ) xTimeNow );
	FreeRTOS_debug_printf( ( "Random numbers: %08X %08X %08X %08X\n", ipconfigRAND32(), ipconfigRAND32(), ipconfigRAND32(), ipconfigRAND32() ) );

//	Xil_DCacheEnable();
}
/*-----------------------------------------------------------*/

struct tm *gmtime_r( const time_t *pxTime, struct tm *tmStruct )
{
	/* Dummy time functions to keep the file system happy in the absence of
	target support. */
	memcpy( tmStruct, gmtime( pxTime ), sizeof( * tmStruct ) );
	return tmStruct;
}
/*-----------------------------------------------------------*/

time_t FreeRTOS_time( time_t *pxTime )
{
time_t xReturn;

	xReturn = time( &xReturn );

	if( pxTime != NULL )
	{
		*pxTime = xReturn;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

const char *pcApplicationHostnameHook( void )
{
	/* Assign the name "FreeRTOS" to this network node.  This function will be
	called during the DHCP: the machine will be registered with an IP address
	plus this name. */
	return mainHOST_NAME;
}
/*-----------------------------------------------------------*/

BaseType_t xApplicationDNSQueryHook( const char *pcName )
{
BaseType_t xReturn;

	/* Determine if a name lookup is for this node.  Two names are given
	to this node: that returned by pcApplicationHostnameHook() and that set
	by mainDEVICE_NICK_NAME. */
	if( strcasecmp( pcName, pcApplicationHostnameHook() ) == 0 )
	{
		xReturn = pdPASS;
	}
	else if( strcasecmp( pcName, mainDEVICE_NICK_NAME ) == 0 )
	{
		xReturn = pdPASS;
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

#warning _gettimeofday_r is just stubbed out here.
struct timezone;
struct timeval;
int _gettimeofday_r(struct _reent * x, struct timeval *y , struct timezone * ptimezone )
{
	( void ) x;
	( void ) y;
	( void ) ptimezone;

	return 0;
}
/*-----------------------------------------------------------*/

/* Called automatically when a reply to an outgoing ping is received. */
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier )
{
static const char *pcSuccess = "Ping reply received - ";
static const char *pcInvalidChecksum = "Ping reply received with invalid checksum - ";
static const char *pcInvalidData = "Ping reply received with invalid data - ";

	switch( eStatus )
	{
		case eSuccess	:
			FreeRTOS_printf( ( pcSuccess ) );
			break;

		case eInvalidChecksum :
			FreeRTOS_printf( ( pcInvalidChecksum ) );
			break;

		case eInvalidData :
			FreeRTOS_printf( ( pcInvalidData ) );
			break;

		default :
			/* It is not possible to get here as all enums have their own
			case. */
			break;
	}

	FreeRTOS_debug_printf( ( "identifier %d\r\n", ( int ) usIdentifier ) );

	/* Prevent compiler warnings in case FreeRTOS_debug_printf() is not defined. */
	( void ) usIdentifier;
}
/*-----------------------------------------------------------*/

void vOutputChar( const char cChar, const TickType_t xTicksToWait  )
{
	/* To keep the linker quiet only. */
	( void ) cChar;
	( void ) xTicksToWait;
}

/*-----------------------------------------------------------*/
