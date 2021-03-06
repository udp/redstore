/*
    RedStore - a lightweight RDF triplestore powered by Redland
    Copyright (C) 2010-2011 Nicholas J Humfrey <njh@aelius.com>

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

#include <stdlib.h>
#include <string.h>

#include "redstore.h"

redhttp_response_t *handle_image_favicon(redhttp_request_t * request, void *user_data)
{
  static const unsigned char const_data[] = {
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x10,
    0x10, 0x00, 0x01, 0x00, 0x04, 0x00, 0x28, 0x01,
    0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdd, 0x7f,
    0x11, 0x00, 0x12, 0x14, 0x20, 0x00, 0x38, 0x2a,
    0x2d, 0x00, 0xf2, 0xbd, 0x33, 0x00, 0x0c, 0x15,
    0x4a, 0x00, 0x54, 0x49, 0x4a, 0x00, 0x10, 0x18,
    0x6f, 0x00, 0x88, 0x79, 0x70, 0x00, 0x24, 0x36,
    0x97, 0x00, 0xc4, 0xba, 0xa4, 0x00, 0x16, 0x1a,
    0xaa, 0x00, 0xf8, 0xf0, 0xc0, 0x00, 0x68, 0x79,
    0xc1, 0x00, 0x26, 0x36, 0xd4, 0x00, 0x19, 0x1e,
    0xd6, 0x00, 0x32, 0x4f, 0xdb, 0x00, 0x00, 0x09,
    0x75, 0x55, 0x55, 0x57, 0x90, 0x00, 0x07, 0x11,
    0x44, 0x66, 0x6a, 0x61, 0x11, 0x70, 0x1a, 0xee,
    0xee, 0xee, 0xe9, 0xfe, 0xee, 0xa1, 0x1e, 0xaa,
    0xaa, 0x6e, 0x30, 0x3d, 0xaa, 0xe2, 0x2e, 0xaa,
    0xdd, 0xe5, 0x39, 0x0d, 0xaa, 0xe2, 0x2e, 0xaf,
    0x00, 0x37, 0xe3, 0xfe, 0xaa, 0xe2, 0x2e, 0xe9,
    0x30, 0x7e, 0xe7, 0xaa, 0xaa, 0xe2, 0x2d, 0xaa,
    0xb0, 0x07, 0xd3, 0xda, 0xaa, 0xd2, 0x2d, 0x8a,
    0xae, 0xe7, 0x05, 0x3d, 0xaa, 0xd2, 0x2d, 0xa8,
    0xaa, 0xae, 0xb0, 0x0f, 0xaa, 0xd2, 0x2f, 0xdd,
    0xff, 0xfe, 0xcb, 0xfe, 0xfd, 0xf2, 0x2f, 0x86,
    0x44, 0x44, 0x46, 0x44, 0x68, 0xf2, 0x14, 0x44,
    0x68, 0x88, 0x88, 0x86, 0x44, 0x42, 0x1d, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xd1, 0x07, 0x14,
    0x46, 0x68, 0x86, 0x64, 0x11, 0x70, 0x00, 0x09,
    0x75, 0x55, 0x55, 0x57, 0x90, 0x00, 0xe0, 0x07,
    0x00, 0x00, 0x80, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01,
    0x00, 0x00, 0xe0, 0x07, 0x00, 0x00,
  };

  redhttp_response_t *response = redhttp_response_new_with_type(REDHTTP_OK, NULL, "image/x-icon");
  redhttp_response_add_header(response, "Last-Modified", BUILD_TIME);
  redhttp_response_set_content(response, (char *) const_data, sizeof(const_data), NULL);
  return response;
}
