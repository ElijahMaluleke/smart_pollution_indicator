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
//#define SLEEP_TIME_MS		1000
#define SLEEP_TIME_MS		2000

#define version				"v1.0.0"

#define LED_ON					0
#define LED_OFF					1

#define BUZZER_ON			0
#define BUZZER_OFF			1

#define PORT0_PIN04 	 	4
#define BUTTON		     	PORT0_PIN04
#define PORT13_PIN04	 	4
#define BUZZER			 	PORT13_PIN04
#define PORT13_PIN07 		7	 	
#define LED4				PORT13_PIN07

#define port0				"gpio_prt0"
#define port13				"gpio_prt13"

//
#define DEVICE_GPIO0 		DT_NODELABEL(gpio_prt0)
//#define DEVICE_GPIO1 		DT_NODELABEL(gpio_prt1)
//#define DEVICE_GPIO2  		DT_NODELABEL(gpio_prt2)
#define DEVICE_GPIO3  		DT_NODELABEL(gpio_prt3)
//#define DEVICE_GPIO4 		DT_NODELABEL(gpio_prt4)
#define DEVICE_GPIO5 		DT_NODELABEL(gpio_prt5)
#define DEVICE_GPIO6 		DT_NODELABEL(gpio_prt6)
//#define DEVICE_GPIO7 		DT_NODELABEL(gpio_prt7)
//#define DEVICE_GPIO8 		DT_NODELABEL(gpio_prt8)
#define DEVICE_GPIO9 		DT_NODELABEL(gpio_prt9)
//#define DEVICE_GPIO10 		DT_NODELABEL(gpio_prt10)
//#define DEVICE_GPIO11 		DT_NODELABEL(gpio_prt11)
#define DEVICE_GPIO12 		DT_NODELABEL(gpio_prt12)
#define DEVICE_GPIO13 		DT_NODELABEL(gpio_prt13)
//#define DEVICE_GPIO14 		DT_NODELABEL(gpio_prt14)

// 
const struct device *gpio0_dev = DEVICE_DT_GET(DEVICE_GPIO0);
//const struct device *gpio1_dev = DEVICE_DT_GET(DEVICE_GPIO1);
//const struct device *gpio2_dev = DEVICE_DT_GET(DEVICE_GPIO2);
const struct device *gpio3_dev = DEVICE_DT_GET(DEVICE_GPIO3);
//const struct device *gpio4_dev = DEVICE_DT_GET(DEVICE_GPIO4);
const struct device *gpio5_dev = DEVICE_DT_GET(DEVICE_GPIO5);
const struct device *gpio6_dev = DEVICE_DT_GET(DEVICE_GPIO6);
//const struct device *gpio7_dev = DEVICE_DT_GET(DEVICE_GPIO7);
//const struct device *gpio8_dev = DEVICE_DT_GET(DEVICE_GPIO8);
const struct device *gpio9_dev = DEVICE_DT_GET(DEVICE_GPIO9);
//const struct device *gpio10_dev = DEVICE_DT_GET(DEVICE_GPIO10);
//const struct device *gpio11_dev = DEVICE_DT_GET(DEVICE_GPIO11);
const struct device *gpio12_dev = DEVICE_DT_GET(DEVICE_GPIO12);
const struct device *gpio13_dev = DEVICE_DT_GET(DEVICE_GPIO13);
//const struct device *gpio14_dev = DEVICE_DT_GET(DEVICE_GPIO14);

uint16_t delay_led = 1000;

/********************************************************************************
 * Get button configuration from the devicetree sw0 alias. This is mandatory.
 ********************************************************************************/
static struct gpio_callback button_cb_data;

/********************************************************************************
 *
 ********************************************************************************/
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
	if(delay_led < 5000) {
		delay_led = delay_led + 1000;
		}
	else {
		delay_led = 1000;
	}
	printk("delay led %d\n\n\n", delay_led);
}

/********************************************************************************
 *
 ********************************************************************************/
void LED_BUZZER(void)
{
	int ret;

	ret = gpio_pin_set(gpio13_dev, LED4, LED_ON);
	ret = gpio_pin_set(gpio13_dev, BUZZER, BUZZER_ON);
	k_msleep(1000);	
	ret = gpio_pin_set(gpio13_dev, LED4, LED_OFF);
	ret = gpio_pin_set(gpio13_dev, BUZZER, BUZZER_OFF);
	k_msleep(1000);
}

/********************************************************************************
 *
 ********************************************************************************/
int main(void)
{
	int ret, i;

	printk("\nA Smart Air Pollution and Noise Level indicator IoT Project: %s\n\n\n", version);
	
	k_msleep(5000);

	if (!gpio_is_ready_dt(&gpio0_dev)) {
		printk("Error: button device %s is not ready\n", port0);
		return 0;
	}

	if (!gpio_is_ready_dt(&gpio13_dev)) {
		printk("Error: button device %s is not ready\n", port13);
		return 0;
	}
	
	ret = gpio_pin_configure(gpio13_dev, LED4, GPIO_OUTPUT);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure(gpio13_dev, BUZZER, GPIO_OUTPUT);
	if (ret < 0) {
		return 0;
	}

	ret = gpio_pin_configure(gpio0_dev, BUTTON, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n", ret, port0, BUTTON);
		return 0;
	}

	ret = gpio_pin_interrupt_configure(gpio0_dev, BUTTON, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n", ret, port0, BUTTON);
		return 0;
	}

	gpio_init_callback(&button_cb_data, button_pressed, BIT(BUTTON));
	gpio_add_callback(gpio0_dev, &button_cb_data);
	printk("Set up button at %s pin %d\n", port0, BUTTON);

	printk("Press the button\n");
	
	for(i = 0; i < 5; i++) {
		ret = gpio_pin_set(gpio13_dev, LED4, LED_ON);
		ret = gpio_pin_set(gpio13_dev, BUZZER, BUZZER_ON);
		k_msleep(delay_led);	
		ret = gpio_pin_set(gpio13_dev, LED4, LED_OFF);
		ret = gpio_pin_set(gpio13_dev, BUZZER, BUZZER_OFF);
		k_msleep(delay_led);
	}

	while (1) {
		k_msleep(30000);
		LED_BUZZER();
	}
	return 0;
}

/********************************************************************************
 *
 ********************************************************************************/