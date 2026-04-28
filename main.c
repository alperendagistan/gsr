#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include "adc.h"
#include "app_tasks.h"

int main(void)
{
	int err;
	
	printk("=================================\n");
	printk("    GSR Application Started\n");
	printk("=================================\n");

	/* ADC başlat */
	// err = adc_init();
	// if (err) {
	// 	printk("ADC init failed: %d\n", err);
	// 	return 0;
	// }

	/* Shell'i başlat */
	app_shell_init();

	/* Task'ları başlat */
	heart_beat_task_start();

	printk("Main: waiting for tasks...\n");
	printk("Shell commands: 'reset' to reboot, 'sysinfo' for system info\n\n");

	while (1) {
		k_sleep(K_FOREVER);
	}

	return 0;
}