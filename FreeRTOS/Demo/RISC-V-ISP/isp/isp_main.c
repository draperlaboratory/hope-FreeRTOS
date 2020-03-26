/*
 * Copyright © 2017-2019 The Charles Stark Draper Laboratory, Inc. and/or Dover Microsystems, Inc.
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

/******************************************************************************
 * main_isp() creates a test task and starts the scheduler.  By default this test
 * task prints a message, but it can be overwritten during linking to run other
 * tests.
 ******************************************************************************/

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "isp_utils.h"

/*-----------------------------------------------------------*/

void isp_main_task(void*);
xTaskHandle xIspTask;

/*-----------------------------------------------------------*/

void main_isp( void )
{
	xTaskCreate(isp_main_task, "Main task", 1000, NULL, 1, &xIspTask);

	vTaskStartScheduler();

	// never reached
	return;
}

/*-----------------------------------------------------------*/

void isp_main_task(void *argument)
{
  unsigned long result;

  result = (unsigned long)isp_main();
  vTaskDelay(1);

  t_printf("\nMain task has completed with code: 0x%08x\n", result);

  for( ;; );

  // this may need changes to portable layer
  vTaskEndScheduler();
}

/*-----------------------------------------------------------*/
