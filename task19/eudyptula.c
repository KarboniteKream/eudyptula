// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/ip.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/printk.h>
#include <linux/textsearch.h>

MODULE_AUTHOR("Klemen Košir <klemen.kosir@kream.io>");
MODULE_DESCRIPTION("A simple Linux module");
MODULE_LICENSE("GPL");

static const char ID[] = "2dfea3d4488b";
static struct ts_config *ts_conf;

static unsigned int netfilter_hook(void *priv, struct sk_buff *skb,
				   const struct nf_hook_state *state)
{
	unsigned int pos = skb_find_text(skb, 0, UINT_MAX, ts_conf);

	if (pos != UINT_MAX)
		pr_debug("Detected %s\n", ID);

	return NF_ACCEPT;
}

static struct nf_hook_ops netfilter_ops = {
	.hook = netfilter_hook,
	.pf = PF_INET,
	.hooknum = NF_INET_LOCAL_IN,
	.priority = NF_IP_PRI_FILTER,
};

static int __init eudyptula_init(void)
{
	ts_conf = textsearch_prepare("kmp", ID, strlen(ID),
				     GFP_KERNEL, TS_AUTOLOAD);
	if (IS_ERR(ts_conf))
		return PTR_ERR(ts_conf);

	return nf_register_net_hook(&init_net, &netfilter_ops);
}

static void __exit eudyptula_exit(void)
{
	nf_unregister_net_hook(&init_net, &netfilter_ops);
	textsearch_destroy(ts_conf);
}

module_init(eudyptula_init);
module_exit(eudyptula_exit);
