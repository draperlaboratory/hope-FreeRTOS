/*
 * File: cgi.c
 *
 * Description:
 *   This file implements CGI setup functions.
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

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOS_HTTP_cgi.h"
#include "task.h"

/* App includes. */
#include "UDPLoggingPrintf.h"
#include "cgi.h"

/* Function declarations */

extern BaseType_t CgiDashboard( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );
extern BaseType_t CgiLogin( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );
extern BaseType_t CgiRegister( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );

static void CgiRegisterSingle( const char *pcUrl, pdCGI_CALLBACK pxFunc );

/* Function definitions */
void CgiSetup( void )
{
    printf("Setting up CGI...\n");
    CgiRegisterSingle( "/cgi-bin/dashboard.cgi", CgiDashboard );
    CgiRegisterSingle( "/cgi-bin/login.cgi", CgiLogin );
    CgiRegisterSingle( "/cgi-bin/register.cgi", CgiRegister );
}

static void CgiRegisterSingle( const char *pcUrl, pdCGI_CALLBACK pxFunc )
{
    BaseType_t rv;

    CGI_Registry_Entry_t entry = {
        pcUrl,
        pxFunc,
    };
    rv = FreeRTOS_CGIRegister( &entry );
    if( rv != 0 )
    {
        FreeRTOS_debug_printf( ("Error registering CGI for URL %s\n", pcUrl) );
    }
}
