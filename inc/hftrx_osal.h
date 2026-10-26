/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef INC_HFTRX_OSAL_H_
#define INC_HFTRX_OSAL_H_

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#ifdef __INCLUDE_NUTTX_CONFIG_H
#define CONFIG_HFTRX_OSAL_THREAD_SET_ARGV int argc, char **argv
#define CONFIG_HFTRX_OSAL_THREAD_GET_ARGV ((uintptr_t)strtoul(argv[1], NULL, 16))
#elif defined(__ZEPHYR__)
#define CONFIG_HFTRX_OSAL_THREAD_SET_ARGV void *p1, void *p2, void *p3
#define CONFIG_HFTRX_OSAL_THREAD_GET_ARGV ((uintptr_t)p1)
#else
#define CONFIG_HFTRX_OSAL_THREAD_SET_ARGV void *argument
#define CONFIG_HFTRX_OSAL_THREAD_GET_ARGV ((uintptr_t)argument)
#endif

#define HFTRX_OSAL_WAITING_FOREVER (0xFFFFFFFFU)

typedef void *hftrx_osal_thread_t;
typedef void *hftrx_osal_sem_t;
typedef void *hftrx_osal_mutex_t;
typedef void *hftrx_osal_mq_t;
typedef void (*hftrx_thread_entry_t)(CONFIG_HFTRX_OSAL_THREAD_SET_ARGV);
typedef void (*hftrx_timer_handler_t)(void *argument);
struct hftrx_osal_timer {
	hftrx_timer_handler_t handler;
    void *argument;
    bool is_period;
    uint32_t timeout_ms;
    void *timer;
};

/*
 * Task with smaller priority value indicates higher task priority
*/
hftrx_osal_thread_t hftrx_osal_thread_create(const char *name, uint32_t stack_size, uint32_t prio, hftrx_thread_entry_t entry, void *args);
void hftrx_osal_thread_delete(hftrx_osal_thread_t thread);
void hftrx_osal_thread_schedule_other(void);

hftrx_osal_sem_t hftrx_osal_sem_create(uint32_t initial_count);
void hftrx_osal_sem_delete(hftrx_osal_sem_t sem);
int hftrx_osal_sem_take(hftrx_osal_sem_t sem, uint32_t timeout);
int hftrx_osal_sem_give(hftrx_osal_sem_t sem);
void hftrx_osal_sem_reset(hftrx_osal_sem_t sem);

hftrx_osal_mutex_t hftrx_osal_mutex_create(void);
void hftrx_osal_mutex_delete(hftrx_osal_mutex_t mutex);
int hftrx_osal_mutex_take(hftrx_osal_mutex_t mutex);
int hftrx_osal_mutex_give(hftrx_osal_mutex_t mutex);

hftrx_osal_mq_t hftrx_osal_mq_create(uint32_t max_msgs);
void hftrx_osal_mq_delete(hftrx_osal_mq_t mq);
int hftrx_osal_mq_send(hftrx_osal_mq_t mq, uintptr_t addr);
int hftrx_osal_mq_recv(hftrx_osal_mq_t mq, uintptr_t *addr, uint32_t timeout);

struct hftrx_osal_timer *hftrx_osal_timer_create(const char *name, uint32_t timeout_ms, hftrx_timer_handler_t handler, void *argument, bool is_period);
void hftrx_osal_timer_delete(struct hftrx_osal_timer *timer);
void hftrx_osal_timer_start(struct hftrx_osal_timer *timer);
void hftrx_osal_timer_stop(struct hftrx_osal_timer *timer);

size_t hftrx_osal_enter_critical_section(void);
void hftrx_osal_leave_critical_section(size_t flag);

void hftrx_osal_msleep(uint32_t delay);

void *hftrx_osal_malloc(size_t size);
void hftrx_osal_free(void *ptr);


#endif /* INC_HFTRX_OSAL_H_ */
