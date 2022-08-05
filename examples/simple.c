#include <rt/critical.h>
#include <rt/rt.h>

#include <limits.h>
#include <stdio.h>

static void simple_fn(void)
{
    int n = 10000;
    while (n > 0)
    {
        printf("%s %d, tick %lu\n", rt_task_self()->cfg.name, n,
               rt_tick());
        fflush(stdout);
        --n;
    }
    rt_stop();
}

int main(void)
{
    static char task0_stack[PTHREAD_STACK_MIN], task1_stack[PTHREAD_STACK_MIN];
    static const struct rt_task_config task0_cfg = {
        .fn = simple_fn,
        .stack = task0_stack,
        .stack_size = sizeof(task0_stack),
        .name = "task0",
        .priority = 1,
    }, task1_cfg = {
        .fn = simple_fn,
        .stack = task1_stack,
        .stack_size = sizeof(task1_stack),
        .name = "task1",
        .priority = 1,
    };
    static struct rt_task task0, task1;
    rt_task_init(&task0, &task0_cfg);
    rt_task_init(&task1, &task1_cfg);

    rt_start();
}
