/* Copyright © 2017-2019 The Charles Stark Draper Laboratory, Inc. and/or Dover Microsystems, Inc. */
/* All rights reserved. */

/* Use and disclosure subject to the following license. */

/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the */
/* "Software"), to deal in the Software without restriction, including */
/* without limitation the rights to use, copy, modify, merge, publish, */
/* distribute, sublicense, and/or sell copies of the Software, and to */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions: */

/* The above copyright notice and this permission notice shall be */
/* included in all copies or substantial portions of the Software. */

/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE */
/* LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION */
/* OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION */
/* WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOS_HTTP_cgi.h"
#include "task.h"

/* App includes. */
#include "UDPLoggingPrintf.h"
#include "cgi.h"

/* Function declarations */

extern BaseType_t CgiAddRecord( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );
extern BaseType_t CgiDashboard( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );
extern BaseType_t CgiLogin( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );
extern BaseType_t CgiLogout( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );
extern BaseType_t CgiRegister( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );
extern BaseType_t CgiSearchResults( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );
extern BaseType_t CgiUpdateUser( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );
extern BaseType_t CgiUserDetails( char *pcWriteBuffer, size_t xWriteBufferLen,
                    char *pcHeaderBuffer, size_t xHeaderBufferLen,
                    const char *pcCgiArgs );

static void CgiRegisterSingle( const char *pcUrl, pdCGI_CALLBACK pxFunc );

/* Function definitions */
void CgiSetup( void )
{
    printf("Setting up CGI...\n");
    CgiRegisterSingle( "/cgi-bin/add-record.cgi", CgiAddRecord );
    CgiRegisterSingle( "/cgi-bin/dashboard.cgi", CgiDashboard );
    CgiRegisterSingle( "/cgi-bin/login.cgi", CgiLogin );
    CgiRegisterSingle( "/cgi-bin/logout.cgi", CgiLogout );
    CgiRegisterSingle( "/cgi-bin/register.cgi", CgiRegister );
    CgiRegisterSingle( "/cgi-bin/search-results.cgi", CgiSearchResults );
    CgiRegisterSingle( "/cgi-bin/update-user.cgi", CgiUpdateUser );
    CgiRegisterSingle( "/cgi-bin/user-details.cgi", CgiUserDetails );
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
