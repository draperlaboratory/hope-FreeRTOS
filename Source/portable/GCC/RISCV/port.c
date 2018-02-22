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

/* Standard includes. */
#include <stdlib.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

#define TIMER_INTERVAL (configTICK_CLOCK_HZ / configTICK_RATE_HZ)  /* raw ticks per timer tick */
#define U_SEC_INTERVAL (configTICK_CLOCK_HZ / 1000000) /* raw ticks per uSec */

/* Contains context when starting scheduler, save all 31 registers */
#ifdef __gracefulExit
BaseType_t xStartContext[31] = {0};
#endif

/*-----------------------------------------------------------*/

/* Setup the timer to generate the tick interrupts. */
extern void vPortInitTimerInterrupt( uint32_t interval);

extern void vPortSetNextTimerInterrupt( uint32_t interval);

/*
 * Setup the tick timer to generate the tick interrupts at the required frequency.
 */
void prvSetupTimerInterrupt( void )
{
  vPortInitTimerInterrupt(TIMER_INTERVAL);
}
/*-----------------------------------------------------------*/

/* Sets the next timer interrupt
 * Reads previous timer compare register, and adds tickrate */
void prvSetNextTimerInterrupt(void)
{
  vPortSetNextTimerInterrupt(TIMER_INTERVAL);
}

/*-----------------------------------------------------------*/
uint32_t uiPortGetWallTimestampUs(void)
{
  return (xPortRawTime() / U_SEC_INTERVAL);
}

/*-----------------------------------------------------------*/

void vPortSysTickHandler( void )
{
  prvSetNextTimerInterrupt();

	/* Increment the RTOS tick. */
	if( xTaskIncrementTick() != pdFALSE )
	{
		vTaskSwitchContext();
	}
	prvSetNextTimerInterrupt();
}
/*-----------------------------------------------------------*/

void prvTaskExitError( void )
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ). */

	portDISABLE_INTERRUPTS();
	panic(__FILE__, __func__, __LINE__, "Task must not exit");
}
/*-----------------------------------------------------------*/

/*
  Mark this as weak, so that random applications can easily override.
*/
void panic(const char* file, const char* func, int line, const char* msg, ...) __attribute__((weak));
void panic(const char* file, const char* func, int line, const char* msg, ...) {
  while (1) ;
}
