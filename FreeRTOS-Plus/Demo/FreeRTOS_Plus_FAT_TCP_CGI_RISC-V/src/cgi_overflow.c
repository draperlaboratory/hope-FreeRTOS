/*
 * File: cgi_overflow.c
 *
 * Description:
 *   This file implements the Buffer Overflow CGI.
 *
 * Copyright DornerWorks 2018
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1.  Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DORNERWORKS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Standard includes. */
#include <string.h>
#include <stdio.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* Defines */
#define OVERFLOW_LEN 8
#define QUOTE_LEN 256
#define TOTAL_LEN (OVERFLOW_LEN + QUOTE_LEN)
#define QUOTE \
    "The trouble with quotes on the Internet is that you never know if they are genuine." \
    "<br/><i> - Benjamin Franklin</i>"

/* Types */
struct strings_s
{
    char user_input[OVERFLOW_LEN];
    char quote[QUOTE_LEN];
};

/* Function definitions */
BaseType_t CgiOverflow( char *pcWriteBuffer, size_t xWriteBufferLen,
                               const char *pcCgiArgs )
{
    struct strings_s strings = {
        { 0 },
        QUOTE,
    };

    int maxCopyLen = strnlen( pcCgiArgs, TOTAL_LEN );

    /*
     * This strncpy invocation is specifically designed to cause a buffer overflow that results in
     * the fouling up of the quote.
     *
     * A correct invocation would get the maxCopyLen from the target buffer, not the source
     * buffer.
     */
    strncpy( strings.user_input, pcCgiArgs, maxCopyLen );

    /*
     * Copy the string to the output buffer. This is a correct strncpy.
     */
    strncpy( pcWriteBuffer, strings.quote, xWriteBufferLen );

    return 0;
}

