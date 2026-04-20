#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/dt-bindings/adc/nrf-saadc.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <stdint.h>

#define ADC_NODE DT_NODELABEL(adc)

static const struct device *adc_dev = DEVICE_DT_GET(ADC_NODE);

static struct adc_channel_cfg channel_cfg = {
    .gain             = ADC_GAIN_1_4,
    .reference        = ADC_REF_INTERNAL,
    .acquisition_time = ADC_ACQ_TIME_DEFAULT,
    .channel_id       = 0,
#if defined(CONFIG_ADC_CONFIGURABLE_INPUTS)
    .input_positive   = NRF_SAADC_AIN4,   /* P1.11 */
#endif
};

int main(void)
{
    int err;
    int16_t sample_buffer;

    struct adc_sequence sequence = {
        .channels    = BIT(0),
        .buffer      = &sample_buffer,
        .buffer_size = sizeof(sample_buffer),
        .resolution  = 12,
    };

    printk("GSR ADC Start\n");

    if (!device_is_ready(adc_dev)) {
        printk("ADC device not ready\n");
        return 0;
    }

    err = adc_channel_setup(adc_dev, &channel_cfg);
    if (err < 0) {
        printk("ADC setup failed (%d)\n", err);
        return 0;
    }

    while (1) {
        sample_buffer = 0;

        err = adc_read(adc_dev, &sequence);
        if (err < 0) {
            printk("adc_read failed (%d)\n", err);
        } else {
            printk("%lld,%d\n", k_uptime_get(), sample_buffer);
        }

        k_sleep(K_MSEC(200));
    }

    return 0;
}