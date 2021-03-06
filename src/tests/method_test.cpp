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
#define BOOST_TEST_MODULE method

#include <boost/test/unit_test.hpp>

#include "json/cJSON.h"
#include "peer.h"
#include "router.h"
#include "method.h"

static const char *method_no_args_path = "/method_no_args/";

static const int INVALID_PARAMS_ERROR = -32602;

extern "C" {
	int send_message(struct peer *p, const char *rendered, size_t len)
	{
		(void)p;
		(void)rendered;
		(void)len;
		return 0;
	}

	int add_io(struct peer *p)
	{
		(void)p;
		return 0;
	}

	void remove_io(const struct peer *p)
	{
		(void)p;
		return;
	}

	void remove_all_states_from_peer(struct peer *p)
	{
		(void)p;
	}

	void remove_all_fetchers_from_peer(struct peer *p)
	{
		(void)p;
	}
}

struct F {
	F()
	{
		create_method_hashtable();
		owner_peer = alloc_peer(-1);
		call_peer = alloc_peer(-1);
	}
	~F()
	{
		free_peer(call_peer);
		free_peer(owner_peer);
		delete_method_hashtable();
	}

	struct peer *owner_peer;
	struct peer *call_peer;
};

static cJSON *create_call_json_rpc(const char *path_string)
{
	cJSON *root = cJSON_CreateObject();
	BOOST_REQUIRE(root != NULL);
	cJSON_AddStringToObject(root, "method", "call");
	cJSON_AddStringToObject(root, "id", "id_1");
	cJSON *params = cJSON_CreateObject();
	BOOST_REQUIRE(params != NULL);
	cJSON_AddItemToObject(root, "params", params);
	cJSON_AddStringToObject(params, "path", path_string);

	return root;
}

static cJSON *create_call_json_rpc_wrong_id_type(const char *path_string)
{
	cJSON *root = cJSON_CreateObject();
	BOOST_REQUIRE(root != NULL);
	cJSON_AddStringToObject(root, "method", "call");
	cJSON_AddTrueToObject(root, "id");
	cJSON *params = cJSON_CreateObject();
	BOOST_REQUIRE(params != NULL);
	cJSON_AddItemToObject(root, "params", params);
	cJSON_AddStringToObject(params, "path", path_string);

	return root;
}

static void check_invalid_params(const cJSON *error)
{
	cJSON *code = cJSON_GetObjectItem(error, "code");
	if (code != NULL) {
		BOOST_CHECK(code->type == cJSON_Number);
		BOOST_CHECK(code->valueint == INVALID_PARAMS_ERROR);
	} else {
		BOOST_FAIL("No code object!");
	}

	cJSON *message = cJSON_GetObjectItem(error, "message");
	if (message != NULL) {
		BOOST_CHECK(message->type == cJSON_String);
		BOOST_CHECK(strcmp(message->valuestring, "Invalid params") == 0);
	} else {
		BOOST_FAIL("No message object!");
	}
}

BOOST_FIXTURE_TEST_CASE(delete_nonexisting_state, F)
{
	const char path[] = "/foo/bar/";
	int ret = remove_method_from_peer(owner_peer, path);
	BOOST_CHECK(ret == -1);
}

BOOST_FIXTURE_TEST_CASE(call_wrong_path, F)
{

	cJSON *error = add_method_to_peer(owner_peer, "/foo/bar");
	BOOST_CHECK(error == NULL);

	cJSON *call_json_rpc = create_call_json_rpc(method_no_args_path);
	error = call_method(owner_peer, "/bar/foo", NULL, call_json_rpc);
	cJSON_Delete(call_json_rpc);

	if (error != NULL) {
		check_invalid_params(error);
		cJSON_Delete(error);
	} else {
		BOOST_FAIL("expected to get an error!");
	}
}

BOOST_FIXTURE_TEST_CASE(add_method_twice, F)
{
	const char path[] = "/foo/bar";

	cJSON *error = add_method_to_peer(owner_peer, path);
	BOOST_CHECK(error == NULL);

	error = add_method_to_peer(owner_peer, path);
	BOOST_REQUIRE(error != NULL);
	check_invalid_params(error);
	cJSON_Delete(error);
}

BOOST_FIXTURE_TEST_CASE(double_free_method, F)
{
	const char path[] = "/foo/bar";
	cJSON *error = add_method_to_peer(owner_peer, path);
	BOOST_CHECK(error == NULL);

	int ret = remove_method_from_peer(owner_peer, path);
	BOOST_CHECK(ret == 0);

	ret = remove_method_from_peer(owner_peer, path);
	BOOST_CHECK(ret == -1);
}


BOOST_FIXTURE_TEST_CASE(call_not_by_owner, F)
{
	cJSON *error = add_method_to_peer(owner_peer, method_no_args_path);
	BOOST_CHECK(error == NULL);

	cJSON *call_json_rpc = create_call_json_rpc(method_no_args_path);
	error = call_method(owner_peer, method_no_args_path, NULL, call_json_rpc);
	cJSON_Delete(call_json_rpc);
	if (error != NULL) {
		check_invalid_params(error);
		cJSON_Delete(error);
	} else {
		BOOST_FAIL("expected to get an error!");
	}
}

BOOST_FIXTURE_TEST_CASE(set_wrong_id_type, F)
{
	cJSON *error = add_method_to_peer(owner_peer, method_no_args_path);
	BOOST_CHECK(error == NULL);

	cJSON *call_json_rpc = create_call_json_rpc_wrong_id_type(method_no_args_path);
	error = call_method(call_peer, method_no_args_path, NULL, call_json_rpc);
	cJSON_Delete(call_json_rpc);

	if ((error != NULL) && (error != (cJSON *)ROUTED_MESSAGE)) {
		check_invalid_params(error);
		cJSON_Delete(error);
	} else  {
		BOOST_FAIL("expected to get an error!");
	}
}
