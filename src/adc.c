#include "adc.h"

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>

#define ADC_NODE        DT_PATH(zephyr_user)
#define SAMPLE_PERIOD_MS 200

static const struct adc_dt_spec adc_chan0 =
	ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 0);

static struct adc_sequence sequence;
static int16_t sample_buffer;

/* Timer + work */
static void adc_timer_handler(struct k_timer *timer_id);
static void adc_work_handler(struct k_work *work);

K_TIMER_DEFINE(adc_timer, adc_timer_handler, NULL);
K_WORK_DEFINE(adc_work, adc_work_handler);

int adc_init(void)
{
	int err;

	if (!adc_is_ready_dt(&adc_chan0)) {
		printk("ADC device not ready\n");
		return -ENODEV;
	}

	err = adc_channel_setup_dt(&adc_chan0);
	if (err < 0) {
		printk("adc_channel_setup_dt failed: %d\n", err);
		return err;
	}

	sequence.buffer = &sample_buffer;
	sequence.buffer_size = sizeof(sample_buffer);

	return 0;
}

int adc_sample(int16_t *value)
{
	int err;

	if (value == NULL) {
		return -EINVAL;
	}

	err = adc_sequence_init_dt(&adc_chan0, &sequence);
	if (err < 0) {
		printk("adc_sequence_init_dt failed: %d\n", err);
		return err;
	}

	err = adc_read(adc_chan0.dev, &sequence);
	if (err < 0) {
		printk("adc_read failed: %d\n", err);
		return err;
	}

	*value = sample_buffer;
	return 0;
}

static void adc_timer_handler(struct k_timer *timer_id)
{
	ARG_UNUSED(timer_id);

	/* Timer callback içinde ağır iş yapma, workqueue’ya bırak */
	k_work_submit(&adc_work);
}

static void adc_work_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	int err;
	int16_t value;

	err = adc_sample(&value);
	if (err == 0) {
		printk("%u,%d\n", k_uptime_get_32(), value);
	}
}

void adc_start_periodic_sampling(void)
{
	k_timer_start(&adc_timer, K_MSEC(SAMPLE_PERIOD_MS), K_MSEC(SAMPLE_PERIOD_MS));
}