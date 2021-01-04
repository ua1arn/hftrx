/******************************************************************************
* Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#include <src/zynq/xil_types.h>
#include <stdio.h>

/*
 * fcntl -- Manipulate a file descriptor.
 *          We don't have a filesystem, so we do nothing.
 */
__attribute__((weak)) sint32 fcntl (sint32 fd, sint32 cmd, long arg)
{
  (void)fd;
  (void)cmd;
  (void)arg;
  return 0;
}
