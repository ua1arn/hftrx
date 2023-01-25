#ifndef __ARM32_TYPES_H__
#define __ARM32_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

//typedef signed int				off_t;
typedef signed long long		loff_t;

typedef signed int				bool_t;
typedef unsigned int			irq_flags_t;

typedef uintptr_t			virtual_addr_t;
typedef ptrdiff_t			virtual_size_t;
typedef uintptr_t			physical_addr_t;
typedef ptrdiff_t			physical_size_t;

typedef struct {
	volatile int counter;
} atomic_t;
//
//typedef struct {
//	volatile int lock;
//} spinlock_t;

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_TYPES_H__ */
