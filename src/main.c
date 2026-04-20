#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/printk.h>

#define ADC_NODE DT_PATH(zephyr_user)

static const struct adc_dt_spec adc_chan0 =
	ADC_DT_SPEC_GET_BY_IDX(ADC_NODE, 0);

int main(void)
{
	int err;
	int16_t buf;
	struct adc_sequence sequence = {
		.buffer = &buf,
		.buffer_size = sizeof(buf),
	};

	if (!adc_is_ready_dt(&adc_chan0)) {
		printk("ADC device not ready\n");
		return 0;
	}

	err = adc_channel_setup_dt(&adc_chan0);
	if (err < 0) {
		printk("adc_channel_setup_dt failed: %d\n", err);
		return 0;
	}

	while (1) {
		err = adc_sequence_init_dt(&adc_chan0, &sequence);
		if (err < 0) {
			printk("adc_sequence_init_dt failed: %d\n", err);
			break;
		}

		err = adc_read(adc_chan0.dev, &sequence);
		if (err < 0) {
			printk("adc_read failed: %d\n", err);
		} else {
			printk("%u,%d\n", k_uptime_get_32(), buf);
		}

		k_sleep(K_MSEC(200));
	}

	return 0;
}