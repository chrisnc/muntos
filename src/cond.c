#include <rt/cond.h>
#include <rt/log.h>

void rt_cond_init(struct rt_cond *cond)
{
    rt_sem_init(&cond->sem, 0);
}

static void cond_post(struct rt_sem *sem, bool broadcast)
{
    int value = rt_atomic_load_explicit(&sem->value, memory_order_relaxed);
    do
    {
        if (value >= 0)
        {
            /* Condition variable has no waiters. */
            return;
        }
        /* Use relaxed ordering always because the waiter is using a mutex. */
    } while (!rt_atomic_compare_exchange_weak_explicit(&sem->value, &value,
                                                       broadcast ? 0
                                                                 : value + 1,
                                                       memory_order_relaxed,
                                                       memory_order_relaxed));

    /* TODO: see sem_post */
    if (!rt_atomic_flag_test_and_set_explicit(&sem->post_pending,
                                              memory_order_acquire))
    {
        rt_syscall(&sem->post_record);
    }
}

void rt_cond_signal(struct rt_cond *cond)
{
    cond_post(&cond->sem, false);
}

void rt_cond_broadcast(struct rt_cond *cond)
{
    cond_post(&cond->sem, true);
}

void rt_cond_wait(struct rt_cond *cond, struct rt_mutex *mutex)
{
    /* Decrement the semaphore while still holding the mutex so that
     * signals from higher priority tasks on the same monitor can see
     * there is a waiter. */
    const int value =
        rt_atomic_fetch_sub_explicit(&cond->sem.value, 1, memory_order_relaxed);

    rt_logf("%s cond wait, new value %d\n", rt_task_name(), value - 1);

    if (value > 0)
    {
        return;
    }

    rt_mutex_unlock(mutex);

    struct rt_syscall_record wait_record;
    wait_record.args.sem_wait.task = rt_task_self();
    wait_record.args.sem_wait.sem = &cond->sem;
    wait_record.syscall = RT_SYSCALL_SEM_WAIT;
    rt_syscall(&wait_record);

    rt_mutex_lock(mutex);
}
