/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/zephyr.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/iso.h>

/** Print data as d_0 d_1 d_2 ... d_(n-2) d_(n-1) d_(n) to show the 3 first and 3 last octets
 *
 * Examples:
 * 01
 * 0102
 * 010203
 * 01020304
 * 0102030405
 * 010203040506
 * 010203...050607
 * 010203...060708
 * etc.
 */
static void iso_print_data(uint8_t *data, size_t data_len)
{
	/* Maximum number of octets from each end of the data */
	const uint8_t max_octets = 3;
	char data_str[35];
	size_t str_len;

	str_len = bin2hex(data, MIN(max_octets, data_len), data_str, sizeof(data_str));
	if (data_len > max_octets) {
		if (data_len > (max_octets * 2)) {
			static const char dots[] = "...";

			strcat(&data_str[str_len], dots);
			str_len += strlen(dots);
		}

		str_len += bin2hex(data + (data_len - MIN(max_octets, data_len - max_octets)),
				   MIN(max_octets, data_len - max_octets),
				   data_str + str_len,
				   sizeof(data_str) - str_len);
	}

	printk("\t %s\n", data_str);
}

static void iso_recv(struct bt_iso_chan *chan, const struct bt_iso_recv_info *info,
		struct net_buf *buf)
{
	printk("Incoming data channel %p len %u\n", chan, buf->len);
	iso_print_data(buf->data, buf->len);
}

static void iso_connected(struct bt_iso_chan *chan)
{
	printk("ISO Channel %p connected\n", chan);
}

static void iso_disconnected(struct bt_iso_chan *chan, uint8_t reason)
{
	printk("ISO Channel %p disconnected (reason 0x%02x)\n", chan, reason);
}

static struct bt_iso_chan_ops iso_ops = {
	.recv		= iso_recv,
	.connected	= iso_connected,
	.disconnected	= iso_disconnected,
};

static struct bt_iso_chan_io_qos iso_rx = {
	.sdu = CONFIG_BT_ISO_TX_MTU,
	.path = NULL,
};

static struct bt_iso_chan_qos iso_qos = {
	.rx = &iso_rx,
	.tx = NULL,
};

static struct bt_iso_chan iso_chan = {
	.ops = &iso_ops,
	.qos = &iso_qos,
};

static int iso_accept(const struct bt_iso_accept_info *info,
		      struct bt_iso_chan **chan)
{
	printk("Incoming request from %p\n", (void *)info->acl);

	if (iso_chan.iso) {
		printk("No channels available\n");
		return -ENOMEM;
	}

	*chan = &iso_chan;

	return 0;
}

static struct bt_iso_server iso_server = {
	.sec_level = BT_SECURITY_L1,
	.accept = iso_accept,
};

void ble_iso_setup(void)
{
	int err;

	err = bt_iso_server_register(&iso_server);
	if (err) {
		printk("Unable to register ISO server (err %d)\n", err);
		return;
	}
}
