/*
 * The MIT License (MIT)
 *
 * Copyright (c) <2015> <Stephan Gatzka>
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

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE response

#include <boost/test/unit_test.hpp>

#include "json/cJSON.h"
#include "peer.h"
#include "response.h"

extern "C" {
	void log_peer_err(const struct peer *p, const char *fmt, ...)
	{
		(void)p;
		(void)fmt;
	}
}

BOOST_AUTO_TEST_CASE(boolean_success_false)
{
	cJSON *id = cJSON_CreateString("request1");
	cJSON *response = create_boolean_success_response(NULL, id, 0);

	cJSON *result = cJSON_GetObjectItem(response, "result");
	BOOST_CHECK(result->type == cJSON_False);

	cJSON_Delete(id);
	cJSON_Delete(response);
}

BOOST_AUTO_TEST_CASE(boolean_success_true)
{
	cJSON *id = cJSON_CreateString("request1");
	cJSON *response = create_boolean_success_response(NULL, id, 1);

	cJSON *result = cJSON_GetObjectItem(response, "result");
	BOOST_CHECK(result->type == cJSON_True);

	cJSON_Delete(id);
	cJSON_Delete(response);
}

BOOST_AUTO_TEST_CASE(boolean_success_true_wrong_id_type)
{
	cJSON *id = cJSON_CreateBool(0);
	cJSON *response = create_boolean_success_response(NULL, id, 1);

	BOOST_CHECK(response == NULL);

	cJSON_Delete(id);
}

BOOST_AUTO_TEST_CASE(internal_error_response)
{
	const char *tag = "reason";
	const char *reason = "not enough memory";
	cJSON *id = cJSON_CreateString("request1");
	cJSON *error = create_internal_error(NULL, tag, reason);
	cJSON *response = create_error_response(NULL, id, error);

	cJSON *err = cJSON_GetObjectItem(response, "error");
	BOOST_CHECK(err->type == cJSON_Object);

	cJSON *code = cJSON_GetObjectItem(err, "code");
	BOOST_CHECK(code->type == cJSON_Number && code->valueint == -32603);

	cJSON *message = cJSON_GetObjectItem(err, "message");
	BOOST_CHECK(message->type == cJSON_String && strcmp(message->valuestring, "Internal error") == 0);
	cJSON *data = cJSON_GetObjectItem(err, "data");
	BOOST_CHECK(data != NULL && data->type == cJSON_Object);

	cJSON *r = cJSON_GetObjectItem(data, tag);
	BOOST_CHECK(r->type == cJSON_String && strcmp(r->valuestring, reason) == 0);


	cJSON_Delete(id);
	cJSON_Delete(response);
}

BOOST_AUTO_TEST_CASE(internal_error_response_wrong_id_type)
{
	const char *tag = "reason";
	const char *reason = "not enough memory";
	cJSON *id = cJSON_CreateBool(0);
	cJSON *error = create_internal_error(NULL, tag, reason);
	cJSON *response = create_error_response(NULL, id, error);

	BOOST_CHECK(response == NULL);

	cJSON_Delete(id);
	cJSON_Delete(error);
}

BOOST_AUTO_TEST_CASE(invalid_request_response)
{
	const char *tag = "reason";
	const char *reason = "neither request nor response";
	cJSON *id = cJSON_CreateString("request1");
	cJSON *error = create_invalid_request_error(NULL, tag, reason);
	cJSON *response = create_error_response(NULL, id, error);

	cJSON *err = cJSON_GetObjectItem(response, "error");
	BOOST_CHECK(err->type == cJSON_Object);

	cJSON *code = cJSON_GetObjectItem(err, "code");
	BOOST_CHECK(code->type == cJSON_Number && code->valueint == -32600);

	cJSON *message = cJSON_GetObjectItem(err, "message");
	BOOST_CHECK(message->type == cJSON_String && strcmp(message->valuestring, "Invalid Request") == 0);
	cJSON *data = cJSON_GetObjectItem(err, "data");
	BOOST_CHECK(data != NULL && data->type == cJSON_Object);

	cJSON *r = cJSON_GetObjectItem(data, tag);
	BOOST_CHECK(r->type == cJSON_String && strcmp(r->valuestring, reason) == 0);


	cJSON_Delete(id);
	cJSON_Delete(response);
}

BOOST_AUTO_TEST_CASE(method_not_found_response)
{
	const char *tag = "reason";
	const char *reason = "calling";
	cJSON *id = cJSON_CreateString("request1");
	cJSON *error = create_method_not_found_error(NULL, tag, reason);
	cJSON *response = create_error_response(NULL, id, error);

	cJSON *err = cJSON_GetObjectItem(response, "error");
	BOOST_CHECK(err->type == cJSON_Object);

	cJSON *code = cJSON_GetObjectItem(err, "code");
	BOOST_CHECK(code->type == cJSON_Number && code->valueint == -32601);

	cJSON *message = cJSON_GetObjectItem(err, "message");
	BOOST_CHECK(message->type == cJSON_String && strcmp(message->valuestring, "Method not found") == 0);
	cJSON *data = cJSON_GetObjectItem(err, "data");
	BOOST_CHECK(data != NULL && data->type == cJSON_Object);

	cJSON *r = cJSON_GetObjectItem(data, tag);
	BOOST_CHECK(r->type == cJSON_String && strcmp(r->valuestring, reason) == 0);


	cJSON_Delete(id);
	cJSON_Delete(response);
}

BOOST_AUTO_TEST_CASE(invalid_params_response)
{
	const char *tag = "not exists";
	const char *reason = "/foo/bar/";
	cJSON *id = cJSON_CreateString("request1");
	cJSON *error = create_invalid_params_error(NULL, tag, reason);
	cJSON *response = create_error_response(NULL, id, error);

	cJSON *err = cJSON_GetObjectItem(response, "error");
	BOOST_CHECK(err->type == cJSON_Object);

	cJSON *code = cJSON_GetObjectItem(err, "code");
	BOOST_CHECK(code->type == cJSON_Number && code->valueint == -32602);

	cJSON *message = cJSON_GetObjectItem(err, "message");
	BOOST_CHECK(message->type == cJSON_String && strcmp(message->valuestring, "Invalid params") == 0);
	cJSON *data = cJSON_GetObjectItem(err, "data");
	BOOST_CHECK(data != NULL && data->type == cJSON_Object);

	cJSON *r = cJSON_GetObjectItem(data, tag);
	BOOST_CHECK(r->type == cJSON_String && strcmp(r->valuestring, reason) == 0);


	cJSON_Delete(id);
	cJSON_Delete(response);
}

