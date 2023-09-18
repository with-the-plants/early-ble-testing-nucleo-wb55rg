/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>
#include <errno.h>

#include <zephyr/types.h>
#include <zephyr/zephyr.h>

#include <zephyr/logging/log.h>

#include <zephyr/usb/usb_device.h>

LOG_MODULE_REGISTER(main);

#include <zephyr/settings/settings.h>

// XXX
extern void usb_disk_setup(void);

extern int  ble_setup(void);
extern void ble_loop(void);

void main(void)
{
	int ret;

	usb_disk_setup();
	ret = ble_setup();
	if (ret != 0) {
		LOG_ERR("Failed to setup BLE");
		return;
	}
	LOG_INF("The devce is in BLE peripheral mode.");

	ret = usb_enable(NULL); // XXX
	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return;
	}
	LOG_INF("The device is in USB mass storage mode.");

	LOG_INF("Starting loop");
	/* Implement notification. At the moment there is no suitable way
	 * of starting delayed work (for BLE), so we do it here.
	 */
	while (1) {
		k_sleep(K_SECONDS(1));

		ble_loop();
	}
	LOG_ERR("Loop failed");
}
