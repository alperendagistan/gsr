#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>

#include "ble_dfu.h"

static struct bt_le_adv_param adv_param = BT_LE_ADV_PARAM_INIT(
	BT_LE_ADV_OPT_CONN,
	BT_GAP_ADV_FAST_INT_MIN_2,
	BT_GAP_ADV_FAST_INT_MAX_2,
	NULL
);

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME,
		sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static void adv_restart_work_handler(struct k_work *work)
{
	int err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("BLE: adv restart failed (%d)\n", err);
	} else {
		printk("BLE: advertising restarted\n");
	}
}

static K_WORK_DEFINE(adv_restart_work, adv_restart_work_handler);

static void connected_cb(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("BLE: connection failed (err %d)\n", err);
		k_work_submit(&adv_restart_work);
		return;
	}
	printk("BLE: connected\n");
}

static void disconnected_cb(struct bt_conn *conn, uint8_t reason)
{
	printk("BLE: disconnected (reason %d)\n", reason);
	k_work_submit(&adv_restart_work);
}

BT_CONN_CB_DEFINE(conn_cbs) = {
	.connected = connected_cb,
	.disconnected = disconnected_cb,
};

int ble_dfu_init(void)
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		printk("BLE: bt_enable failed (%d)\n", err);
		return err;
	}
	printk("BLE: enabled\n");

	err = bt_le_adv_start(&adv_param, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("BLE: adv start failed (%d)\n", err);
		return err;
	}
	printk("BLE: advertising as \"%s\"\n", CONFIG_BT_DEVICE_NAME);

	return 0;
}
