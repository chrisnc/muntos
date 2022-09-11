#ifndef RT_MUTEX_H
#define RT_MUTEX_H

#include <rt/list.h>
#include <rt/syscall.h>

#include <stdatomic.h>

struct rt_mutex;

void rt_mutex_init(struct rt_mutex *mutex);

void rt_mutex_lock(struct rt_mutex *mutex);

bool rt_mutex_trylock(struct rt_mutex *mutex);

void rt_mutex_unlock(struct rt_mutex *mutex);

struct rt_mutex
{
    struct rt_list wait_list;
    struct rt_syscall_record syscall_record;
    struct atomic_flag unlock_pending;
    struct atomic_flag lock;
    int num_waiters;
};

#define RT_MUTEX_INIT(name)                                                   \
    {                                                                          \
        .wait_list = RT_LIST_INIT(name.wait_list),                            \
        .syscall_record = {.next = NULL, .syscall = RT_SYSCALL_MUTEX_UNLOCK}, \
        .unlock_pending = ATOMIC_FLAG_INIT, .lock = ATOMIC_FLAG_INIT,          \
        .num_waiters = 0,                                                      \
    }

#define RT_MUTEX(name) struct rt_mutex name = RT_MUTEX_INIT(name)

#endif /* RT_MUTEX_H */
