/*
 * sys_arch.h
 *
 *  Created on: Nov 25, 2026
 *      Author: Gena
 */

#ifndef ARCH_SYS_ARCH_H_
#define ARCH_SYS_ARCH_H_


typedef int sys_sem_t;
#define sys_sem_valid(sema) ((sema) != NULL)

typedef int sys_mutex_t;
#define sys_mutex_valid(mutex) (((mutex) != NULL)

struct lwip_mbox {
  void* sem;
  void** q_mem;
  unsigned int head, tail;
  int size;
  int used;
};
typedef struct lwip_mbox sys_mbox_t;
#define SYS_MBOX_NULL NULL
#define sys_mbox_valid(mbox) ((mbox != NULL) && ((mbox)->sem != NULL)  && ((mbox)->sem != (void*)-1))
#define sys_mbox_valid_val(mbox) (((mbox).sem != NULL)  && ((mbox).sem != (void*)-1))

/* DWORD (thread id) is used for sys_thread_t but we won't include windows.h */
typedef u32_t sys_thread_t;

#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev)
#define SYS_ARCH_UNPROTECT(lev)

/* to implement doing something while blocking on an mbox or semaphore:
 * pass a function to test_sys_arch_wait_callback() that returns
 * '0' if waiting again and
 * '1' if now there should be something to do (used for asserting)
 */
typedef int (*test_sys_arch_waiting_fn)(sys_sem_t* wait_sem, sys_mbox_t* wait_mbox);
void test_sys_arch_wait_callback(test_sys_arch_waiting_fn waiting_fn);

/* current time */
extern u32_t lwip_sys_now;

sys_sem_t* sys_arch_netconn_sem_get(void);
void sys_arch_netconn_sem_alloc(void);
void sys_arch_netconn_sem_free(void);
#define LWIP_NETCONN_THREAD_SEM_GET()   sys_arch_netconn_sem_get()
#define LWIP_NETCONN_THREAD_SEM_ALLOC() sys_arch_netconn_sem_alloc()
#define LWIP_NETCONN_THREAD_SEM_FREE()  sys_arch_netconn_sem_free()


#endif /* ARCH_SYS_ARCH_H_ */
