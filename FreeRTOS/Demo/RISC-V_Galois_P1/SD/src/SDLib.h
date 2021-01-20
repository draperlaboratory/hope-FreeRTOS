#ifdef __cplusplus
extern "C" {
#endif
#ifndef __SD_LIB_H__
#define __SD_LIB_H__

#include <stddef.h>
#include <stdint.h>

/**
 * Initialize SD card
 * Has to be called once before any other SDLib function
 */
extern int sdlib_initialize(void);

/**
 * Return true if given filepath exists
 */
extern bool sdlib_exists(const char *filepath);

/**
 * Return size of the file given by `filename`
 * or 0 if no such file exists
 */
extern size_t sdlib_size(const char *filename);

/**
 * Seek by `new_offset`
 */
extern bool sdlib_seek(const char *filename, uint32_t new_offset);

/**
 * `tell` is called `position`
 */
extern uint32_t sdlib_tell(const char *filename);

/**
 * Sync the date with the SD filesystem
 */
extern void sdlib_sync(const char *filename);

/**
 * Close the file
 */
extern void sdlib_close(const char *filename);

/**
 * Open or create file specified by `filename`
 * and write `buf` into it.
 * Return number of bytes written, or 0 if an error occured
 */
extern size_t sdlib_write_to_file(const char *filename, const uint8_t *buf,
                                  size_t size);

/**
 * Read from a file specified by `filename` 
 * Return 0 if no bytes were read.
 */
extern size_t sdlib_read_from_file(const char *filename, uint8_t *buf,
                                   size_t size);

#endif // __SD_LIB_H__
#ifdef __cplusplus
}
#endif