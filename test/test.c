#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

#include "json-build.h"

#include "greatest.h"

TEST
check_valid_singles(void)
{
    char buf[2048] = { 0 };
    jsonb b;

    jsonb_init(&b);
    ASSERT(jsonb_push_array(&b, buf, sizeof(buf)) >= 0);
    ASSERT(jsonb_pop_array(&b, buf, sizeof(buf)) >= 0);
    ASSERT_STR_EQ("[]", buf);
    memset(buf, 0, sizeof(buf));

    jsonb_init(&b);
    ASSERT(jsonb_push_object(&b, buf, sizeof(buf)) >= 0);
    ASSERT(jsonb_pop_object(&b, buf, sizeof(buf)) >= 0);
    ASSERT_STR_EQ("{}", buf);
    memset(buf, 0, sizeof(buf));

    jsonb_init(&b);
    ASSERT(jsonb_push_bool(&b, 0, buf, sizeof(buf)) >= 0);
    ASSERT_STR_EQ("false", buf);
    memset(buf, 0, sizeof(buf));

    jsonb_init(&b);
    ASSERT(jsonb_push_bool(&b, 1, buf, sizeof(buf)) >= 0);
    ASSERT_STR_EQ("true", buf);
    memset(buf, 0, sizeof(buf));

    jsonb_init(&b);
    ASSERT(jsonb_push_number(&b, 10, buf, sizeof(buf)) >= 0);
    ASSERT_STR_EQ("10", buf);
    memset(buf, 0, sizeof(buf));

    jsonb_init(&b);
    ASSERT(jsonb_push_string(&b, "hi", 2, buf, sizeof(buf)) >= 0);
    ASSERT_STR_EQ("\"hi\"", buf);
    memset(buf, 0, sizeof(buf));

    jsonb_init(&b);
    ASSERT(jsonb_push_null(&b, buf, sizeof(buf)) >= 0);
    ASSERT_STR_EQ("null", buf);
    memset(buf, 0, sizeof(buf));

    PASS();
}

TEST
check_valid_array(void)
{
    char buf[2048] = { 0 };
    jsonb b;

    jsonb_init(&b);
    ASSERT(jsonb_push_array(&b, buf, sizeof(buf)) >= 0);
    {
        ASSERT(jsonb_push_bool(&b, 1, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_bool(&b, 0, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_null(&b, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_number(&b, 10, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_string(&b, "foo", 3, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_object(&b, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_pop_object(&b, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_pop_array(&b, buf, sizeof(buf)) >= 0);
    }

    ASSERT_STR_EQ("[true,false,null,10,\"foo\",{}]", buf);

    PASS();
}

TEST
check_valid_object(void)
{
    char buf[2048] = { 0 };
    jsonb b;

    jsonb_init(&b);
    ASSERT(jsonb_push_object(&b, buf, sizeof(buf)) >= 0);
    {
        ASSERT(jsonb_push_key(&b, "a", 1, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_bool(&b, 1, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_key(&b, "b", 1, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_bool(&b, 0, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_key(&b, "c", 1, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_null(&b, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_key(&b, "d", 1, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_number(&b, 10, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_key(&b, "e", 1, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_string(&b, "foo", 3, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_key(&b, "f", 1, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_array(&b, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_pop_array(&b, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_pop_object(&b, buf, sizeof(buf)) >= 0);
    }

    ASSERT_STR_EQ(
        "{\"a\":true,\"b\":false,\"c\":null,\"d\":10,\"e\":\"foo\",\"f\":[]}",
        buf);

    PASS();
}

SUITE(valid_output)
{
    RUN_TEST(check_valid_singles);
    RUN_TEST(check_valid_array);
    RUN_TEST(check_valid_object);
}

TEST
check_deep_nesting_array(void)
{
    char buf[2048] = { 0 };
    jsonb b;
    int i;

    jsonb_init(&b);
    for (i = 0; i < JSONB_MAX_DEPTH; ++i)
        ASSERT(jsonb_push_array(&b, buf, sizeof(buf)) >= 0);
    for (i = 0; i < JSONB_MAX_DEPTH; ++i)
        ASSERT(jsonb_pop_array(&b, buf, sizeof(buf)) >= 0);
    PASS();
}

TEST
check_deep_nesting_object(void)
{
    char buf[4096] = { 0 };
    jsonb b;
    int i;

    jsonb_init(&b);
    for (i = 0; i < JSONB_MAX_DEPTH - 1; ++i) {
        ASSERT(jsonb_push_object(&b, buf, sizeof(buf)) >= 0);
        ASSERT(jsonb_push_key(&b, "a", 1, buf, sizeof(buf)) >= 0);
    }
    ASSERT(jsonb_push_object(&b, buf, sizeof(buf)) >= 0);
    for (i = 0; i < JSONB_MAX_DEPTH; ++i) {
        ASSERT(jsonb_pop_object(&b, buf, sizeof(buf)) >= 0);
    }
    PASS();
}

TEST
check_deep_nesting_object_and_array(void)
{
    char buf[8192] = { 0 };
    jsonb b;
    int i;

    jsonb_init(&b);
    for (i = 0; i < JSONB_MAX_DEPTH; ++i) {
        if (i % 2 == 0) {
            ASSERT(jsonb_push_object(&b, buf, sizeof(buf)) >= 0);
            ASSERT(jsonb_push_key(&b, "a", 1, buf, sizeof(buf)) >= 0);
        }
        else {
            ASSERT(jsonb_push_array(&b, buf, sizeof(buf)) >= 0);
        }
    }
    for (i = 0; i < JSONB_MAX_DEPTH; ++i) {
        if (i % 2 == 0)
            ASSERT(jsonb_pop_array(&b, buf, sizeof(buf)) >= 0);
        else
            ASSERT(jsonb_pop_object(&b, buf, sizeof(buf)) >= 0);
    }
    PASS();
}

SUITE(nesting)
{
    RUN_TEST(check_deep_nesting_array);
    RUN_TEST(check_deep_nesting_object);
    RUN_TEST(check_deep_nesting_object_and_array);
}

/* TODO: check escaped strings against expected results */
TEST
check_string_escaping(void)
{
    char *const strs[] = {
        "begin", "\nhi\n", "\n\t\t", "\b\a\a\ttest\n", "end",
    };
    char buf[1028] = { 0 };
    size_t i;
    jsonb b;

    jsonb_init(&b);
    ASSERT(jsonb_push_array(&b, buf, sizeof(buf)) >= 0);
    for (i = 0; i < sizeof(strs) / sizeof(char *); ++i) {
        size_t len = strlen(strs[i]);
        ASSERT(jsonb_push_string(&b, strs[i], len, buf, sizeof(buf)) >= 0);
    }
    ASSERT(jsonb_pop_array(&b, buf, sizeof(buf)) >= 0);

    PASS();
}

SUITE(string)
{
    RUN_TEST(check_string_escaping);
}

TEST
check_invalid_top_level_tokens_in_sequence(void)
{
    char buf[1028] = { 0 };
    jsonb b;

    jsonb_init(&b);
    jsonb_push_bool(&b, 1, buf, sizeof(buf));
    ASSERT(jsonb_push_bool(&b, 0, buf, sizeof(buf)) == JSONB_ERROR_INPUT);

    jsonb_init(&b);
    jsonb_push_array(&b, buf, sizeof(buf));
    jsonb_pop_array(&b, buf, sizeof(buf));
    ASSERT(jsonb_push_array(&b, buf, sizeof(buf)) == JSONB_ERROR_INPUT);

    jsonb_init(&b);
    jsonb_push_array(&b, buf, sizeof(buf));
    jsonb_pop_array(&b, buf, sizeof(buf));
    ASSERT(jsonb_push_bool(&b, 1, buf, sizeof(buf)) == JSONB_ERROR_INPUT);

    jsonb_init(&b);
    jsonb_push_bool(&b, 1, buf, sizeof(buf));
    ASSERT(jsonb_push_array(&b, buf, sizeof(buf)) == JSONB_ERROR_INPUT);

    jsonb_init(&b);
    jsonb_push_bool(&b, 1, buf, sizeof(buf));
    ASSERT(jsonb_push_string(&b, "", 0, buf, sizeof(buf))
           == JSONB_ERROR_INPUT);

    PASS();
}

TEST
check_not_enough_buffer_memory(void)
{
    char buf[128] = { 0 };
    jsonb b;

    jsonb_init(&b);
    ASSERT(jsonb_push_bool(&b, 1, buf, 0) == JSONB_ERROR_NOMEM);
    ASSERT(jsonb_push_bool(&b, 1, buf, sizeof(buf)) == sizeof("true") - 1);

    PASS();
}

SUITE(force_error)
{
    RUN_TEST(check_invalid_top_level_tokens_in_sequence);
    RUN_TEST(check_not_enough_buffer_memory);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(valid_output);
    RUN_SUITE(nesting);
    RUN_SUITE(string);
    RUN_SUITE(force_error);

    GREATEST_MAIN_END();
}
