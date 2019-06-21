#ifndef __SERLCD_H__
#define __SERLCD_H__

#include <stdint.h>

void serLcdPrintf(char *str, uint8_t len);
void serLcdPrintTwoLines(char* line_1, uint8_t len_1, char* line_2, uint8_t len_2);

#endif /* __SERLCD_H__ */