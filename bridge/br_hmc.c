/*
 *	Communicating with Hybrid Mesh Core
 *
 *	Authors:
 *	Dicky Chiang		<chiang@akiranet.com>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <net/arp.h>
#include <net/ip.h>
#include <linux/ieee80211.h>

#include "br_private.h"

#define HMC_PLC_IFACE   "eth0"
#define HMC_WIFI_IFACE  "mesh0"

struct net_bridge_hmc br_hmc;
struct net_device *hmc_local_dev;

void br_hmc_print_skb(struct sk_buff *skb, const char* type, int offset)
{
        size_t len;
        int rowsize = 16;
        int i, l, linelen, remaining;
        int li = 0;
        u8 *data, ch; 

        data = (u8 *) skb_mac_header(skb);
       //data = (u8 *) skb->head;

        if (skb_is_nonlinear(skb)) {
                len = skb->data_len;
        } else {
                len = skb->len;
        }

        if (len > 256)
            len = 256;

        remaining = len + 2 + offset;
        printk("Packet hex dump (len = %ld):\n", len);
        printk("============== %s ==============\n", type);
        for (i = 0; i < len; i += rowsize) {
                printk("%06d\t", li);

                linelen = min(remaining, rowsize);
                remaining -= rowsize;

                for (l = 0; l < linelen; l++) {
                        ch = data[l];
                        printk(KERN_CONT "%02X ", (uint32_t) ch);
                }

                data += linelen;
                li += 10; 

                printk(KERN_CONT "\n");
        }
        printk("====================================\n");
}
EXPORT_SYMBOL(br_hmc_print_skb);

void br_hmc_net_info(struct sk_buff *skb)
{
    struct net_bridge_port *port;

    port = br_port_get_rcu(skb->dev);
    br_hmc_info("port->name = %s\n", port->dev->name);
}

int br_hmc_forward(struct sk_buff *skb, struct net_bridge_hmc *hmc)
{
    struct net_bridge *br;
    struct net_bridge_port *p;

    BR_TRACE();

    if (!hmc || !skb) {
        br_hmc_err("hmc or skb is null");
        return -ENOMEM;
    }

    br = netdev_priv(hmc_local_dev);

    skb->dev = hmc_local_dev;

    br_hmc_info("port egress = %d\n", hmc->egress);

    rcu_read_lock();

    list_for_each_entry(p, &br->port_list, list) {

        if (hmc->egress == HMC_PORT_FLOOD) {
            br_forward(p, skb, true, false);
            continue;
        }

        if (hmc->egress == HMC_PORT_PLC &&
            (strncmp(p->dev->name, HMC_PLC_IFACE, strlen(HMC_PLC_IFACE)) == 0)) {
            br_forward(p, skb, true, false);
            continue;
        }

        if (hmc->egress == HMC_PORT_WIFI &&
            (strncmp(p->dev->name, HMC_WIFI_IFACE, strlen(HMC_WIFI_IFACE)) == 0)) {
            br_forward(p, skb, true, false);
            continue;
        }
    }

    rcu_read_unlock();
    return 0;
}
EXPORT_SYMBOL(br_hmc_forward);

int br_hmc_rx_handler(struct sk_buff *skb)
{
    struct net_bridge_hmc *hmc, *n;

    BR_TRACE();

    list_for_each_entry_safe(hmc, n, &br_hmc.list, list) {
        if (hmc->ops->rx)
            hmc->ops->rx(skb);
    }

    return 0;
}

struct net_bridge_hmc *br_hmc_alloc(const char *name, struct net_bridge_hmc_ops *ops)
{
    struct net_bridge_hmc *hmc;

    hmc = kmalloc(sizeof(*hmc), GFP_KERNEL);
    if (!hmc) {
        br_hmc_err("Failed to allocate mem for hmc\n");
        return NULL;
    }

    memcpy(hmc->br_addr, hmc_local_dev->dev_addr, ETH_ALEN);
    hmc->egress = HMC_PORT_FLOOD;
    hmc->ops = ops;

    list_add(&hmc->list, &br_hmc.list);

    return hmc;
}
EXPORT_SYMBOL(br_hmc_alloc);

void br_hmc_dealloc(void)
{
    struct net_bridge_hmc *p, *n;

    BR_TRACE();

    list_for_each_entry_safe(p, n, &br_hmc.list, list) {
        list_del(&p->list);
        kfree(p);
    }
}
EXPORT_SYMBOL(br_hmc_dealloc);

void br_hmc_notify(int cmd, struct net_device *dev)
{
    BR_TRACE();

    switch (cmd) {
    case HMC_ADD_BR:
        if (!hmc_local_dev)
            hmc_local_dev = NULL;
        hmc_local_dev = dev;
        break;
    case HMC_ADD_IF:
        break;
    };
}

int br_hmc_init(void)
{
    int ret = 0;

    BR_TRACE();

    INIT_LIST_HEAD(&br_hmc.list);

    return ret;
}

void br_hmc_deinit(void)
{
    BR_TRACE();
}