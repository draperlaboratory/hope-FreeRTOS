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

/*
 * main_gpio() creates one task to test the GPIO and then starts the
 * scheduler.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo includes. */
#include "gpio.h"

/*-----------------------------------------------------------*/
#define MAIN_GPIO_DELAY_MS 100
/*-----------------------------------------------------------*/

/*
 * Called by main when PROG=main_gpio
 */
void main_gpio(void);

/*
 * The tasks as described in the comments at the top of this file.
 */
static void vTestGPIO_output(void *pvParameters);
static void vTestGPIO_input(void *pvParameters);
static void vTestLED(void *pvParameters);

/*-----------------------------------------------------------*/

void main_gpio(void)
{
    /* Create GPIO test */
    xTaskCreate(vTestGPIO_output, "GPIO Output Test", 1000, NULL, 0, NULL);
    xTaskCreate(vTestGPIO_input, "GPIO Input Test", 1000, NULL, 0, NULL);
    xTaskCreate(vTestLED, "LED Test", 1000, NULL, 0, NULL);
}
/*-----------------------------------------------------------*/


void vTestGPIO_input(void *pvParameters)
{
    (void)pvParameters;

    // GPIO Input has a pull-up, it is high unless pulled to the ground
    uint8_t input[4] = {1};
    uint8_t input_last[4] = {1};

    for (;;)
    {
        input[0] = gpio1_read(0);
        input[1] = gpio1_read(1);
        input[2] = gpio1_read(2);
        input[3] = gpio1_read(3);

        for (uint8_t idx = 0; idx < 4; idx++) {
            if (input[idx] != input_last[idx]) {
                printf("#%u changed: %u -> %u\r\n", idx, input_last[idx], input[idx]);
                input_last[idx] = input[idx];
            }
        }

        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
    }
}

void vTestGPIO_output(void *pvParameters)
{
    /* vTestGPIO() tests the AXI GPIO on the VCU118 by 
    testing that the output pins and the LEDs can be written to.
    This should be verified by measuring the pins and looking
    at the LEDs. */

    (void)pvParameters;

    // Delay so the input thread can catch up
    vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS*2));

    /* GPIO are already set in hardware to be outputs */
    for (;;)
    {
        /***** WRITE TO PINS *****/
        /* Write to GPIO_2 pin #0 */
        gpio2_write(0);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));

        /* Write to GPIO_2 pin #1 */
        gpio2_write(1);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));

        /* Clear GPIO_2 pin #0 */
        gpio2_clear(0);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));

        /* Clear GPIO_2 pin #1 */
        gpio2_clear(1);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));   
    }
}

void vTestLED(void *pvParameters)
{
    (void)pvParameters;

    for(;;)
    {
        /* Write to every LED */
        led_write(0);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_write(1);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_write(2);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_write(3);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_write(4);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_write(5);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_write(6);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_write(7);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));

        /* Clear every LED */
        led_clear(0);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_clear(1);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_clear(2);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_clear(3);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_clear(4);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_clear(5);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_clear(6);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
        led_clear(7);
        vTaskDelay(pdMS_TO_TICKS(MAIN_GPIO_DELAY_MS));
    }
}

/*-----------------------------------------------------------*/
