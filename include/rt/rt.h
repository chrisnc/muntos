#ifndef RT_H
#define RT_H

#include <rt/task.h>

/*
 * Start the rt scheduler.
 */
void rt_start(void);

/*
 * Stop the rt scheduler.
 */
void rt_stop(void);

/*
 * Yield control of the processor to another runnable task.
 */
void rt_yield(void);

/*
 * Terminate the currently running task.
 */
void rt_task_exit(void);

/*
 * End all tasks. Called from rt_stop.
 */
void rt_end_all_tasks(void);

/*
 * Get a pointer to the currently executing task.
 */
struct rt_task *rt_task_self(void);

/*
 * Resume a task. Used internally to make a task that was previously blocked
 * runnable again. Must be called within a critical section.
 */
void rt_task_resume(struct rt_task *task);

#endif /* RT_H */
