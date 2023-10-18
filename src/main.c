/********************************************************************************
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

/********************************************************************************
 *
 ********************************************************************************/
#define PORT13_PIN07	 	7

//
#define PORT0_PIN04 		4
#define PORT0_PIN05 		5

//
#define DEVICE_GPIO0 	DT_NODELABEL(gpio_prt0)
/*#define DEVICE_GPIO1 	DT_NODELABEL(gpio1)
#define DEVICE_GPIO2  	DT_NODELABEL(gpio2)
#define DEVICE_GPIO3  	DT_NODELABEL(gpio3)
#define DEVICE_GPIO4 	DT_NODELABEL(gpio4)
#define DEVICE_GPIO5 	DT_NODELABEL(gpio5)
#define DEVICE_GPIO6 	DT_NODELABEL(gpio6)
#define DEVICE_GPIO7 	DT_NODELABEL(gpio_prt7)
#define DEVICE_GPIO8 	DT_NODELABEL(gpio8)
#define DEVICE_GPIO9 	DT_NODELABEL(gpio9)
#define DEVICE_GPIO10 	DT_NODELABEL(gpio10)
#define DEVICE_GPIO11 	DT_NODELABEL(gpio11)
#define DEVICE_GPIO12 	DT_NODELABEL(gpio12)*/
#define DEVICE_GPIO13 	DT_NODELABEL(gpio_prt13)
/*#define DEVICE_GPIO14 	DT_NODELABEL(gpio14)
*/

//#define SLEEP_TIME_MS	1000
#define SLEEP_TIME_MS	2000

/*
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 */
#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
	#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

#define LED0_NODE	DT_ALIAS(led0)
#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
	#error "Unsupported board: led0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;
/*
 * The led0 devicetree alias is optional. If present, we'll use it
 * to turn on the LED whenever the button is pressed.
 */

// const struct device *gpio_dev;
const struct device *gpio0_dev = DEVICE_DT_GET(DEVICE_GPIO0);
/*const struct device *gpio1_dev = DEVICE_DT_GET(DEVICE_GPIO1);
const struct device *gpio2_dev = DEVICE_DT_GET(DEVICE_GPIO2);
const struct device *gpio3_dev = DEVICE_DT_GET(DEVICE_GPIO3);
const struct device *gpio4_dev = DEVICE_DT_GET(DEVICE_GPIO4);
const struct device *gpio5_dev = DEVICE_DT_GET(DEVICE_GPIO5);
const struct device *gpio6_dev = DEVICE_DT_GET(DEVICE_GPIO6);
const struct device *gpio7_dev = DEVICE_DT_GET(DEVICE_GPIO7);
/*const struct device *gpio8_dev = DEVICE_DT_GET(DEVICE_GPIO8);
const struct device *gpio9_dev = DEVICE_DT_GET(DEVICE_GPIO9);
const struct device *gpio10_dev = DEVICE_DT_GET(DEVICE_GPIO10);
const struct device *gpio11_dev = DEVICE_DT_GET(DEVICE_GPIO11);
const struct device *gpio12_dev = DEVICE_DT_GET(DEVICE_GPIO12);*/
const struct device *gpio13_dev = DEVICE_DT_GET(DEVICE_GPIO13);
/*const struct device *gpio14_dev = DEVICE_DT_GET(DEVICE_GPIO14);
*/

uint16_t delay_led = 50;

/********************************************************************************
 *
 ********************************************************************************/
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
	if(delay_led < 5000) {
		delay_led = delay_led + 50;
	}
	else {
		delay_led = 50;
	}
	printk("delay led %d\n\n\n", delay_led);
}

/********************************************************************************
 *
 ********************************************************************************/
int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&gpio13_dev)) {
		return;
	}

	if (!gpio_is_ready_dt(&gpio0_dev)) {
		return;
	}
	
	ret = gpio_pin_configure(gpio13_dev, PORT13_PIN07, GPIO_OUTPUT);
	if (ret < 0) {
		return;
	}

	ret = gpio_pin_configure(gpio0_dev, PORT0_PIN05, GPIO_OUTPUT);
	if (ret < 0) {
		return;
	}

	while (1) {
		ret = gpio_pin_set(gpio13_dev, PORT13_PIN07, false);
		ret = gpio_pin_set(gpio0_dev, PORT0_PIN05, true);
		k_msleep(SLEEP_TIME_MS);	
		ret = gpio_pin_set(gpio13_dev, PORT13_PIN07, true);
		ret = gpio_pin_set(gpio0_dev, PORT0_PIN05, false);
		k_msleep(SLEEP_TIME_MS);
	}

	if (!gpio_is_ready_dt(&led)) {
		return;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
	if (ret < 0) {
		return;
	}

	while (1) {
		ret = gpio_pin_set_dt(&led, false);
		k_msleep(SLEEP_TIME_MS);	
		ret = gpio_pin_set_dt(&led, true);
		k_msleep(SLEEP_TIME_MS);
	}


	if (!gpio_is_ready_dt(&button)) {
		printk("Error: button device %s is not ready\n",
		       button.port->name);
		return 0;
	}

	ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, button.port->name, button.pin);
		return 0;
	}

	ret = gpio_pin_interrupt_configure_dt(&button,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, button.port->name, button.pin);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);
	printk("Set up button at %s pin %d\n", button.port->name, button.pin);

	if (led.port && !device_is_ready(led.port)) {
		printk("Error %d: LED device %s is not ready; ignoring it\n",
		       ret, led.port->name);
		led.port = NULL;
	}
	if (led.port) {
		ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
		if (ret != 0) {
			printk("Error %d: failed to configure LED device %s pin %d\n",
			       ret, led.port->name, led.pin);
			led.port = NULL;
		} else {
			printk("Set up LED at %s pin %d\n", led.port->name, led.pin);
		}
	}

	printk("Press the button\n");
	if (led.port) {
		while (1) {
			/* If we have an LED, match its state to the button's. */
			int val = gpio_pin_get_dt(&button);
			/*
			if (val >= 0) {
				gpio_pin_set_dt(&led, val);
			}
			k_msleep(SLEEP_TIME_MS);
			*/

			ret = gpio_pin_toggle_dt(&led);
			if (ret < 0) {
				return 0;
			}
			k_msleep(delay_led);
		}
	}
	return 0;
}

/********************************************************************************
 *
 ********************************************************************************/
