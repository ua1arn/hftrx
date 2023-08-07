//
// unzipLIB - Arduino unzip library
// based on zlib and contrib/minizip/unzip
// written by Larry Bank
// bitbank@pobox.com
//
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
#include "unzipLIB.h"

int UNZIP::openZIP(uint8_t *pData, uint32_t iDataSize)
{
    _zip.zHandle = unzOpen(NULL, pData, iDataSize, &_zip, NULL, NULL, NULL, NULL);
    if (_zip.zHandle == NULL) {
//       printf("Error opening file: %s\n", argv[1]);
       return -1;
    }
    return 0;
} /* open() */

int UNZIP::openZIP(const char *szFilename, ZIP_OPEN_CALLBACK *pfnOpen, ZIP_CLOSE_CALLBACK *pfnClose, ZIP_READ_CALLBACK *pfnRead, ZIP_SEEK_CALLBACK *pfnSeek)
{
    _zip.zHandle = unzOpen(szFilename, NULL, 0, &_zip, pfnOpen, pfnRead, pfnSeek, pfnClose);
    if (_zip.zHandle == NULL) {
//       printf("Error opening file: %s\n", argv[1]);
       return -1;
    }
    return 0;
} /* open() */

int UNZIP::closeZIP()
{
    _zip.iLastError = unzClose((unzFile)_zip.zHandle);
    return _zip.iLastError;
} /* closeZIP() */

int UNZIP::openCurrentFile()
{
    _zip.iLastError = unzOpenCurrentFile((unzFile)_zip.zHandle);
    return _zip.iLastError;
} /* openCurrentFile() */

int UNZIP::closeCurrentFile()
{
    _zip.iLastError = unzCloseCurrentFile((unzFile)_zip.zHandle);
    return _zip.iLastError;
} /* closeCurrentFile() */

int UNZIP::readCurrentFile(uint8_t *buffer, uint32_t iLength)
{
    return unzReadCurrentFile((unzFile)_zip.zHandle, buffer, iLength);
} /* readCurrentFile() */
int UNZIP::getCurrentFilePos()
{
    return (int)unztell((unzFile)_zip.zHandle);
} /* getCurrentFilePos() */

int UNZIP::gotoFirstFile()
{
    return unzGoToFirstFile((unzFile)_zip.zHandle);
} /* gotoFirstFile() */
int UNZIP::gotoNextFile()
{
    _zip.iLastError = unzGoToNextFile((unzFile)_zip.zHandle);
    return _zip.iLastError;
} /* gotoNextFile() */
int UNZIP::locateFile(const char *szFilename)
{
    _zip.iLastError = unzLocateFile((unzFile)_zip.zHandle, szFilename, 2);
    return _zip.iLastError;
} /* locateFile() */

int UNZIP::getFileInfo(unz_file_info *pFileInfo, char *szFileName, int iFileNameBufferSize, void *extraField, int iExtraFieldBufferSize, char *szComment, int iCommentBufferSize) // get info about the current file

{
    return unzGetCurrentFileInfo((unzFile)_zip.zHandle,
                            pFileInfo,
                            szFileName,
                            iFileNameBufferSize,
                            extraField,
                            iExtraFieldBufferSize,
                            szComment,
                            iCommentBufferSize);
} /* getFileInfo() */

int UNZIP::getLastError()
{
    return _zip.iLastError;
} /* getLastError() */

int UNZIP::getGlobalComment(char *destBuffer, int iBufferSize)
{
    _zip.iLastError =  unzGetGlobalComment((unzFile)_zip.zHandle, destBuffer, iBufferSize);
    return _zip.iLastError;
} /* getComment() */
