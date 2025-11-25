// assignment_sensor.c
//
// Minimal kernel module that periodically "samples" a mock sensor
// (using get_random_bytes as data source) and logs the value using pr_info().
//

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/jiffies.h>

static struct task_struct *sensor_thread;

/*
 * Sampling interval in seconds. This can be changed at load time, e.g.:
 *   insmod assignment_sensor.ko interval_sec=2
 */
static unsigned int interval_sec = 5;
module_param(interval_sec, uint, 0644);
MODULE_PARM_DESC(interval_sec, "Sampling interval in seconds");

/*
 * Thread function: periodically read a mock sensor and log the value.
 * Uses get_random_bytes() as a stand-in for a hardware sensor.
 */
static int assignment_sensor_thread_fn(void *data)
{
    u32 sample = 0;

    pr_info("assignment_sensor: thread started, interval=%u s\n", interval_sec);

    while (!kthread_should_stop())
    {
        /* Simulated sensor read */
        get_random_bytes(&sample, sizeof(sample));

        pr_info("assignment_sensor: sample=0x%08x, jiffies=%lu\n",
                sample, jiffies);

        /*
         * Sleep for the requested interval. ssleep() is interruptible:
         * if the thread is asked to stop, it will wake up early.
         */
        if (interval_sec == 0)
            interval_sec = 1; /* avoid zero-sleep */

        ssleep(interval_sec);
    }

    pr_info("assignment_sensor: thread stopping\n");
    return 0;
}

/*
 * Module initialization: start the kthread.
 */
static int __init assignment_sensor_init(void)
{
    pr_info("assignment_sensor: init, starting thread\n");

    sensor_thread = kthread_run(assignment_sensor_thread_fn,
                                NULL,
                                "assignment_sensor_kthread");
    if (IS_ERR(sensor_thread))
    {
        int err = PTR_ERR(sensor_thread);
        pr_err("assignment_sensor: failed to start thread (err=%d)\n", err);
        sensor_thread = NULL;
        return err;
    }

    return 0;
}

/*
 * Module cleanup: stop the kthread.
 */
static void __exit assignment_sensor_exit(void)
{
    pr_info("assignment_sensor: exit, stopping thread\n");

    if (sensor_thread)
    {
        kthread_stop(sensor_thread);
        sensor_thread = NULL;
    }
}

// call functions
module_init(assignment_sensor_init);
module_exit(assignment_sensor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("macreat");
MODULE_DESCRIPTION("Minimal mock sensor kernel module for assignment");
MODULE_VERSION("1.0");
