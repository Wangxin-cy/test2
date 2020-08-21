/*
 * LEDs driver for GPIOs
 *
 * Copyright (C) 2007 8D Technologies inc.
 * Raphael Assenat <raph@8d.com>
 * Copyright (C) 2008 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/kernel.h>
#include <linux/leds.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/of_gpio.h>
#define GPIO_LOW 0
#define GPIO_HIGH 1


int gpio;

static long led_ioctl( struct file *files, unsigned int cmd,unsigned long arg)
{
	
	printk("cmd is %d\n",cmd);
	if(cmd>0)
		gpio_set_value(gpio,GPIO_HIGH);
	else if(cmd<=0)
		gpio_set_value(gpio,GPIO_LOW);
	return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
	printk(KERN_EMERG "hello release\n");

	return 0;
}

static int led_open(struct inode *inode, struct file *file)
{
	printk(KERN_EMERG "hello open\n");
	return 0;
}

static struct file_operations led_fops = {
	.owner=THIS_MODULE,
	.open=led_open,
	.release=led_release,
	.unlocked_ioctl=led_ioctl,
};
static  struct miscdevice led_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "leds_gpio2",
	.fops = &led_fops,
};

static int gpio_led_probe(struct platform_device *pdev)
{	

	int ret ;
	enum of_gpio_flags flag;
	struct device_node *led_node = pdev->dev.of_node;
	 
	gpio = of_get_named_gpio_flags(led_node,"leds-gpio2", 0,&flag);
	
	if (!gpio_is_valid(gpio)){
	printk(KERN_INFO "hello: invalid gpio : %d\n",gpio);
	return -1;
	}
	ret = gpio_request(gpio, "led");
	if (ret) {
	gpio_free(gpio);
	return -1;
	}

gpio_direction_output(gpio, GPIO_HIGH);

misc_register(&led_dev);
return 0;

}

static int gpio_led_remove(struct platform_device *pdev)
{
	
	printk(KERN_EMERG "\tremove\n");
	misc_deregister(&led_dev);
	return 0;
}
static const struct of_device_id of_gpio_leds_match[] = {
	{ .compatible = "gpio-leds2", },
	{},
};
MODULE_DEVICE_TABLE(of, of_gpio_leds_match);
static struct platform_driver gpio_led_driver = {
	.probe		= gpio_led_probe,
	.remove		= gpio_led_remove,
	.driver		= {
		.name	= "gpio-leds2",
		.owner    = THIS_MODULE,
		.of_match_table = of_gpio_leds_match,
	},
};


static int hello_init(void)
{

	int DriverState;
	
	DriverState = platform_driver_register(&gpio_led_driver);

	return 0;
}

static void hello_exit(void)
{

	platform_driver_unregister(&gpio_led_driver);	
}




module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("Raphael Assenat <raph@8d.com>, Trent Piepho <tpiepho@freescale.com>");
MODULE_DESCRIPTION("GPIO LED driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:leds-gpio");
