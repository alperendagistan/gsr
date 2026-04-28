#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/shell/shell.h>
#include <zephyr/version.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/reboot.h>
#include "shell.h"

/* ======================== */
/* Reset Komutu             */
/* ======================== */
static int cmd_reset(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "System rebooting...\n");
	k_sleep(K_MSEC(100));

	// Zephyr'da reset için sys_reboot kullan
	sys_reboot(SYS_REBOOT_COLD);

	return 0;
}

/* ======================== */
/* Sistem Bilgisi Komutu    */
/* ======================== */
static int cmd_sysinfo(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "\n=== Sistem Bilgileri ===");
	shell_print(sh, "Zephyr Version: %s", KERNEL_VERSION_STRING);
	shell_print(sh, "Build Date: %s %s", __DATE__, __TIME__);
	shell_print(sh, "Uptime: %lld ms", k_uptime_get());

	#ifdef CONFIG_ARCH
	shell_print(sh, "Architecture: %s", CONFIG_ARCH);
	#endif

	#ifdef CONFIG_BOARD_TARGET
	shell_print(sh, "Board: %s", CONFIG_BOARD_TARGET);
	#endif

	shell_print(sh, "=== Bellek Bilgileri ===");
	#ifdef CONFIG_HEAP_MEM_POOL_SIZE
	shell_print(sh, "Heap Size: %d bytes", CONFIG_HEAP_MEM_POOL_SIZE);
	#endif

	shell_print(sh, "========================\n");

	return 0;
}

/* ======================== */
/* Komutları Kaydet         */
/* ======================== */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_gsr_cmds,
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(gsr, &sub_gsr_cmds, "GSR komutları", NULL);

/* Root level komutları */
SHELL_CMD_REGISTER(reset, NULL, "Sistemi yeniden başlat", cmd_reset);
SHELL_CMD_REGISTER(sysinfo, NULL, "Sistem bilgilerini göster", cmd_sysinfo);

/* ======================== */
/* Shell Başlatması         */
/* ======================== */
void app_shell_init(void)
{
	printk("Shell initialized\n");
}
