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
  if (!card.init()) {
    printf("initialization failed.\r\n");
  } else {
    printf("Wiring is correct and a card is present.");
  }
  printf("\r\n");

  // print the type of card
  printf("Card type:         ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      printf("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      printf("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      printf("SDHC");
      break;
    default:
      printf("Unknown");
  }
  printf("\r\n");

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    printf("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    while (1);
  }
  printf("\r\n");

  printf("Clusters:          ");
  printf("%u",volume.clusterCount());
  printf("\r\n");
  printf("Blocks x Cluster:  ");
  printf("%u",volume.blocksPerCluster());
  printf("\r\n");

  printf("Total Blocks:      ");
  printf("%u",volume.blocksPerCluster() * volume.clusterCount());
  printf("\r\n");

  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  printf("Volume type is:    FAT");
  printf("%u",volume.fatType());
  printf("\r\n");

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize /= 2;                           // SD card blocks are always 512 bytes (2 blocks are 1KB)
  printf("Volume size (Kb):  ");
  printf("%u",volumesize);
  printf("\r\n");
  printf("Volume size (Mb):  ");
  volumesize /= 1024;
  printf("%u",volumesize);
  printf("\r\n");
  /*
   * NOTE: printing floating point numbers is not supported
  printf("Volume size (Gb):  ");
  printf("%f",(float)volumesize / 1024.0);
  printf("\r\n");
  */

  printf("\nFiles found on the card (name, date and size in bytes): ");
  if (root.openRoot(volume) == 1) {
      printf("Root opened\r\n");
  } else {
      printf("openRoot failed\r\n");
      while(1);
  }
  printf("\r\n");

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
  printf("\r\n");
}
