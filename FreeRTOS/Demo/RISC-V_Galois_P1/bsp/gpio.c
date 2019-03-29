#include "gpio.h"

#if BSP_USE_GPIO
static struct gpio_reg_str *gpio_reg = (void *)XPAR_GPIO_BASEADDR;

void gpio1_write(uint8_t i)
{
	configASSERT(i <= 1);
	gpio_reg->gpio1_data |= (1 << i);
}

void gpio1_clear(uint8_t i)
{
	configASSERT(i <= 1);
	gpio_reg->gpio1_data &= ~(1 << i);
}

void gpio2_write(uint8_t i)
{
	configASSERT(i <= 7);
	gpio_reg->gpio2_data |= (1 << i);
}

void gpio2_clear(uint8_t i)
{
	configASSERT(i <= 7);
	gpio_reg->gpio2_data &= ~(1 << i);
}
#endif