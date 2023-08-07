// Copyright 2021 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================

#ifndef __UNZIPLIB__
#define __UNZIPLIB__
#if defined( PICO_BUILD ) || defined( __MACH__ ) || defined( __LINUX__ ) || defined( __MCUXPRESSO ) || 1
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#define memcpy_P memcpy
#define PROGMEM
#else
#include <Arduino.h>
#endif
//
// unzip library
// Written by Larry Bank
// Copyright (c) 2021 BitBank Software, Inc.
// bitbank@pobox.com
// 
// An embedded-friendly unzip library
// which needs only 41K of RAM
//
#include "unzip.h"

#ifdef __cplusplus
//
// The UNZIP class wraps portable C code which does the actual work
//
class UNZIP
{
  public:
    int openZIP(uint8_t *pData, uint32_t iDataSize);
    int openZIP(const char *szFilename, ZIP_OPEN_CALLBACK *pfnOpen, ZIP_CLOSE_CALLBACK *pfnClose, ZIP_READ_CALLBACK *pfnRead, ZIP_SEEK_CALLBACK *pfnSeek);
    int closeZIP();
    int openCurrentFile();
    int closeCurrentFile();
    int readCurrentFile(uint8_t *buffer, uint32_t iLength);
    int getCurrentFilePos();
    int gotoFirstFile();
    int gotoNextFile();
    int locateFile(const char *szFilename);
    int getFileInfo(unz_file_info *pFileInfo, char *szFileName, int iFilenameBufferSize, void *extraField, int iExtraFieldBufferSize, char *szComment, int iCommentBufferSize); // get info about the current file
    int getLastError();
    int getGlobalComment(char *destBuffer, int iBufferSize);

  private:
    ZIPFILE _zip;
}; // UNZIP class
// Add C interface here
#endif // __cplusplus

#endif // __UNZIPLIB__
