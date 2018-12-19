/*
 * File: FreeRTOS_HTTP_cgi.c
 *
 * Description:
 *   This file includes the HTTP server CGI implementations.
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

/* FreeRTOS+TCP includes. */
#include "FreeRTOSIPConfig.h"
#include "FreeRTOS_HTTP_commands.h"

/* FreeRTOS Protocol includes. */
#include "FreeRTOS_HTTP_cgi.h"

/* Local defines */
#define CGI_XML_START "<?xml version=\"1.0\" encoding=\"utf-8\"?><responseText>"
#define CGI_XML_END   "</responseText>"
#define MAX_REST_DATA_LEN 2048

/* Local function declarations */
static BaseType_t prvCgiLookup( const char *pcUrl, CGI_Registry_Entry_t **pxCgi );

/* Local data */
static CGI_Registry_Entry_t CgiRegistry[CGI_NUM_CGIS_MAX] = { 0 };
static int CgiCount = 0;

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/* Local function definitions */
static BaseType_t prvCgiLookup( const char *pcUrl, CGI_Registry_Entry_t **pxCgi )
{
BaseType_t xRc = 1;

    for( int i = 0; i < CgiCount; i++ )
    {
        if( 0 == strcmp( pcUrl, CgiRegistry[i].pcCgiUrl ) )
        {
            if( NULL != pxCgi )
            {
                (*pxCgi) = &CgiRegistry[i];
            }
            xRc = 0;
            break;
        }
    }

    return xRc;
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
/*-----------------------------------------------------------*/

/* External function definitions */
BaseType_t FreeRTOS_CGIRegister( const CGI_Registry_Entry_t *const pxCgi )
{
BaseType_t xRc = 1;

    if( CgiCount < CGI_NUM_CGIS_MAX )
    {
        if( 0 != prvCgiLookup( pxCgi->pcCgiUrl, NULL ) )
        {
            memcpy( &CgiRegistry[CgiCount], pxCgi, sizeof( CGI_Registry_Entry_t ) );
            CgiCount++;
            xRc = 0;
        }
    }

    return xRc;
}
/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_CGIArgsGet( const char *pcRestData, char *pcArgs, size_t szArgsLen )
{
BaseType_t xRc = 0;
const char pcArgsStartDelim[] = CGI_REST_ARGS_DELIM_START;
const char pcArgsEndDelim[] = CGI_REST_ARGS_DELIM_END;
char pcRestDataFixed[MAX_REST_DATA_LEN] = { 0 };

    pcArgs[0] = '\0';

    strncpy( pcRestDataFixed, pcRestData, sizeof(pcRestDataFixed) );
    pcRestDataFixed[MAX_REST_DATA_LEN - 1] = '\0';

    char *pcArgsStart = strstr( pcRestDataFixed, pcArgsStartDelim );
    if( NULL != pcArgsStart )
    {
        /* If have start delimiter but not end, then have malformed request */
        xRc = WEB_BAD_REQUEST;

        /* Find last instance of CGI_REST_ARGS_DELIM_END */
        char *pcArgsEnd = NULL;
        char *pcNextEndDelim = pcRestDataFixed;
        do
        {
            pcArgsEnd = pcNextEndDelim;
            pcNextEndDelim = strstr( pcNextEndDelim + 1, pcArgsEndDelim );
        } while( NULL != pcNextEndDelim );

        if( NULL != pcArgsEnd )
        {
            /* Have args start and end; now put into pcArgs */
            char *pcStartAddr = &pcArgsStart[strlen( pcArgsStartDelim )];
            size_t szRestArgsLen = (size_t)(pcArgsEnd - pcStartAddr);
            size_t szMinArgsLen = (szRestArgsLen < szArgsLen) ? szRestArgsLen : szArgsLen;

            strncpy( pcArgs, pcStartAddr, szMinArgsLen );
            pcArgs[szArgsLen - 1] = '\0';
            xRc = 0;
        }
    }

    return xRc;
}
/*-----------------------------------------------------------*/

BaseType_t FreeRTOS_CGIExec( const char *const pcUrl, char *pcWriteBuffer, size_t xWriteBufferLen,
                             const char *pcCgiArgs )
{
BaseType_t xRc = 0;
CGI_Registry_Entry_t *pxCgi = NULL;

    xRc = prvCgiLookup( pcUrl, &pxCgi );

    if( 0 == xRc )
    {
        DEBUG_PRINTF( "CGI lookup found for url: %s", pcUrl );
        /* Basic HTML start */
        int written = snprintf( pcWriteBuffer, xWriteBufferLen, CGI_XML_START );

        /* Exec the CGI */
        int availableLen = xWriteBufferLen - written - strlen( CGI_XML_END ) - 1;
        xRc = pxCgi->pxCgiFunc( &pcWriteBuffer[written], availableLen, pcCgiArgs );

        /* Basic HTML end */
        int currIndex = strnlen( pcWriteBuffer, xWriteBufferLen );
        snprintf( &pcWriteBuffer[currIndex], xWriteBufferLen - currIndex, CGI_XML_END );
        DEBUG_PRINTF( "CGI return text: \n\t%s", pcWriteBuffer );
    }
    else
    {
        DEBUG_PRINTF( "CGI lookup not found for url: %s", pcUrl );
        xRc = WEB_NOT_FOUND;
    }

    return xRc;
}
/*-----------------------------------------------------------*/


