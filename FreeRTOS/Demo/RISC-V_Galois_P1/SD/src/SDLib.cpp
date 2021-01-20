#include "SDLib.h"
#include "SD.h"
#include <cstdio>

File sdfile;

/**
 * Initialize SD card
 * Has to be called once before any other SDLib function
 */
int sdlib_initialize(void) {
  printf("Initializing SD card...\r\n");
  if (!SD.begin()) {
    printf("initialization failed!\r\n");
    return 0;
  }
  printf("initialization done.\r\n");
  return 1;
}

/**
 * Return true if given filepath exists
 */
bool sdlib_exists(const char *filepath) {
  return SD.exists(filepath);
}

/**
 * Sync the date with the SD filesystem
 */
void sdlib_sync(const char *filename) {
  if (sdfile) {
    sdfile.flush();
  }
}

/**
 * Return size of the file given by `filename`
 * or 0 if no such file exists
 */
size_t sdlib_size(const char *filename) {
  // check if sdfile is open
  size_t size = 0;
  sdfile.close();
  sdfile = SD.open(filename);
  if (sdfile) {
    // successfuly opened
    size = sdfile.size();
  }

  sdfile.close();
  return size;
}

/**
 * Seek by `new_offset`
 */
bool sdlib_seek(const char *filename, uint32_t new_offset) {
  // check if sdfile is open
  if (sdfile) {
    // check if it is the same file
    if (strcmp(sdfile.name(),filename) == 0) {
      // file is already open, return the position
      return sdfile.seek(new_offset);
    } else {
      // it is a different file, close it first
      sdfile.close();
    }
  }

  // we closed the old file
  // attempt to open the new file with FILE_WRITE, because we might want to write to it
  sdfile = SD.open(filename, FILE_WRITE);
  if (sdfile) {
    // successfuly opened
    return sdfile.seek(new_offset);
  }
  
  // nothing worked, return 0
  return false;
}

/**
 * `tell` is called `position`
 */
uint32_t sdlib_tell(const char *filename) {
  // check if sdfile is open
  if (sdfile) {
    // check if it is the same file
    if (strcmp(sdfile.name(),filename) == 0) {
      // file is already open, return the position
      return sdfile.position();
    } else {
      // it is a different file, close it first
      sdfile.close();
    }
  }

  // we closed the old file
  // attempt to open the new file with FILE_WRITE, because we might want to write to it
  sdfile = SD.open(filename, FILE_WRITE);
  if (sdfile) {
    // successfuly opened
    return sdfile.position();
  }
  
  // nothing worked, return 0
  return 0;
}

/**
 * Close the file, doesn't matter what filename it is, as long as it is open
 */
void sdlib_close(const char *filename) {
  (void)filename;
  sdfile.close();
}

/**
 * Open or create file specified by `filename` (O_READ | O_WRITE | O_CREAT | O_APPEND)
 * and write `buf` into it.
 * Return number of bytes written, or 0 if an error occured
 */
size_t sdlib_write_to_file(const char *filename, const uint8_t *buf,
                           size_t size) {
  size_t r = 0;
  // is the file already open?
  if (sdfile) {
    // check filename
    if (strcmp(sdfile.name(),filename) == 0) {
      r = sdfile.write((uint8_t *)buf, size);
      // return what we have
      return r;
    } else {
      // close it, because it is not our file
      sdfile.close();
    }
  }
  // open the sdfile. note that only one sdfile can be open at a time
  sdfile = SD.open(filename, FILE_WRITE);
  // if the sdfile opened okay, write to it:
  if (sdfile) {
    r = sdfile.write((uint8_t *)buf, size);
  }
  return r;
}

/**
 * Read from a file specified by `filename` 
 * Return 0 if no bytes were read.
 */
size_t sdlib_read_from_file(const char *filename, uint8_t *buf, size_t size) {
  size_t r = 0;
  if (sdfile) {
    // check filename
    if (strcmp(sdfile.name(),filename) == 0) {
      // read from the file until there's nothing else in it or we fill the buffer
      while (sdfile.available() && r < size) {
        uint8_t c = sdfile.read();
        buf[(uint16_t)r] = c;
        r++;
      }
      // return what we have
      return r; 
    } else {
      // close it, because it is not our file
      sdfile.close();
    }
  }
  // open the sdfile. note that only one sdfile can be open at a time,
  sdfile = SD.open(filename, FILE_WRITE);
  if (sdfile) {
    sdfile.seek(0); // reset to the beginning (FILE_WRITE moves it to the end)
    // read from the file until there's nothing else in it:
    while (sdfile.available() && r < size) {
      uint8_t c = sdfile.read();
      buf[(uint16_t)r] = c;
      r++;
    }
  }
  return r;
}
