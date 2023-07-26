//
// ZIP library example sketch
//
// Written by Larry Bank
// June 9, 2021
//
// This example shows how to do the following:
// - Search an SD card for ZIP files
// - Step through all of the files in a ZIP archive (if found)
// - Display the name and compressed/uncompressed size of each file in the zip archive
// - Write the code for the open/close/read/seek callback functions to use any media type
//

#include <unzipLIB.h>
#include <SD.h>

UNZIP zip; // statically allocate the UNZIP structure (41K)

void setup() {
  Serial.begin(115200);

  while (!Serial && millis() < 3000); // wait up to 3 seconds for Arduino Serial Monitor
  Serial.println("Search for ZIP files on the SD card");

  while (!SD.begin(4/*BUILTIN_SDCARD*/)) { // change this to the appropriate value for your setup
    Serial.println("Unable to access SD Card");
    delay(1000);
  }
}

// Functions to access a file on the SD card
static File myfile;

//
// Callback functions needed by the unzipLIB to access a file system
// The library has built-in code for memory-to-memory transfers, but needs
// these callback functions to allow using other storage media
//
void * myOpen(const char *filename, int32_t *size) {
  myfile = SD.open(filename);
  *size = myfile.size();
  return (void *)&myfile;
}
void myClose(void *p) {
  ZIPFILE *pzf = (ZIPFILE *)p;
  File *f = (File *)pzf->fHandle;
  if (f) f->close();
}

int32_t myRead(void *p, uint8_t *buffer, int32_t length) {
  ZIPFILE *pzf = (ZIPFILE *)p;
  File *f = (File *)pzf->fHandle;
  return f->read(buffer, length);
}

int32_t mySeek(void *p, int32_t position, int iType) {
  ZIPFILE *pzf = (ZIPFILE *)p;
  File *f = (File *)pzf->fHandle;
  if (iType == SEEK_SET)
    return f->seek(position);
  else if (iType == SEEK_END) {
    return f->seek(position + pzf->iSize); 
  } else { // SEEK_CUR
    long l = f->position();
    return f->seek(l + position);
  }
}

// Main loop, scan for all .PNG files on the card and display them
void loop() {
  int rc, filecount = 0;
  char szComment[256], szName[256];
  unz_file_info fi;
  
  File dir = SD.open("/");
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    if (entry.isDirectory() == false) {
      const char *name = entry.name();
      const int len = strlen(name);
      if (len > 3 && strcmp(name + len - 3, "ZIP") == 0) {
        Serial.print("File: ");
        Serial.println(name);
        rc = rc = zip.openZIP(name, myOpen, myClose, myRead, mySeek);
        if (rc == UNZ_OK) {
          Serial.print("found zip file: ");
          Serial.println(name);
          // Display the global comment and all of the filenames within
          rc = zip.getGlobalComment(szComment, sizeof(szComment));
          Serial.print("Global comment: ");
          Serial.println(szComment);
          Serial.println("Files in this archive:");
          zip.gotoFirstFile();
          rc = UNZ_OK;
          while (rc == UNZ_OK) { // Display all files contained in the archive
            rc = zip.getFileInfo(&fi, szName, sizeof(szName), NULL, 0, szComment, sizeof(szComment));
            if (rc == UNZ_OK) {
              Serial.print(szName);
              Serial.print(" - ");
              Serial.print(fi.compressed_size, DEC);
              Serial.print("/");
              Serial.println(fi.uncompressed_size, DEC);
            }
            rc = zip.gotoNextFile();
          } // while more files...
          zip.closeZIP();
        }
        filecount = filecount + 1;
      }
    }
    entry.close();
  }
  if (filecount == 0) {
    Serial.println("No .ZIP files found");
  }
  delay(2000);
}
