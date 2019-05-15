#include "gpio.h"

#if BSP_USE_GPIO

#define GPIO1_MAX 3
#define GPIO2_MAX 1
#define LED_MAX 7

static struct gpio_reg_str *gpio_reg_1 = (void *)XPAR_GPIO_1_BASEADDR;
static struct gpio_reg_str *gpio_reg_2 = (void *)XPAR_GPIO_2_BASEADDR;

void led_write(uint8_t i)
{
	configASSERT(i <= LED_MAX);
	gpio_reg_1->gpio2_data |= (1 << i);
}

void led_clear(uint8_t i)
{
	configASSERT(i <= LED_MAX);
	gpio_reg_1->gpio2_data &= ~(1 << i);
}

uint8_t gpio1_read(uint8_t i)
{
	return ((gpio_reg_1->gpio1_data >> i) & 0x1);
}

void gpio1_write(uint8_t i)
{
	configASSERT(i <= GPIO1_MAX);
	gpio_reg_1->gpio1_data |= (1 << i);
}

void gpio1_clear(uint8_t i)
{
	configASSERT(i <= GPIO1_MAX);
	gpio_reg_1->gpio1_data &= ~(1 << i);
}

void gpio2_write(uint8_t i)
{
	configASSERT(i <= GPIO2_MAX);
	gpio_reg_2->gpio1_data |= (1 << i);
}

void gpio2_clear(uint8_t i)
{
	configASSERT(i <= GPIO2_MAX);
	gpio_reg_2->gpio1_data &= ~(1 << i);
}
#endif