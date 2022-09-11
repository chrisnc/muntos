#include <rt/mutex.h>

#include <rt/rt.h>

void rt_mutex_init(struct rt_mutex *mutex)
{
    rt_list_init(&mutex->wait_list);
    mutex->syscall_record.syscall = RT_SYSCALL_MUTEX_UNLOCK;
    mutex->num_waiters = 0;
    atomic_flag_clear_explicit(&mutex->unlock_pending, memory_order_relaxed);
    atomic_flag_clear_explicit(&mutex->lock, memory_order_release);
}

void rt_mutex_lock(struct rt_mutex *mutex)
{
    if (rt_mutex_trylock(mutex))
    {
        /* Acquired the lock. */
        return;
    }

    struct rt_task *self = rt_self();
    self->syscall_args.mutex = mutex;
    self->syscall_record.syscall = RT_SYSCALL_MUTEX_LOCK;
    rt_syscall_push(&self->syscall_record);
    rt_syscall_post();
}

bool rt_mutex_trylock(struct rt_mutex *mutex)
{
    return !atomic_flag_test_and_set_explicit(&mutex->lock,
                                              memory_order_acquire);
}

void rt_mutex_unlock(struct rt_mutex *mutex)
{
    /* TODO: make the lock go to the highest priority waiter, rather than
     * whoever gets to the test_and_set first. */
    atomic_flag_clear_explicit(&mutex->lock, memory_order_release);

    /* If there isn't already an unlock system call pending, then create one. */
    if (!atomic_flag_test_and_set_explicit(&mutex->unlock_pending,
                                           memory_order_acquire))
    {
        rt_syscall_push(&mutex->syscall_record);
        rt_syscall_post();
    }
}