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
#include "diskio.h"

void main_sd(void);
void print_array(char *name, uint8_t *buf, size_t len);

static void prvSdTestTask0(void *pvParameters);

void print_array(char *name, uint8_t *buf, size_t len)
{
    printf("%s = [\r\n", name);
    for (size_t i = 0; i < len; i++)
    {
        printf("0x%X, ", buf[i]);
        if (i % 10 == 0)
        {
            printf("\r\n");
        }
    }
    printf("];\r\n");
}

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
    DIR dp;

    //printf(" disk_init = %u\r\n", disk_initialize(0));

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
    printf(" disk_init = %u\r\n", disk_initialize(0));

    uint8_t res;
    uint64_t data = 42;
    uint8_t buff[1024] = {0xFF};

    //res = disk_ioctl(0, GET_SECTOR_COUNT, &data);
    //printf("disk_ioctl GET_SECTOR_COUNT: res=%u, data=%llu\r\n", res, data);

    //res = disk_ioctl(0, CTRL_SYNC, &data);
    //printf("disk_ioctl CTRL_SYNC: res=%u, data=%llu\r\n", res, data);

    //res = disk_ioctl(0, GET_BLOCK_SIZE, &data);
    //printf("disk_ioctl GET_BLOCK_SIZE: res=%u, data=%llu\r\n", res, data);

    // memset(buff, 0xFF, sizeof(buff));
    // res = disk_read(0, buff, 0, 1);
    // if (!res) {
    //     printf("read #1 OK: %u\r\n", res);
    //     print_array("buff", buff, 512);
    // }
    // //vTaskDelay(pdMS_TO_TICKS(1000));

    // memset(buff, 0x0, sizeof(buff));
    // res = disk_read(0, buff, 0, 1);
    // if (!res) {
    //     printf("read #2 OK: %u\r\n", res);
    //     print_array("buff", buff, 512);
    // }
    // //vTaskDelay(pdMS_TO_TICKS(1000));

    // memset(buff, 0xA5, sizeof(buff));
    // res = disk_read(0, buff, 0, 1);
    // if (!res) {
    //     printf("read #3 OK: %u\r\n", res);
    //     print_array("buff", buff, 512);
    // }
    // //vTaskDelay(pdMS_TO_TICKS(1000));

    //res = f_mkfs ("0", FM_FAT, 65536, WorkBuf, 65536);

    res = f_mount(&FatFs, "", 1);
    printf("f_mount result = %i\r\n", res); /* Give a work area to the default drive */

    res = f_opendir(&dp, "/");
    printf("f_opendir res = %u\r\n", res);

    res = f_readdir(&dp, &FilInfo);
    printf("f_readdir res = %u\r\n", res);
    printf("filename: %s\r\n", FilInfo.fname);

    printf("terminating, res = %u\r\n", res);
    configASSERT(0);
    for (;;)
    {
        // loop forever
    }
}
