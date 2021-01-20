#include "SD.h"
#include <cstdio>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

extern "C" {
    void sd_demo(void);
}

void sd_demo() {
  printf("Initializing SD card...\r\n");

  // see if the card is present and can be initialized:
  if (!SD.begin(0)) {
    printf("Card failed, or not present\r\n");
    // don't do anything more:
    while (1);
  }
  
  printf("card initialized.\r\n");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("busybox.txt");

  // if the file is available, write to it:
  if (dataFile) {
    uint8_t cnt = 0;
    while (dataFile.available()) {
      if (cnt == 0) {
        printf("\r\n");
      }
      cnt++;
      printf("%c",dataFile.read());
    }
    printf("\r\nClosing file\r\n");
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    printf("error opening file");
  }
  printf("Done!\r\n");
}