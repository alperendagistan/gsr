#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdbool.h>
#include "app_tasks.h"

/* Heart beat task stack ve priority tanımlamaları */
#define HEART_BEAT_STACK_SIZE 1024
#define HEART_BEAT_PRIORITY 7

static void heart_beat_task(void *p1, void *p2, void *p3);
K_THREAD_STACK_DEFINE(heart_beat_stack, HEART_BEAT_STACK_SIZE);
static struct k_thread heart_beat_thread;
static k_tid_t heart_beat_tid;
static bool heart_beat_started;

static void heart_beat_task(void *p1, void *p2, void *p3)
{
	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	printk("Heart beat task started\n");

	while (1) {
		printk("alive %lld\n", k_uptime_get());
		k_sleep(K_SECONDS(2));
	}
}

void heart_beat_task_start(void)
{
	if (heart_beat_started) {
		printk("Heart beat task already running\n");
		return;
	}

	heart_beat_tid = k_thread_create(&heart_beat_thread,
					 heart_beat_stack,
					 K_THREAD_STACK_SIZEOF(heart_beat_stack),
					 heart_beat_task,
					 NULL, NULL, NULL,
					 HEART_BEAT_PRIORITY,
					 0,
					 K_NO_WAIT);
	if (!heart_beat_tid) {
		printk("Heart beat task create failed\n");
		return;
	}

	heart_beat_started = true;
	printk("Heart beat task initialized\n");
}
