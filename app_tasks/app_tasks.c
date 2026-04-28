#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "app_tasks.h"

/* Heart beat task stack ve priority tanımlamaları */
#define HEART_BEAT_STACK_SIZE 512
#define HEART_BEAT_PRIORITY 7

static void heart_beat_task(void *p1, void *p2, void *p3);

K_THREAD_DEFINE(heart_beat_tid, HEART_BEAT_STACK_SIZE, heart_beat_task,
		NULL, NULL, NULL, HEART_BEAT_PRIORITY, 0, 0);

static void heart_beat_task(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	printk("Heart beat task started\n");

	while (1) {
		printk("♥ Heart beat\n");
		k_sleep(K_SECONDS(2));
	}
}

void heart_beat_task_start(void)
{
	printk("Heart beat task initialized\n");
}
