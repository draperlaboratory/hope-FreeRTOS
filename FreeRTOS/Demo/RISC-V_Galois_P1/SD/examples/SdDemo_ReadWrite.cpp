/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */
#include "SD.h"
#include <cstdio>

File myFile;

extern "C" {
    void sd_demo(void);
}

const char *filename = "test.txt";

void sd_demo() {
  printf("Initializing SD card...\r\n");

  if (!SD.begin()) {
    printf("initialization failed!\r\n");
    while (1);
  }
  printf("initialization done.\r\n");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    printf("Writing to test.txt...\r\n");
    const char * text = "testing 1, 2, 3.";
    myFile.write((uint8_t*)text, strlen(text));
    // close the file:
    myFile.close();
    printf("done.\r\n");
  } else {
    // if the file didn't open, print an error:
    printf("error opening test.txt\r\n");
  }

  // re-open the file for reading:
  myFile = SD.open(filename);
  if (myFile) {
    printf("%s",filename);

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      printf("%c",myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    printf("error opening test.txt\r\n");
  }
  printf("\r\nDone!\r\n");
}

