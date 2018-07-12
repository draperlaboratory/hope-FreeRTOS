/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
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

/*
 * RISCV port derived from ARM 7
*/

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portBASE_TYPE	long

#ifdef __riscv64
//#if __riscv_xlen == 64
	#define portSTACK_TYPE	uint64_t
	#define portPOINTER_SIZE_TYPE	uint64_t
#else
	#define portSTACK_TYPE	uint32_t
	#define portPOINTER_SIZE_TYPE	uint32_t
#endif

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif
/*-----------------------------------------------------------*/

/* Hardware specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) (1000 / configTICK_RATE_HZ) )
#ifdef __riscv64
//#if __riscv_xlen == 64
	#define portBYTE_ALIGNMENT	8
#else
	#define portBYTE_ALIGNMENT	4
#endif
#define portCRITICAL_NESTING_IN_TCB					1
/*-----------------------------------------------------------*/

extern void vPortYield( void );
extern void vTaskSwitchContext( void );
#define portYIELD()					vPortYield()
#define portYIELD_FROM_ISR() vTaskSwitchContext()

/* Critical section handling. */

extern void vPortDisableInterrupts(void);
extern void vPortEnableInterrupts(void);
#define portDISABLE_INTERRUPTS() vPortDisableInterrupts()
#define portENABLE_INTERRUPTS() vPortEnableInterrupts()

extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );
#define portENTER_CRITICAL()		vTaskEnterCritical();
#define portEXIT_CRITICAL()			vTaskExitCritical();

#if configGENERATE_RUN_TIME_STATS
extern unsigned long uiPortGetWallTimestampUs(void);
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE            uiPortGetWallTimestampUs
#define configUSE_STATS_FORMATTING_FUNCTIONS	  1
#endif

extern void prvSetupTimerInterrupt( void );
extern void prvSetNextTimerInterrupt(void);
extern uint32_t uiPortGetWallTimestampUs(void);
extern uint64_t xPortRawTime(void);
extern void vPortSysTickHandler( void );
extern void prvTaskExitError( void );
extern void panic(const char *, ...);

#define UNUSED(x) (void)x

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

