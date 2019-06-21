#include "serLcd.h"
#include <string.h>

#define LCD_USE_SPI 0

#if LCD_USE_SPI
#include "spi.h"
#include <stdio.h>
#define LCD_SPI_SLAVE_IDX 1
#else
#include "iic.h"
#define LCD_DEFAULT_BUS Iic0     // Default iic bus
#define LCD_DEFAULT_ADDRESS 0x72 //This is the default address of the OpenLCD
#endif

#define LCD_MAX_LINELEN 16        // Max length of the text
#define LCD_COMMAND_CODE 254      // Code to send before changing line
#define LCD_LINE_2_CODE 64

uint8_t cmd_mode[] = {
    '|',
    '-',
};

uint8_t go_to_line_2[] = {
	LCD_COMMAND_CODE,
	LCD_LINE_2_CODE,
};

void serLcdPrintf(char *str, uint8_t len)
{
    if (len > LCD_MAX_LINELEN)
    {
        len = LCD_MAX_LINELEN;
    }
#if LCD_USE_SPI
    configASSERT(spi1_transmit(LCD_SPI_SLAVE_IDX, cmd_mode, sizeof(cmd_mode)) != -1);
    vTaskDelay(pdMS_TO_TICKS(100));
    configASSERT(spi1_transmit(LCD_SPI_SLAVE_IDX, (uint8_t*)str, len) != -1);
#else
    configASSERT(iic_transmit(&LCD_DEFAULT_BUS, LCD_DEFAULT_ADDRESS, cmd_mode, sizeof(cmd_mode)) != -1);
    vTaskDelay(pdMS_TO_TICKS(100));
    configASSERT(iic_transmit(&LCD_DEFAULT_BUS, LCD_DEFAULT_ADDRESS, (uint8_t *)str, len) != -1);
#endif
}

void serLcdPrintTwoLines(char* line_1, uint8_t len_1, char* line_2, uint8_t len_2) 
{
    if (len_1 > LCD_MAX_LINELEN)
    {
    	len_1 = LCD_MAX_LINELEN;    
    }
    if (len_2 > LCD_MAX_LINELEN)
    {
    	len_2 = LCD_MAX_LINELEN;
    }
#if LCD_USE_SPI
    configASSERT(spi1_transmit(LCD_SPI_SLAVE_IDX, cmd_mode, sizeof(cmd_mode)) != -1);
    vTaskDelay(pdMS_TO_TICKS(100));
    configASSERT(spi1_transmit(LCD_SPI_SLAVE_IDX, (uint8_t*)line_1, len_1) != -1);
    configASSERT(spi1_transmit(LCD_SPI_SLAVE_IDX, go_to_line_2, sizeof(go_to_line_2)) != -1);
    configASSERT(spi1_transmit(LCD_SPI_SLAVE_IDX, (uint8_t*)line_2, len_2) != -1);    
#else
    configASSERT(iic_transmit(&LCD_DEFAULT_BUS, LCD_DEFAULT_ADDRESS, cmd_mode, sizeof(cmd_mode)) != -1);
    vTaskDelay(pdMS_TO_TICKS(100));
    configASSERT(iic_transmit(&LCD_DEFAULT_BUS, LCD_DEFAULT_ADDRESS, (uint8_t *)line_1, len_1) != -1);
    configASSERT(iic_transmit(&LCD_DEFAULT_BUS, LCD_DEFAULT_ADDRESS, go_to_line_2, sizeof(go_to_line_2)) != -1);
    configASSERT(iic_transmit(&LCD_DEFAULT_BUS, LCD_DEFAULT_ADDRESS, (uint8_t *)line_2, len_2) != -1);
#endif
}
