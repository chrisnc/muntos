#include <rt/rt.h>

#include <rt/context.h>
#include <rt/critical.h>
#include <rt/port.h>

__attribute__((noreturn)) static void idle_task_fn(size_t argc, uintptr_t *argv)
{
  (void)argc;
  (void)argv;

  for (;;)
  {
    rt_yield();
  }
}

static struct rt_task idle_task = {
    .cfg =
        {
            .fn = idle_task_fn,
            .argc = 0,
            .argv = NULL,
            .stack = NULL,
            .stack_size = 0,
            .name = "idle",
            .priority = 0,
        },
    .runnable = true,
};

static struct list ready_list = LIST_INIT(ready_list);
static struct rt_task *active_task = &idle_task;

struct rt_task *rt_self(void)
{
  return active_task;
}

static inline void cycle_tasks(void)
{
  struct list *front = list_first(&ready_list);
  list_del(front);
  list_add_tail(&ready_list, &active_task->list);
  active_task = list_item(front, struct rt_task, list);
}

void rt_yield(void)
{
  rt_critical_begin();
  struct rt_task *old = rt_self();
  cycle_tasks();
  struct rt_task *new = rt_self();
  rt_critical_end();
  rt_context_swap(&old->ctx, &new->ctx);
}

void rt_suspend(struct rt_task *task)
{
  if (task == rt_self())
  {
    rt_critical_begin();
    cycle_tasks();
    struct rt_task *new_task = rt_self();
    list_del(&task->list);
    rt_critical_end();
    rt_context_swap(&task->ctx, &new_task->ctx);
  }
  else
  {
    rt_critical_begin();
    task->runnable = false;
    list_del(&task->list);
    rt_critical_end();
  }
}

void rt_resume(struct rt_task *task)
{
  if (task == rt_self())
  {
    return;
  }
  rt_critical_begin();
  task->runnable = true;
  list_add_tail(&ready_list, &task->list);
  // TODO: deal with different priorities
  rt_critical_end();
}

static void run_task(void *arg)
{
  const struct rt_task *task = arg;
  task->cfg.fn(task->cfg.argc, task->cfg.argv);
  rt_suspend(rt_self());
}

void rt_task_init(struct rt_task *task, const struct rt_task_config *cfg)
{
  task->cfg = *cfg;
  rt_context_init(&task->ctx, cfg->stack, cfg->stack_size, run_task, task);
  rt_resume(task);
}

static rt_tick_t ticks;

void rt_tick(void)
{
  ++ticks;
  // TODO: evaluate delay list
  rt_yield();
}

rt_tick_t rt_tick_count(void)
{
  return ticks;
}

void rt_delay(rt_tick_t ticks_to_delay)
{
  struct rt_task *self = rt_self();
  self->delay_time = ticks;
  self->delay_duration = ticks_to_delay;
  // TODO
}

void rt_start(void)
{
  rt_port_start();
  idle_task.cfg.fn(idle_task.cfg.argc, idle_task.cfg.argv);
}

static uint_fast8_t critical_nesting = 0;

void rt_critical_begin(void)
{
  rt_disable_interrupts();
  ++critical_nesting;
}

void rt_critical_end(void)
{
  --critical_nesting;
  if (critical_nesting == 0)
  {
    rt_enable_interrupts();
  }
}
