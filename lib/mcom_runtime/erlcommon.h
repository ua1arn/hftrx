/*
 * File:   erlcommon.h
 *
 *  This file contains Elcore Runtime Library error types description
 *
 * (c) multicore.ru
 */

#ifndef ERLCOMMON_H
#define ERLCOMMON_H

#ifdef __cplusplus
extern "C" {
#endif

// Enumeration: DSP_TASK_STATUS
//
// Codes of possible errors in program linked with ERL library
//
enum ERL_ERROR {
    // Constant: ERL_NO_ERROR
    // No error while function execution
    ERL_NO_ERROR = 0,
    // Constant: ERL_SYSTEM_ERROR
    // Device error: no responce, device is busy
    ERL_SYSTEM_ERROR,
    // Constant: ERL_UNITIALIZED_ARG
    // Functions receives uninitialized argument or zero pointer
    ERL_UNITIALIZED_ARG,
    // Constant: ERL_MEMORY_ALLOC
    // Memeory allocation error
    ERL_MEMORY_ALLOC,
    // Constant: ERL_PROGRAM_ERROR
    // Program error, wrong logic in parameters setup
    ERL_PROGRAM_ERROR,
    // Constant: ERL_OVERLAY_ERROR
    // Overlay linking or loading by incorrect address
    ERL_OVERLAY_ERROR
};

#ifdef __cplusplus
}
#endif

#endif /* ERLCOMMON_H */
