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

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

// Drivers
#include "iic.h"
#include "ds1338rtc.h"

// Define as 1 if you want to set the clock time as well
#define RTC_SET_TIME 0

// The time below correcsponds to of the time structure to July 27, 2019 and
// the time to 9:55:00 am
#define RTC_YEAR 19
#define RTC_MONTH 7
#define RTC_DAY 24
#define RTC_HOUR 9
#define RTC_MINUTE 55
#define RTC_SECONDS 0

#if !(BSP_USE_IIC0)
#error "One or more peripherals are nor present, this test cannot be run"
#endif

void main_rtc(void);

static void prvIicTestTask0(void *pvParameters);
/*-----------------------------------------------------------*/

void main_rtc(void)
{
	xTaskCreate(prvIicTestTask0, "prvIicTestTask0", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
}
/*-----------------------------------------------------------*/


static void prvIicTestTask0(void *pvParameters)
{
	(void)pvParameters;
    // A structure to hold the time read from the device
    struct rtctime_t time;

    // A character array to hold the string representation of the time
    char time_str[20];

	printf("Starting prvIicTestTask0\r\n");

	// Give the sensor time to power up
	vTaskDelay(pdMS_TO_TICKS(100));

    ds1338_clean_osf();
    ds1338_disable_sqw();

#if RTC_SET_TIME
    make_time(&time, RTC_YEAR, RTC_MONTH, RTC_DAY, RTC_HOUR, RTC_MINUTE, RTC_SECONDS);

    // Set the clock
    if (ds1338_write_time(&time))
    {
        printf("Unable to set the time--an I2C error has occured.\r\n");
    }
    else
    {
        printf("Clock set successfully!\r\n");
    }
#endif /* RTC_SET_TIME */

	for (;;)
	{
		ds1338_read_time(&time);
        format_time_str(&time, time_str);
        printf("%s\r\n",time_str);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
