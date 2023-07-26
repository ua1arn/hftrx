//
// unzip test
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../src/unzip.h"
#include "bmp_icons.h"

void * myOpen(const char *filename, int32_t *size) {
  printf("Attempting to open %s\n", filename);
    FILE *f;
    size_t filesize;
    f = fopen(filename,"r+b");
    if (f) {
        fseek(f, 0, SEEK_END);
        filesize = ftell(f);
        fseek(f, 0, SEEK_SET);
        *size = (int32_t) filesize;
    }
    return (void *)f;
} /* myOpen() */

void myClose(void *p) {
    ZIPFILE *pzf = (ZIPFILE *)p;
  if (pzf) fclose((FILE *)pzf->fHandle);
} /* myClose() */

int32_t myRead(void *p, uint8_t *buffer, int32_t length) {
    ZIPFILE *pzf = (ZIPFILE *)p;
  if (!pzf) return 0;
    return (int32_t)fread(buffer, 1, length, (FILE *)pzf->fHandle);
} /* myRead() */

int32_t mySeek(void *p, int32_t position, int type) {
    ZIPFILE *pzf = (ZIPFILE *)p;
  if (!pzf) return 0;
  return fseek((FILE *)pzf->fHandle, position, type);
}

int main(int argc, const char *argv[])
{
int rc, i;
unzFile zHandle;
char szTemp[256];
ZIPFILE zpf;
    
    if (argc != 3 && argc != 1) // unzip <zip> <file to extract>
    {
       printf("Usage: unziptest <zip file> <file to unzip within the zip>\n");
       printf("       or unziptest (no arguments) to test mem to mem unzipping\n");
       return 0;
    }
    if (argc == 3) {
       printf("Starting unzip test...reading file %s from zip archive %s\n", argv[2], argv[1]);
       zHandle = unzOpen(argv[1], NULL, 0, &zpf, myOpen, myRead, mySeek, myClose);
    } else {
       zHandle = unzOpen(NULL, (uint8_t *)bmp_icons, sizeof(bmp_icons), &zpf, NULL, NULL, NULL, NULL);
    }

    if (zHandle == NULL) {
       printf("Error opening file: %s\n", argv[1]);
       return -1;
    }
    // Display the global comment (if any)
    rc = unzGetGlobalComment(zHandle, (char *)szTemp, sizeof(szTemp));
    printf("Global Comment: %s\n", szTemp);
    
//    rc = unzLocateFile(zHandle, argv[2], 2);
    rc = unzLocateFile(zHandle, "key.bmp", 2);
    if (rc != UNZ_OK) /* Report the file not found */
    {
        printf("file %s not found within archive\n", argv[2]);
        unzClose(zHandle);
        return -1;
    }
    rc = unzOpenCurrentFile(zHandle); /* Try to open the file we want */
    if (rc != UNZ_OK) {
       printf("Error opening file = %d\n", rc);
       unzClose(zHandle);
       return -1;
    }
    printf("File located within archive.\n");
    rc = 1;
    i = 0;
    while (rc > 0) {
        rc = unzReadCurrentFile(zHandle, szTemp, sizeof(szTemp));
        if (rc >= 0) {
            i += rc;
        } else {
            printf("Error reading from file\n");
            break;
        }
    }
    printf("Total bytes read = %d (reading 256 bytes at a time)\n", i);
    rc = unzCloseCurrentFile(zHandle);
    unzClose(zHandle);
  return 0;
} /* main() */ 

