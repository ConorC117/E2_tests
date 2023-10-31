// SPDX-License-Identifier: GPL-3
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/init.h>
#include <linux/uaccess.h>

static const char message[] = "Hello from Charlie's file in /proc\n";

static int proc_kdlp_show(struct seq_file *m, void *v)
{
    return seq_write(m, message, sizeof(message) - 1);
}

static int __init proc_kdlp_init(void)
{
    struct proc_dir_entry *entry = proc_create_single("kdlp", 0, NULL, proc_kdlp_show);
    if (!entry)
        return -ENOMEM;
    proc_set_size(entry, sizeof(message) - 1);
    return 0;
}

static void __exit proc_kdlp_exit(void)
{
    remove_proc_entry("kdlp", NULL);
}

module_init(proc_kdlp_init);
module_exit(proc_kdlp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charles Mirabile");
MODULE_DESCRIPTION("Create a demonstration entry in the procfs with a hello message in it.");
