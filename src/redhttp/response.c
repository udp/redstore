/*
    RedHTTP - a lightweight HTTP server library
    Copyright (C) 2010 Nicholas J Humfrey <njh@aelius.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>

#include <errno.h>
#include <sys/types.h>

#include "redhttp.h"


redhttp_response_t *redhttp_response_new(int code, const char *message)
{
    redhttp_response_t *response;

    assert(code >= 100 && code < 1000);

    response = calloc(1, sizeof(redhttp_response_t));
    if (!response) {
        perror("failed to allocate memory for redhttp_response_t");
        return NULL;
    } else {
        response->status_code = code;
        if (message == NULL) {
            switch (code) {
            case REDHTTP_OK:
                message = "OK";
                break;
            case REDHTTP_CREATED:
                message = "Created";
                break;
            case REDHTTP_ACCEPTED:
                message = "Accepted";
                break;
            case REDHTTP_NO_CONTENT:
                message = "No Content";
                break;
            case REDHTTP_MOVED_PERMANENTLY:
                message = "Moved Permanently";
                break;
            case REDHTTP_MOVED_TEMPORARILY:
                message = "Moved Temporarily";
                break;
            case REDHTTP_SEE_OTHER:
                message = "See Other";
                break;
            case REDHTTP_NOT_MODIFIED:
                message = "Not Modified";
                break;
            case REDHTTP_BAD_REQUEST:
                message = "Bad Request";
                break;
            case REDHTTP_UNAUTHORIZED:
                message = "Unauthorized";
                break;
            case REDHTTP_FORBIDDEN:
                message = "Forbidden";
                break;
            case REDHTTP_NOT_FOUND:
                message = "Not Found";
                break;
            case REDHTTP_METHOD_NOT_ALLOWED:
                message = "Method Not Allowed";
                break;
            case REDHTTP_INTERNAL_SERVER_ERROR:
                message = "Internal Server Error";
                break;
            case REDHTTP_NOT_IMPLEMENTED:
                message = "Not Implemented";
                break;
            case REDHTTP_BAD_GATEWAY:
                message = "Bad Gateway";
                break;
            case REDHTTP_SERVICE_UNAVAILABLE:
                message = "Service Unavailable";
                break;
            default:
                message = "Unknown";
                break;
            }
        }
        response->status_message = calloc(1, strlen(message) + 1);
        strcpy(response->status_message, message);
    }

    return response;
}

redhttp_response_t *redhttp_response_new_error_page(int code, const char *explanation)
{
    redhttp_response_t *response = redhttp_response_new(code, NULL);

    assert(code >= 100 && code < 1000);
    if (!response)
        return NULL;

    redhttp_headers_add(&response->headers, "Content-Type", "text/html");
    redhttp_response_content_append(response,
                                    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    redhttp_response_content_append(response,
                                    "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\""
                                    " \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n");
    redhttp_response_content_append(response, "<html>\n");
    redhttp_response_content_append(response,
                                    "<head><title>%d %s</title></head>\n", code,
                                    response->status_message);
    redhttp_response_content_append(response, "<body><h1>%d %s</h1>\n", code,
                                    response->status_message);

    if (explanation) {
        redhttp_response_content_append(response, "<p>%s</p>\n", explanation);
    }
    redhttp_response_content_append(response, "</body></html>\n");

    return response;
}

redhttp_response_t *redhttp_response_new_redirect(const char *url)
{
    static const char MESSAGE_FMT[] = "The document has moved <a href=\"%s\">here</a>.";
    redhttp_response_t *response;
    size_t message_length;
    char *message;

    message_length = snprintf(NULL, 0, MESSAGE_FMT, url);
    message = malloc(message_length + 1);
    if (!message)
        return NULL;
    snprintf(message, message_length + 1, MESSAGE_FMT, url);

    // Build the response
    response = redhttp_response_new_error_page(REDHTTP_MOVED_PERMANENTLY, message);
    if (response)
        redhttp_response_add_header(response, "Location", url);
    free(message);

    return response;
}

redhttp_response_t *redhttp_response_new_with_content(const char *data,
                                                      size_t length, const char *type)
{
    redhttp_response_t *response = redhttp_response_new(REDHTTP_OK, NULL);
    if (response)
        redhttp_response_set_content(response, data, length, type);
    return response;
}

int redhttp_response_content_append(redhttp_response_t * response, const char *fmt, ...)
{
    va_list args;
    int len;

    assert(response != NULL);
    assert(fmt != NULL);

    if (strlen(fmt) == 0)
        return 0;

    // Get the length of the formatted string
    va_start(args, fmt);
    len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    // Does the buffer already exist?
    if (!response->content_buffer) {
        response->content_buffer = malloc(BUFSIZ + len);
        if (response->content_buffer) {
            response->content_buffer_size = BUFSIZ + len;
            response->content_length = 0;
        } else {
            // Memory allocation error
            response->content_buffer_size = 0;
            response->content_length = 0;
            return -1;
        }
    }
    // Is the buffer big enough?
    if (response->content_buffer_size - response->content_length < len) {
        int new_size = response->content_buffer_size + len + BUFSIZ;
        char *new_buf = realloc(response->content_buffer, new_size);
        if (new_buf) {
            response->content_buffer_size = new_size;
            response->content_buffer = new_buf;
        } else {
            // Memory allocation error
            return -1;
        }
    }
    // Add formatted string the buffer
    va_start(args, fmt);
    response->content_length +=
        vsnprintf(&response->content_buffer[response->content_length],
                  response->content_buffer_size - response->content_length, fmt, args);
    va_end(args);

    // Success
    return 0;
}

const char *redhttp_response_get_header(redhttp_response_t * response, const char *key)
{
    return redhttp_headers_get(&response->headers, key);
}

void redhttp_response_add_header(redhttp_response_t * response, const char *key, const char *value)
{
    redhttp_headers_add(&response->headers, key, value);
}

void redhttp_response_add_time_header(redhttp_response_t * response, const char *key, time_t timer)
{
    static const char RFC1123FMT[] = "%a, %d %b %Y %H:%M:%S GMT";
    char *date_str = malloc(BUFSIZ);
    struct tm *time_tm = gmtime(&timer);

    if (!date_str)
        return;

    // FIXME: calculate the length of the date string instead of using BUFSIZ
    strftime(date_str, BUFSIZ, RFC1123FMT, time_tm);
    redhttp_headers_add(&response->headers, key, date_str);

    free(date_str);
}


void redhttp_response_set_content(redhttp_response_t * response,
                                  const char *data, size_t length, const char *type)
{
    char *new_buf;

    assert(response != NULL);
    assert(data != NULL);
    assert(length > 0);
    assert(type != NULL);

    new_buf = realloc(response->content_buffer, length);
    if (new_buf) {
        memcpy(new_buf, data, length);
        response->content_buffer = new_buf;
        response->content_buffer_size = length;
        response->content_length = length;
        redhttp_headers_add(&response->headers, "Content-Type", type);
    }
}


void redhttp_response_send(redhttp_response_t * response, redhttp_request_t * request)
{
    assert(request != NULL);
    assert(response != NULL);

    if (!response->headers_sent) {
        const char *signature = redhttp_server_get_signature(request->server);

        redhttp_response_add_time_header(response, "Date", time(NULL));
        redhttp_response_add_header(response, "Connection", "Close");

        if (signature)
            redhttp_response_add_header(response, "Server", signature);

        if (response->content_length) {
            // FIXME: must be better way to do int to string
            char *length_str = malloc(BUFSIZ);
            if (length_str) {
                snprintf(length_str, BUFSIZ, "%d", (int) response->content_length);
                redhttp_response_add_header(response, "Content-Length", length_str);
                free(length_str);
            }
        }

        if (strncmp(request->version, "0.9", 3)) {
            fprintf(request->socket, "HTTP/1.0 %d %s\r\n",
                    response->status_code, response->status_message);
            redhttp_headers_send(&response->headers, request->socket);
            fputs("\r\n", request->socket);
        }

        response->headers_sent = 1;
    }

    if (response->content_buffer) {
        int written = 0;
        assert(response->content_length > 0);
        written = fwrite(response->content_buffer, 1, response->content_length, request->socket);
        if (written != response->content_length) {
            perror("failed to write response to client");
        }
    }
}

int redhttp_response_get_status_code(redhttp_response_t * response)
{
    return response->status_code;
}

const char *redhttp_response_get_status_message(redhttp_response_t * response)
{
    return response->status_message;
}

void redhttp_response_free(redhttp_response_t * response)
{
    assert(response != NULL);

    if (response->status_message)
        free(response->status_message);
    if (response->content_buffer)
        free(response->content_buffer);

    redhttp_headers_free(&response->headers);
    free(response);
}
