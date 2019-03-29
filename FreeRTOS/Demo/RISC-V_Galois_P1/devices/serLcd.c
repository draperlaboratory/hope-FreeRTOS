#include "serLcd.h"
#include <string.h>

#define LCD_DEFAULT_ADDRESS 0x72 //This is the default address of the OpenLCD
#define LCD_MAX_STRLEN 16        // Max length of the text
#define LCD_DEFAULT_BUS Iic0     // Default iic bus

uint8_t cmd_mode[] = {
    '|',
    '-',
};

void serLcdPrintf(char *str)
{
    uint8_t len = strlen(str);
    if (len > LCD_MAX_STRLEN)
    {
        len = LCD_MAX_STRLEN;
    }

    configASSERT(iic_transmit(&LCD_DEFAULT_BUS, LCD_DEFAULT_ADDRESS, cmd_mode, sizeof(cmd_mode)) == XST_SUCCESS);
    configASSERT(iic_transmit(&LCD_DEFAULT_BUS, LCD_DEFAULT_ADDRESS, (uint8_t *)str, len) == XST_SUCCESS);
}
