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
#include "stream_buffer.h"

// Drivers
#include "uart.h"
#include "gpio.h"

// Devices
#include "vcnl4010.h"
#include "serLcd.h"

#if !(BSP_USE_IIC0 && BSP_USE_IIC0 && BSP_USE_UART0 && BSP_USE_UART1 && BSP_USE_GPIO && BSP_USE_SPI1)
#error "One or more peripherals are nor present, this test cannot be run"
#endif 

/*-----------------------------------------------------------*/
#define PROXIMITY_THRESHOLD 200
#define PROXIMITY_DELAY_MS 100
#define BUFFER_WAIT_TIME_MS 50
#define LED_PROXIMITY_0_A 0
#define LED_PROXIMITY_0_B 1
#define LED_PROXIMITY_0_C 2
#define LED_PROXIMITY_0_D 3

#define LED_PROXIMITY_1_A 4
#define LED_PROXIMITY_1_B 5
#define LED_PROXIMITY_1_C 6
#define LED_PROXIMITY_1_D 7

#define sbiSTREAM_BUFFER_LENGTH_BYTES ((size_t)100)
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_1 ((BaseType_t)1)
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_5 ((BaseType_t)5)

void main_drivers(void);

static void prvIicTestTask0(void *pvParameters);
static void prvIicTestTask1(void *pvParameters);
static void prvUartTxTestTask(void *pvParameters);
static void prvUartRx0TestTask(void *pvParameters);
static void prvUartRx1TestTask(void *pvParameters);
static void prvLcdTestTask(void *pvParameters);
static void prvMotorControlTask(void *pvParameters);

/* The stream buffer that is used to send data from an interrupt to the task. */
static StreamBufferHandle_t xStreamBufferUartToLcd = NULL;
static StreamBufferHandle_t xStreamBufferProxyToMotor = NULL;
/*-----------------------------------------------------------*/

void main_drivers(void)
{
	/* Create the stream buffer that sends data from the interrupt to the
	task, and create the task. */
	xStreamBufferUartToLcd = xStreamBufferCreate(/* The buffer length in bytes. */
										sbiSTREAM_BUFFER_LENGTH_BYTES,
										/* The stream buffer's trigger level. */
										sbiSTREAM_BUFFER_TRIGGER_LEVEL_5);

	xStreamBufferProxyToMotor = xStreamBufferCreate(/* The buffer length in bytes. */
										sbiSTREAM_BUFFER_LENGTH_BYTES,
										/* The stream buffer's trigger level. */
										sbiSTREAM_BUFFER_TRIGGER_LEVEL_1);

	xTaskCreate(prvIicTestTask0, "prvIicTestTask0", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvIicTestTask1, "prvIicTestTask1", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvUartTxTestTask, "prvUartTxTestTask", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvUartRx0TestTask, "prvUartRx0TestTask", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvUartRx1TestTask, "prvUartRx1TestTask", configMINIMAL_STACK_SIZE * 2U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvLcdTestTask, "prvLcdTestTask", configMINIMAL_STACK_SIZE * 3U, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(prvMotorControlTask, "prvMotorControlTask", configMINIMAL_STACK_SIZE * 3U, NULL, tskIDLE_PRIORITY + 1, NULL);
}
/*-----------------------------------------------------------*/

/**
 * This tasks controls GPIO and the connected motors
 */
static void prvMotorControlTask(void *pvParameters)
{
	(void)pvParameters;
	uint8_t rx_buf[sbiSTREAM_BUFFER_LENGTH_BYTES] = {0};
	size_t retval = 0;

	printf("Starting prvMotorControlTask\r\n");

	/* Set motors to stop (all GPIOs are cleared) */
	gpio1_clear(0);
	gpio1_clear(1);
	gpio1_clear(2);
	gpio1_clear(3);

	for (;;) {
		retval = xStreamBufferReceive(xStreamBufferProxyToMotor,
									  (void *)rx_buf,
								 	  sizeof(rx_buf),
									  pdMS_TO_TICKS(BUFFER_WAIT_TIME_MS));

		if (retval > 0) {
			/* A message was received */
			if (rx_buf[0] == 1) {
				/* Set both motors on */ 
				gpio1_write(0);
				//gpio1_write(1);
				gpio1_write(2);
				//gpio1_write(3);
			} else {
			/* Set motors to stop (all GPIOs are cleared) */
			gpio1_clear(0);
			gpio1_clear(1);
			gpio1_clear(2);
			gpio1_clear(3);
			}
			/* Sleep for a bit */ 
			vTaskDelay(pdMS_TO_TICKS(100));
		}
	}

}

/**
 * This tasks displays data received over StreamBuffer on SerLCD connected
 * on i2c bus.
 */
static void prvLcdTestTask(void *pvParameters)
{
	(void)pvParameters;
	char str[16];
	size_t retval = 0;
	char *no_data_msg = "Scan code...";

	printf("Starting prvLcdTestTask\r\n");

	for (;;)
	{
		/* Keep receiving characters until the end of the string is received.
		Note:  An infinite block time is used to simplify the example.  Infinite
		block times are not recommended in production code as they do not allow
		for error recovery. */
		retval = xStreamBufferReceive(/* The stream buffer data is being received from. */
									  xStreamBufferUartToLcd,
									  /* Where to place received data. */
									  (void *)str,
									  /* The number of bytes to receive. */
									  sizeof(str) - 1,
									  /* The time to wait for the next data if the buffer
							  is empty. */
									  pdMS_TO_TICKS(1000));
		if (retval == 0)
		{
			serLcdPrintf(no_data_msg);
		}
		else
		{
			//printf("received %u bytes, printing\r\n", retval);
			str[retval] = '\0';
			serLcdPrintf(str);
		}
	}
}

/**
 * This tasks handles reading from a proximity sensor
 * located at i2c bus 1
 * An onboard LED is triggered if a close object is detected.
 */
static void prvIicTestTask1(void *pvParameters)
{
	(void)pvParameters;
	struct Vcnl4010_t sensor1;
	uint16_t proximity = 0;
	uint16_t ambient_light = 0;

	uint8_t tx_buf[sbiSTREAM_BUFFER_LENGTH_BYTES] = {0};

	printf("Starting prvIicTestTask1\r\n");
	configASSERT(vcnl4010_init(&sensor1, &Iic1));

	for (;;)
	{
		proximity = vcnl4010_readProximity(&sensor1);
		printf("#1 Proximity: %u\r\n", proximity);
		if (proximity > PROXIMITY_THRESHOLD)
		{
			gpio2_write(LED_PROXIMITY_1_A);
			gpio2_write(LED_PROXIMITY_1_B);
			gpio2_write(LED_PROXIMITY_1_C);
			gpio2_write(LED_PROXIMITY_1_D);
			
			tx_buf[0] = 1;
			xStreamBufferSend(xStreamBufferProxyToMotor,(const void *)tx_buf,1,pdMS_TO_TICKS(BUFFER_WAIT_TIME_MS));
		}
		else
		{
			gpio2_clear(LED_PROXIMITY_1_A);
			gpio2_clear(LED_PROXIMITY_1_B);
			gpio2_clear(LED_PROXIMITY_1_C);
			gpio2_clear(LED_PROXIMITY_1_D);

			tx_buf[0] = 0;
			xStreamBufferSend(xStreamBufferProxyToMotor,(const void *)tx_buf,1,pdMS_TO_TICKS(BUFFER_WAIT_TIME_MS));
		}

		ambient_light = vcnl4010_readAmbient(&sensor1);
		printf("#1 ambient: %u\r\n", ambient_light);

		vTaskDelay(pdMS_TO_TICKS(PROXIMITY_DELAY_MS));
	}
}

/**
 * This tasks handles reading from a proximity sensor
 * located at i2c bus 0
 * An onboard LED is triggered if a close object is detected.
 */
static void prvIicTestTask0(void *pvParameters)
{
	(void)pvParameters;
	struct Vcnl4010_t sensor0;
	uint16_t proximity = 0;
	uint16_t ambient_light = 0;

	printf("Starting prvIicTestTask0\r\n");
	configASSERT(vcnl4010_init(&sensor0, &Iic0));

	for (;;)
	{
		proximity = vcnl4010_readProximity(&sensor0);
		printf("#0 Proximity: %u\r\n", proximity);
		if (proximity > PROXIMITY_THRESHOLD)
		{
			gpio2_write(LED_PROXIMITY_0_A);
			gpio2_write(LED_PROXIMITY_0_B);
			gpio2_write(LED_PROXIMITY_0_C);
			gpio2_write(LED_PROXIMITY_0_D);
		}
		else
		{
			gpio2_clear(LED_PROXIMITY_0_A);
			gpio2_clear(LED_PROXIMITY_0_B);
			gpio2_clear(LED_PROXIMITY_0_C);
			gpio2_clear(LED_PROXIMITY_0_D);
		}

		ambient_light = vcnl4010_readAmbient(&sensor0);
		printf("#0 ambient: %u\r\n", ambient_light);

		vTaskDelay(pdMS_TO_TICKS(PROXIMITY_DELAY_MS));
	}
}

/**
 * This tasks sends a string representation of an incrementing
 * counter to UART1, with the period of 1s
 */
static void prvUartTxTestTask(void *pvParameters)
{
	(void)pvParameters;
	int cnt = 0;
	char str[12];

	printf("Starting prvUartTxTestTask\r\n");
	for (;;)
	{
		sprintf(str, "%d\r\n", cnt);
		int len = uart1_txbuffer(str, strlen(str));
		if (len == -1)
		{
			printf("Timeout\r\n");
		}
		cnt++;
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

/**
 * This tasks receives data on UART0 and immediately echoes
 * them back. It also blinks onboard LEDs to signalize success/error.
 */
static void prvUartRx0TestTask(void *pvParameters)
{
	(void)pvParameters;
	int cnt = 0;
	char str[12];

	printf("Starting prvUartRx0TestTask\r\n");

	for (;;)
	{
		int len = uart0_rxbuffer(str, 1);
		if (len == -1)
		{
			printf("prvUartRx0TestTask rx error\r\n");
		}
		else
		{
			uart0_txbuffer(str, 1);
		}
		cnt++;
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

/**
 * This task reads data from UART1 until we get 0xd (Carriage Return)
 * At that point, the stored data are sent over StreamBuffer to LCD task
 * to be displayed.
 */
static void prvUartRx1TestTask(void *pvParameters)
{
	(void)pvParameters;
	int cnt = 0;
	char str[12] = {0};
	char msg[16] = {0};
	uint8_t idx = 0;

	printf("Starting prvUartRx1TestTask\r\n");

	for (;;)
	{
		int len = uart1_rxbuffer(str, 1);
		if (len == -1)
		{
			printf("prvUartRx1TestTask rx error\r\n");
		}
		else
		{
			char c = str[0];
			if (c == 0xd)
			{
				/* Send the next four bytes to the stream buffer. */
				printf("Sending %u bytes\r\n", idx);
				xStreamBufferSend(xStreamBufferUartToLcd,
								  (const void *)msg,
								  idx,
								  pdMS_TO_TICKS(1000));
				idx = 0;
			}
			else
			{
				msg[idx] = str[0];
				idx++;
				if (idx > 15)
				{
					idx = 0;
				}
			}
		}
		cnt++;
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}