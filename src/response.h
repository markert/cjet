/*
 *The MIT License (MIT)
 *
 * Copyright (c) <2014> <Stephan Gatzka>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CJET_RESPONSE_H
#define CJET_RESPONSE_H

#include "json/cJSON.h"
#include "peer.h"

#ifdef __cplusplus
extern "C" {
#endif

static const int TRUE = 1;
static const int FALSE = 0;

cJSON *create_invalid_request_error(const struct peer *p, const char *tag, const char *reason);
cJSON *create_invalid_params_error(const struct peer *p, const char *tag, const char *reason);
cJSON *create_internal_error(const struct peer *p, const char *tag, const char *reason);
cJSON *create_method_not_found_error(const struct peer *p, const char *tag, const char *reason);
cJSON *create_error_response(const struct peer *p, const cJSON *id, cJSON *error);
cJSON *create_boolean_success_response(const struct peer *p, const cJSON *id, int true_false);
cJSON *create_result_response(const struct peer *p, const cJSON *id, cJSON *result, const char *result_type);

#ifdef __cplusplus
}
#endif

#endif
