//
// ZIP library example sketch
//
#include <unzipLIB.h>
// Use a zip file in memory for this test
#include "bmp_icons.h"
UNZIP zip; // Statically allocate the 41K UNZIP class/structure

void setup() {
  Serial.begin(115200);
  while (!Serial) {};
  Serial.println("UNZIP library demo - open a zip file from FLASH");
}

void loop() {
  int rc;
  char szComment[256], szName[256];
  unz_file_info fi;

  rc = zip.openZIP((uint8_t *)bmp_icons, sizeof(bmp_icons));
  if (rc == UNZ_OK) {
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
     }
     zip.closeZIP();
  }
  while (1) {};
}
