/*
 * File: httpStatuses.h
 *
 * Description:
 *   Define HTTP statuses.
 *
 * Copyright DornerWorks 2019
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

#ifndef HTTP_STATUSES_H
#define	HTTP_STATUSES_H

//enum {
//    HTTP_OK = 200,
//    HTTP_NO_CONTENT = 204,
//    HTTP_SEE_OTHER = 303,
//    HTTP_BAD_REQUEST = 400,
//    HTTP_UNAUTHORIZED = 401,
//    HTTP_NOT_FOUND = 404,
//    HTTP_GONE = 410,
//    HTTP_PRECONDITION_FAILED = 412,
//    HTTP_INTERNAL_SERVER_ERROR = 500,
//};
#define HTTP_OK 200
#define HTTP_NO_CONTENT 204
#define HTTP_SEE_OTHER 303
#define HTTP_BAD_REQUEST 400
#define HTTP_UNAUTHORIZED 401
#define HTTP_NOT_FOUND 404
#define HTTP_GONE 410
#define HTTP_PRECONDITION_FAILED 412
#define HTTP_INTERNAL_SERVER_ERROR 500

#endif /* HTTP_STATUSES_H */
