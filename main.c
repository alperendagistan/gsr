#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/dfu/mcuboot.h>

#include "main.h"
#include "adc.h"
#include "app_tasks.h"
#include "ble_dfu.h"

int main(void)
{
	int err;

	printk("=================================\n");
	printk("    GSR Application v" APP_VERSION_STRING "\n");
	printk("=================================\n");

	/* MCUboot: confirm current image so bootloader won't revert */
	if (!boot_is_img_confirmed()) {
		err = boot_write_img_confirmed();
		if (err) {
			printk("Image confirm failed (%d)\n", err);
		} else {
			printk("Image confirmed\n");
		}
	}

	/* Shell */
	app_shell_init();

	/* BLE + mcumgr SMP for OTA DFU */
	if (ble_dfu_init()) {
		printk("BLE DFU init failed\n");
	}

	/* Tasks */
	heart_beat_task_start();

	printk("System ready. Shell on UART, DFU over BLE.\n");

	while (1) {
		k_sleep(K_FOREVER);
	}

	return 0;
}