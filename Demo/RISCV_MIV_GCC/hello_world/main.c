/*
 * Copyright © 2017-2018 Dover Microsystems, Inc.
 * All rights reserved. 
 *
 * Use and disclosure subject to the following license. 
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <string.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "utils.h"

#define TICK_INTERVAL 10
#define TIMER_INTERVAL (TICK_INTERVAL * configTICK_CLOCK_HZ / configTICK_RATE_HZ)

#define PREEMPTIVE


extern uint64_t xPortRawTime( void );

static void hello_task(void *p) {
  TickType_t last_wake_time;
  
  (void)p;
  last_wake_time = xTaskGetTickCount();
  while (1) {
	uint64_t raw = xPortRawTime();
	uint32_t low = (uint32_t)(raw & 0xFFFFFFFF);
	uint32_t high = ((uint32_t)(raw >> 32)) & 0xFFFFFFFF;
  
	printf_uart("hello %x %x\r\n", high, low);
#ifdef PREEMPTIVE  
	vTaskDelayUntil(&last_wake_time, TICK_INTERVAL);
#else
	taskYIELD();
#endif
  }
}

static void world_task(void *p) {
  TickType_t last_wake_time;
  
  (void)p;

  last_wake_time = xTaskGetTickCount();
  while (1) {
	uint64_t raw = xPortRawTime();
	uint32_t low = (uint32_t)(raw & 0xFFFFFFFF);
	uint32_t high = ((uint32_t)(raw >> 32)) & 0xFFFFFFFF;
  
	printf_uart("world %x %x\r\n", high, low);
#ifdef PREEMPTIVE  
	vTaskDelayUntil(&last_wake_time, TICK_INTERVAL);
#else
	taskYIELD();
#endif
  }
}
int main( void )
{
  /* no need to init uart */
  printf_uart("main: create hello task\r\n");
  xTaskCreate(hello_task, "Hello task", 1000, NULL, 1, NULL);
  printf_uart("main: create world task\r\n");
  xTaskCreate(world_task, "World task", 1000, NULL, 1, NULL);

  printf_uart("timer interval: 0x%x\r\n", TIMER_INTERVAL);

  printf_uart("start scheduler\r\n");
  vTaskStartScheduler();
  
  // scheduler shouldn't get here unless a task calls xTaskEndScheduler()
  for (;;);
}

void vApplicationMallocFailedHook( void );
void vApplicationMallocFailedHook( void )
{
  taskDISABLE_INTERRUPTS();
  for( ;; );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
  ( void ) pcTaskName;
  ( void ) pxTask;
  taskDISABLE_INTERRUPTS();
  for( ;; );
}

void vApplicationTickHook( void );
void vApplicationTickHook( void ) { }
void vApplicationIdleHook( void );
void vApplicationIdleHook( void ) { }
