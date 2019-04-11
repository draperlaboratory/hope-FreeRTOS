#include "gpio.h"

#if BSP_USE_GPIO

#define GPIO1_MAX 3
#define GPIO2_MAX 7
static struct gpio_reg_str *gpio_reg = (void *)XPAR_GPIO_BASEADDR;

void gpio1_write(uint8_t i)
{
	configASSERT(i <= GPIO1_MAX);
	gpio_reg->gpio1_data |= (1 << i);
}

void gpio1_clear(uint8_t i)
{
	configASSERT(i <= GPIO1_MAX);
	gpio_reg->gpio1_data &= ~(1 << i);
}

void gpio2_write(uint8_t i)
{
	configASSERT(i <= GPIO2_MAX);
	gpio_reg->gpio2_data |= (1 << i);
}

void gpio2_clear(uint8_t i)
{
	configASSERT(i <= GPIO2_MAX);
	gpio_reg->gpio2_data &= ~(1 << i);
}
#endif