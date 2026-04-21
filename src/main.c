#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "adc.h"

int main(void)
{
	int err;

	printk("ADC periodic sampling start\n");

	err = adc_init();
	if (err) {
		printk("ADC init failed: %d\n", err);
		return 0;
	}

	adc_start_periodic_sampling();

	while (1) {
		k_sleep(K_FOREVER);
	}

	return 0;
}