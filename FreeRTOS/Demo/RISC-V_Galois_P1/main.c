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

/**
 * This is docs for main
 */


/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>

/* Bsp includes. */
#include "bsp.h"

/******************************************************************************
 * This project provides test applications for Galois P1 SSITH processor.
 * TODO: update docs
 */
#if mainDEMO_TYPE == 1
	#pragma message "Demo type 1: Basic Blinky"
	extern void main_blinky( void );
#elif mainDEMO_TYPE == 2
	#pragma message "Demo type 2: Full"
	extern void main_full( void );
#elif mainDEMO_TYPE == 3
	#pragma message "Demo type 3: Drivers"
	extern void main_drivers( void );
#elif mainDEMO_TYPE == 4
	#pragma message "Demo type 4: Ping"
	extern void main_ping( void );
#elif mainDEMO_TYPE == 5
	#pragma message "Demo type 5: TCP"
	extern void main_tcp( void );
#else
	#error "Unsupported demo type"
#endif /* #if mainCREATE_SIMPLE_BLINKY_DEMO_ONLY == 1 */

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file.  See https://www.freertos.org/a00016.html */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

void vToggleLED( void );

/*-----------------------------------------------------------*/

/* The UART to which strings are output, and the GPIO used to toggle the LED. */
//static UART_instance_t g_uart;
//static gpio_instance_t g_gpio_out;

/*-----------------------------------------------------------*/

int main( void )
{
	prvSetupHardware();

	/* The mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is described at the top
	of this file. */
	#if mainDEMO_TYPE == 1
	{
		main_blinky();
	}
	#elif mainDEMO_TYPE == 2
	{
		main_full();
	}
	#elif mainDEMO_TYPE == 3
	{
		main_drivers();
	}
	#elif mainDEMO_TYPE == 4
	{
		main_ping();
	}
	#elif mainDEMO_TYPE == 5
	{
		main_tcp();
	}
	#endif
}
/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/

// TODO: toggle a real LED at some point
void vToggleLED( void )
{
	static uint32_t ulLEDState = 0;

// 	GPIO_set_outputs( &g_gpio_out, ulLEDState );
	ulLEDState = !ulLEDState;
}
// /*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	__asm volatile( "ebreak" );
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
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
	__asm volatile( "ebreak" );
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void ) {
	/* The tests in the full demo expect some interaction with interrupts. */
	#if( mainDEMO_TYPE == 2 )
	{
		extern void vFullDemoTickHook( void );
		vFullDemoTickHook();
	}
	#endif
}
