#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "utils.h"
#include "uart_task.h"

void printf_uart(const char* s, ...)
{
  char buf[128];
  va_list vl;

  va_start(vl, s);
  vsnprintf(buf, sizeof buf, s, vl);
  va_end(vl);

  uart_print(buf);
}

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
  vTaskDelayUntil(&last_wake_time, 1);
  taskYIELD();
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
    vTaskDelayUntil(&last_wake_time, 1);
    taskYIELD();
  }
}
int main( void )
{
  printf_uart("main: init uart\r\n");
  init_uart(UART_TASK_DEFAULT_PRIORITY);
  printf_uart("main: create hello task\r\n");
  xTaskCreate(hello_task, "Hello task", 1000, NULL, 1, NULL);
  printf_uart("main: create world task\r\n");
  xTaskCreate(world_task, "World task", 1000, NULL, 1, NULL);

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
