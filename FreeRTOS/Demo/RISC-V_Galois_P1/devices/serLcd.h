#ifndef __SERLCD_H__
#define __SERLCD_H__

/**
 * Display this text!
 * predicate initialized_display = (display.state == INITIALIZED_DISPLAY)
 */
//@ requires (display.state == INITIALIZED_DISPLAY) || (display.state == SHOWING_TEXT)
//@ requires \valid (the_text);
//@ requires \valid (the_text + (0 .. 15));
// TODO: pass a length of the text + make sure it is <16
// ensures the_text is printed on the LCD
void display_this_text(char *the_text, uint8_t length);

void serLcdPrintf(char *str);

#endif /* __SERLCD_H__ */