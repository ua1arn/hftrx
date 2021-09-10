/*
 * types.h
 *
 *  Created on: Sep 10, 2021
 *      Author: gena
 */

#ifndef SRC_USB_IPXE_TYPES_H_
#define SRC_USB_IPXE_TYPES_H_


#define container_of(member_ptr, containing_type, member)		\
	 ((containing_type *)						\
	  ((char *)(member_ptr)						\
	   - container_off(containing_type, member))			\
	  + check_types_match(*(member_ptr), ((containing_type *)0)->member))

#define container_off(containing_type, member)	\
	offsetof(containing_type, member)

#if 1//HAVE_TYPEOF
	#define check_type(expr, type)			\
		((typeof(expr) *)0 != (type *)0)
	
	#define check_types_match(expr1, expr2)		\
		((typeof(expr1) *)0 != (typeof(expr2) *)0)
#else
	#include <ccan/build_assert/build_assert.h>
	/* Without typeof, we can only test the sizes. */
	#define check_type(expr, type)					\
		BUILD_ASSERT_OR_ZERO(sizeof(expr) == sizeof(type))
	
	#define check_types_match(expr1, expr2)				\
		BUILD_ASSERT_OR_ZERO(sizeof(expr1) == sizeof(expr2))
#endif /* HAVE_TYPEOF */

#endif /* SRC_USB_IPXE_TYPES_H_ */
