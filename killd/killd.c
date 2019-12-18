#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");   

static int pid = -1;
module_param(pid, int, 0600);

static int state = -1;
module_param(state, int, 0600);

static int __init killd_init(void)
{
    struct task_struct *ts = NULL;

    if (state == -1 || pid == -1) {
        printk(KERN_INFO "usage: insmod killd.ko pid=PID state=STATE\n");
        return 0;
    }

    rcu_read_lock();
    ts = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);
    rcu_read_unlock();
    if (!ts) {
        printk(KERN_INFO "process with pid:%d not found\n", pid);
        return 0;
    }

    printk(KERN_INFO "killd: pid %d old state: %#lx\n", pid, ts->state);
    ts->state = state;
    printk(KERN_INFO "killd: pid %d new state: %#lx\n", pid, ts->state);

    return 0;
}

static void __exit killd_cleanup(void)
{
    printk(KERN_INFO "killd deinit\n");
}

module_init(killd_init);
module_exit(killd_cleanup);
