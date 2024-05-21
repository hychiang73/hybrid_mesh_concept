/*
 *	Testing functions in bridge hybrid mesh core
 *
 *	Authors:
 *	Dicky Chiang		<chiang@akiranet.com>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>

#include "../bridge/br_private.h"

struct proc_dir_entry *proc_dir_hmc;
struct net_bridge_hmc *plc = NULL;

void test_hmc_gen_pkt(void)
{
    struct sk_buff *new_sk;
    struct ethhdr *ether;
    //const u8 da[ETH_ALEN] = {0x00,0x04,0x4b,0xe6,0xec,0x3d};
    const u8 da[ETH_ALEN] = {0x00,0x19,0x94,0x38,0xfd,0x8e};
    const u8 sa[ETH_ALEN] = {0x00,0x04,0x4b,0xec,0x28,0x3b};
    u8 *pos;

    BR_TRACE();

    new_sk = dev_alloc_skb(128);
    if (!new_sk) {
        pr_err("no space to allocate");
        return;
    }

    skb_reserve(new_sk, 2);

    ether = (struct ethhdr *)skb_put(new_sk, ETH_HLEN);
    memset(ether, 0, ETH_HLEN);

    memcpy(ether->h_dest, da, ETH_ALEN);
    memcpy(ether->h_source, sa, ETH_ALEN);
    ether->h_proto = ntohs(0xAA55);

    pos = skb_put(new_sk, 5);

    *pos++ = 100;
    *pos++ = 101;
    *pos++ = 102;
    *pos++ = 103;
    *pos++ = 104;

    skb_reset_mac_header(new_sk);

    br_hmc_print_skb(new_sk, "br_hmc_gen_pkt", 0);

    plc->egress = HMC_PORT_PLC;

    br_hmc_forward(new_sk, plc);
}

static ssize_t hmc_proc_test_read(struct file *pFile, char __user *buf, size_t size, loff_t *pos)
{
	if (*pos != 0)
        return 0;

    pr_info("*** BR-HMC tx test\n");
    test_hmc_gen_pkt();
    return 0;
}

static ssize_t hmc_proc_test_write(struct file *filp, const char *buff, size_t size, loff_t *pos)
{
    return size;
}

struct file_operations proc_test_fops = {
	.read = hmc_proc_test_read,
    .write = hmc_proc_test_write,
};

static int test_br_hmc_rx_plc(struct sk_buff *skb)
{
    pr_info("*** BR-HMC PLC rx callback test\n");
    return 0;
}

static struct net_bridge_hmc_ops test_br_hmc_ops_p = {
    .rx = test_br_hmc_rx_plc,
};

static int test_hmc_proc_create(void)
{
    int ret = 0;
    struct proc_dir_entry *node;

    proc_dir_hmc = proc_mkdir("hmc", NULL);

    node = proc_create("test", 0666, proc_dir_hmc, &proc_test_fops);
    if (!node) {
        pr_info("Failed to create proc node");
        ret = -ENODEV;
    }

    return ret;
}

static int test_br_hmc_alloc(void)
{
    int i;

    plc = br_hmc_alloc("PLC", &test_br_hmc_ops_p);

    if (!plc) {
        pr_err("plc is null\n");
        return -ENOMEM;
    }

    for (i = 0; i < ETH_ALEN; i++)
        pr_info("br addr = %x\n", plc->br_addr[i]);

    return 0;
}

static int __init test_hmc_init(void)
{
    test_br_hmc_alloc();
    test_hmc_proc_create();
    return 0;
}

void test_hmc_exit(void)
{
    remove_proc_entry("test", proc_dir_hmc);
    remove_proc_entry("hmc", NULL);
    br_hmc_dealloc();
}

module_init(test_hmc_init);
module_exit(test_hmc_exit);
MODULE_AUTHOR("AkiraNET");
MODULE_DESCRIPTION("Hybrid Mesh Core test");
MODULE_LICENSE("GPL");

