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
#include "bsp.h"

#include "ff.h" /* Declarations of FatFs API */

void main_sd(void);

static void prvSdTestTask0(void *pvParameters);

void main_sd(void)
{
    xTaskCreate(prvSdTestTask0, "prvSdTestTask0", configMINIMAL_STACK_SIZE * 3U, NULL, tskIDLE_PRIORITY + 1, NULL);
}

/**
 * This tasks controls GPIO and the connected motors
 */
static void prvSdTestTask0(void *pvParameters)
{
    (void)pvParameters;

    FATFS FatFs; /* FatFs work area needed for each volume */
    FIL Fil;     /* File object needed for each open file */
    FILINFO FilInfo;
    UINT bw;
    DIR d;

    

    // if (f_open(&Fil, "newfile.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {	/* Create a file */
    // 	f_write(&Fil, "It works!\r\n", 11, &bw);	/* Write data to the file */
    //  	f_close(&Fil);								/* Close the file */
    //  	if (bw == 11) {		/* Lights green LED if data written well */
    //  		printf("data written sucessfully\r\n");
    //  	} else {
    //  		printf("data write error!\r\n");
    //  	}
      //} else {
     // 	printf("f_mount error!\r\n");
      //}
    //printf(" disk_init = %u\r\n", disk_initialize(0));
    //uint64_t data = 42;
    //uint8_t res = disk_ioctl(0, GET_SECTOR_COUNT, &data);
    //printf("disk_ioctl: res=%u, data=%llu\r\n", res, data);
    uint8_t res;
    res = f_mount(&FatFs, "", 1);
    printf("f_mount result = %i\r\n", res);		/* Give a work area to the default drive */  
    printf("terminating\r\n");
    //configASSERT(0);
}
