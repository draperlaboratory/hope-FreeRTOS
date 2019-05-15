#include <stdint.h>
#include "bsp.h"

#if BSP_USE_GPIO
/* Struct for GPIO register space */
struct __attribute__((aligned(4))) gpio_reg_str
{
	/* 0x00 -- Channel 1 GPIO data register */
	volatile uint8_t gpio1_data __attribute__((aligned(4)));

	/* 0x04 -- Channel 1 GPIO 3-state control register */
	volatile uint8_t gpio1_tri __attribute__((aligned(4)));

	/* 0x08 -- Channel 2 GPIO data register */
	volatile uint8_t gpio2_data __attribute__((aligned(4)));

	/* 0x0C -- Channel 2 GPIO 3-state control register */
	volatile uint8_t gpio2_tri __attribute__((aligned(4)));
};

/**
 * Set GPIO_1 index `i`
 * GPIO_1 has only bank 1, bank_2 is for onboard LED control
 * @assert 0 <= i <= 3
 */
void gpio1_write(uint8_t i);

/**
 * Clear GPIO_1 index `i`
 * GPIO_1 has only bank 1, bank_2 is for onboard LED control
 * @assert 0 <= i <= 3
 */
void gpio1_clear(uint8_t i);

/**
 * Read GPIO_1 index `i`
 * Reads from bank_1 only
 * @assert 0 <= i <= 3
 */
uint8_t gpio1_read(uint8_t i);

/**
 * Set GPIO_2 index `i`
 * GPIO_2 has only bank 1
 * @assert 0 <= i <= 1
 */
void gpio2_write(uint8_t i);

/**
 * Clear GPIO_2 index `i`
 * GPIO_2 has only bank 1
 * @assert 0 <= i <= 1
 */
void gpio2_clear(uint8_t i);

/**
 * Turn on LED 0-7
 * Onboard LEDs are on GPIO_1 bank 2
 */
void led_write(uint8_t i);

/**
 * Turn off LED 0-7
 * Onboard LEDs are on GPIO_1 bank 2
 */
void led_clear(uint8_t i);
#endif