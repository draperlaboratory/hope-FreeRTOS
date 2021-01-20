#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <string.h>
#include "SDLib.h"
void sd_demo(void);
uint8_t sd_test_buf[2048] = {0};

void sd_demo(void) {
    const char *log1 = "test0.log";
    const char *log2 = "test1.log";

    const char * entry1 =
        "hello "
        "test01xxxxaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbccccccccccccccccdddddddddddd"
        "ddddeeeeeeeeeeeeeeeeffffffffffffffffgggggggggggggggghhhhhhhhhhhhhhhhii"
        "iiiiiiiiiiiiiijjjjjjjjjjjjjjjjkkkkkkkkkkkkkkkkllllllllllllllllmmmmmmmm"
        "mmmmmmmmnnnnnnnnnnnnnnnnooooooooooooooo"; // 256 chars including final \0

    const char * entry2 =
        "hello "
        "test01xxxxaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbccccccccccccccccdddddddddddd"
        "ddddeeeeeeeeeeeeeeeeffffffffffffffffgggggggggggggggghhhhhhhhhhhhhhhhii"
        "iiiiiiiiiiiiiijjjjjjjjjjjjjjjjkkkkkkkkkkkkkkkkllllllllllllllllmmmmmmmm"
        "mmmmmmmmnnnnnnnnnnnnnnnnooooooooooooooo"; // 256 chars including final \0

    configASSERT(sdlib_initialize());
    vTaskDelay(pdMS_TO_TICKS(1000));

    size_t r;
    uint16_t cnt = 1;
    for (;;)
    {   
        r = sdlib_write_to_file(log1, (const uint8_t*)entry1, strlen(entry1));
        printf("#%u: %s written %u bytes\r\n", cnt, log1, r);
        r = sdlib_write_to_file(log2, (const uint8_t*)entry2, strlen(entry2));
        printf("#%u: %s written %u bytes\r\n", cnt, log2, r);
        r = sdlib_read_from_file(log1, sd_test_buf, sizeof(sd_test_buf));
        printf("#%u: %s read %u bytes\r\n", cnt, log1, r);
        r = sdlib_read_from_file(log2, sd_test_buf, sizeof(sd_test_buf));
        printf("#%u: %s read %u bytes\r\n", cnt, log2, r);
        cnt++;
        // Include small delay so we can show stats
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}