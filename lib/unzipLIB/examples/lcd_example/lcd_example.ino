//
// ZIP library example sketch
//
// Written by Larry Bank
// June 9, 2021
//
// This example shows how to do the following:
// - Step through all of the files in a ZIP archive
// - Display the name of each file
// - Allocate a buffer for the uncompressed file size
// - Read (decompress) the data into the buffer
// - Display the results (in this case a BMP file) on the SPI LCD
//
#include <unzipLIB.h>
#include <bb_spi_lcd.h>
// Set these to the appropriate values for your SPI LCD display
// -1 means the pin is not connected
// The CLK and MOSI pins might be -1 for your board if it has a default SPI setup
#define TFT_CS 10
#define TFT_RST -1
#define TFT_DC 9
#define TFT_CLK 13
#define TFT_MOSI 11

// Use a zip file in memory for this test
#include "bmp_icons.h"
UNZIP zip; // Statically allocate the 41K UNZIP class/structure
SPILCD lcd; // my display library

void setup() {
  Serial.begin(115200);
  delay(1000); // give Serial a little time to start
  spilcdInit(&lcd, LCD_ILI9341, FLAGS_NONE, 40000000, TFT_CS, TFT_DC, TFT_RST, -1, -1, TFT_MOSI, TFT_CLK);
  spilcdSetOrientation(&lcd, LCD_ORIENTATION_90); // for the ILI9341 it's nice to use it in 320x240 orientation
}

void loop() {
  int rc, x, y;
  char szComment[256], szName[256];
  unz_file_info fi;
  uint8_t *ucBitmap; // temp storage for each icon bitmap 

  spilcdFill(&lcd, 0, DRAW_TO_LCD); // Erase the display to black
  spilcdWriteString(&lcd, 0, 0, (char *)"Unzip BMP Files Test", 0xffff, 0, FONT_12x16, DRAW_TO_LCD); // white text on a black background
  x = 0; y = 24; // starting point to draw bitmaps
  rc = zip.openZIP((uint8_t *)bmp_icons, sizeof(bmp_icons));
  if (rc == UNZ_OK) {
     rc = zip.getGlobalComment(szComment, sizeof(szComment));
     Serial.print("Global comment: ");
     Serial.println(szComment);
     zip.gotoFirstFile();
     rc = UNZ_OK;
     while (rc == UNZ_OK) { // Display all files contained in the archive
        rc = zip.getFileInfo(&fi, szName, sizeof(szName), NULL, 0, szComment, sizeof(szComment));
        if (rc == UNZ_OK) {
            ucBitmap = (uint8_t *)malloc(fi.uncompressed_size); // allocate enough to hold the bitmap
            if (ucBitmap != NULL) { // malloc succeeded (it should, these bitmaps are only 2K bytes each)
              zip.openCurrentFile(); // if you don't open it explicitly, readCurrentFile will fail with UNZ_PARAMERROR
              spilcdWriteString(&lcd, 0, 224, "                           ", 0xff1f, 0, FONT_12x16, DRAW_TO_LCD); // erase old name
              spilcdWriteString(&lcd, 0, 224, szName, 0xff1f, 0, FONT_12x16, DRAW_TO_LCD); // display current file name at the bottom
              rc = zip.readCurrentFile(ucBitmap, fi.uncompressed_size); // we know the uncompressed size of these BMP images
              if (rc != fi.uncompressed_size) {
                  Serial.print("Read error, rc=");
                  Serial.println(rc, DEC);
              }
              spilcdDrawBMP(&lcd, ucBitmap, x, y, 1, -1, DRAW_TO_LCD); // Display the BMP file stretched 2X starting at (x,y)
              x += 64; // Draw them across the display from left to right
              if (x >= 256) { // move down for the next row
                x = 0;
                y += 64;
              }
              free(ucBitmap); // finished with this bitmap
            }
            delay(1000); // Allow time to see it happen, otherwise it will zip by too quickly
        }
        rc = zip.gotoNextFile();
     }
     zip.closeZIP();
  }
//  while (1) {};
}
