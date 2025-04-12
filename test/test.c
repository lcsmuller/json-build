#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

#define JSONB_MAX_DEPTH 1028
#include "json-build.h"

#include "greatest.h"

TEST
check_valid_singles(void)
{
    static char buf[2048];
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_array(&b, buf, sizeof(buf)));
    ASSERT_EQm(buf, JSONB_END, jsonb_array_pop(&b, buf, sizeof(buf)));
    ASSERT_STR_EQ("[]", buf);
    ASSERT_EQ(sizeof("[]") - 1, b.pos);
    ASSERT_EQ(sizeof("[]") - 1, strlen(buf));

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_object(&b, buf, sizeof(buf)));
    ASSERT_EQm(buf, JSONB_END, jsonb_object_pop(&b, buf, sizeof(buf)));
    ASSERT_STR_EQ("{}", buf);
    ASSERT_EQ(sizeof("{}") - 1, b.pos);
    ASSERT_EQ(sizeof("{}") - 1, strlen(buf));

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_bool(&b, buf, sizeof(buf), 0));
    ASSERT_STR_EQ("false", buf);
    ASSERT_EQ(sizeof("false") - 1, b.pos);
    ASSERT_EQ(sizeof("false") - 1, strlen(buf));

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_bool(&b, buf, sizeof(buf), 1));
    ASSERT_STR_EQ("true", buf);
    ASSERT_EQ(sizeof("true") - 1, b.pos);
    ASSERT_EQ(sizeof("true") - 1, strlen(buf));

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_number(&b, buf, sizeof(buf), 10));
    ASSERT_STR_EQ("10", buf);
    ASSERT_EQ(sizeof("10") - 1, b.pos);
    ASSERT_EQ(sizeof("10") - 1, strlen(buf));

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_string(&b, buf, sizeof(buf), "hi", 2));
    ASSERT_STR_EQ("\"hi\"", buf);
    ASSERT_EQ(sizeof("\"hi\"") - 1, b.pos);
    ASSERT_EQ(sizeof("\"hi\"") - 1, strlen(buf));

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_null(&b, buf, sizeof(buf)));
    ASSERT_STR_EQ("null", buf);
    ASSERT_EQ(sizeof("null") - 1, b.pos);
    ASSERT_EQ(sizeof("null") - 1, strlen(buf));

    PASS();
}

TEST
check_valid_array(void)
{
    static const char expected[] = "[true,false,null,10,\"foo\",{}]";
    static char buf[2048];
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_array(&b, buf, sizeof(buf)));
    {
        ASSERT_EQm(buf, JSONB_OK, jsonb_bool(&b, buf, sizeof(buf), 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_bool(&b, buf, sizeof(buf), 0));
        ASSERT_EQm(buf, JSONB_OK, jsonb_null(&b, buf, sizeof(buf)));
        ASSERT_EQm(buf, JSONB_OK, jsonb_number(&b, buf, sizeof(buf), 10));
        ASSERT_EQm(buf, JSONB_OK,
                   jsonb_string(&b, buf, sizeof(buf), "foo", 3));
        ASSERT_EQm(buf, JSONB_OK, jsonb_object(&b, buf, sizeof(buf)));
        ASSERT_EQm(buf, JSONB_OK, jsonb_object_pop(&b, buf, sizeof(buf)));
        ASSERT_EQm(buf, JSONB_END, jsonb_array_pop(&b, buf, sizeof(buf)));
    }

    ASSERT_STR_EQ(expected, buf);
    ASSERT_EQ(sizeof(expected) - 1, b.pos);
    ASSERT_EQ(sizeof(expected) - 1, strlen(buf));

    PASS();
}

TEST
check_valid_object(void)
{
    static const char expected[] =
        "{\"a\":true,\"b\":false,\"c\":null,\"d\":10,\"e\":\"foo\",\"f\":[]}";
    static char buf[2048];
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_object(&b, buf, sizeof(buf)));
    {
        ASSERT_EQm(buf, JSONB_OK, jsonb_key(&b, buf, sizeof(buf), "a", 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_bool(&b, buf, sizeof(buf), 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key(&b, buf, sizeof(buf), "b", 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_bool(&b, buf, sizeof(buf), 0));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key(&b, buf, sizeof(buf), "c", 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_null(&b, buf, sizeof(buf)));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key(&b, buf, sizeof(buf), "d", 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_number(&b, buf, sizeof(buf), 10));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key(&b, buf, sizeof(buf), "e", 1));
        ASSERT_EQm(buf, JSONB_OK,
                   jsonb_string(&b, buf, sizeof(buf), "foo", 3));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key(&b, buf, sizeof(buf), "f", 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_array(&b, buf, sizeof(buf)));
        ASSERT_EQm(buf, JSONB_OK, jsonb_array_pop(&b, buf, sizeof(buf)));
        ASSERT_EQm(buf, JSONB_END, jsonb_object_pop(&b, buf, sizeof(buf)));
    }

    ASSERT_STR_EQ(expected, buf);
    ASSERT_EQ(sizeof(expected) - 1, b.pos);
    ASSERT_EQ(sizeof(expected) - 1, strlen(buf));

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
    static char buf[JSONB_MAX_DEPTH * sizeof("[]")];
    jsonb b;
    int i;

    jsonb_init(&b);
    for (i = 0; i < JSONB_MAX_DEPTH; ++i)
        ASSERT_EQm(buf, JSONB_OK, jsonb_array(&b, buf, sizeof(buf)));
    for (i = 0; i < JSONB_MAX_DEPTH - 1; ++i)
        ASSERT_EQm(buf, JSONB_OK, jsonb_array_pop(&b, buf, sizeof(buf)));
    ASSERT_EQm(buf, JSONB_END, jsonb_array_pop(&b, buf, sizeof(buf)));

    PASS();
}

TEST
check_deep_nesting_object(void)
{
    static char buf[JSONB_MAX_DEPTH * (sizeof("{}") + sizeof("\"a\":"))];
    jsonb b;
    int i;

    jsonb_init(&b);
    for (i = 0; i < JSONB_MAX_DEPTH - 1; ++i) {
        ASSERT_EQm(buf, JSONB_OK, jsonb_object(&b, buf, sizeof(buf)));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key(&b, buf, sizeof(buf), "a", 1));
    }
    ASSERT_EQm(buf, JSONB_OK, jsonb_object(&b, buf, sizeof(buf)));
    for (i = 0; i < JSONB_MAX_DEPTH - 1; ++i)
        ASSERT_EQm(buf, JSONB_OK, jsonb_object_pop(&b, buf, sizeof(buf)));
    ASSERT_EQm(buf, JSONB_END, jsonb_object_pop(&b, buf, sizeof(buf)));

    PASS();
}

TEST
check_deep_nesting_object_and_array(void)
{
    static char buf[JSONB_MAX_DEPTH * (sizeof("[{}]") + sizeof("\"a\":"))];
    jsonb b;
    int i;

    jsonb_init(&b);
    for (i = 0; i < JSONB_MAX_DEPTH; ++i) {
        if (i % 2 == 0) {
            ASSERT_EQm(buf, JSONB_OK, jsonb_object(&b, buf, sizeof(buf)));
            ASSERT_EQm(buf, JSONB_OK, jsonb_key(&b, buf, sizeof(buf), "a", 1));
        }
        else {
            ASSERT_EQm(buf, JSONB_OK, jsonb_array(&b, buf, sizeof(buf)));
        }
    }

    if (JSONB_MAX_DEPTH % 2 != 0) {
        ASSERT_EQm(buf, JSONB_OK, jsonb_null(&b, buf, sizeof(buf)));
        for (i = 0; i < JSONB_MAX_DEPTH - 1; ++i) {
            if (i % 2 == 0)
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_object_pop(&b, buf, sizeof(buf)));
            else
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_array_pop(&b, buf, sizeof(buf)));
        }
    }
    else {
        for (i = 0; i < JSONB_MAX_DEPTH - 1; ++i) {
            if (i % 2 == 0)
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_array_pop(&b, buf, sizeof(buf)));
            else
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_object_pop(&b, buf, sizeof(buf)));
        }
    }
    ASSERT_EQm(buf, JSONB_END, jsonb_object_pop(&b, buf, sizeof(buf)));

    PASS();
}

SUITE(nesting)
{
    RUN_TEST(check_deep_nesting_array);
    RUN_TEST(check_deep_nesting_object);
    RUN_TEST(check_deep_nesting_object_and_array);
}

TEST
check_string_escaping(void)
{
    static const char *const strs[] = {
        "begin", "\nhi\n", "\n\t\t", "\b\a\a\ttest\n", "end",
    };
    static const char *const expect[] = {
        "\"begin\"",      ",\"\\nhi\\n\"",
        ",\"\\n\\t\\t\"", ",\"\\b\\u0007\\u0007\\ttest\\n\"",
        ",\"end\"",
    };
    static char buf[1028];
    size_t i;
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_array(&b, buf, sizeof(buf)));
    for (i = 0; i < sizeof(strs) / sizeof(char *); ++i) {
        size_t len = strlen(strs[i]), prev_pos = b.pos;
        ASSERT_GTEm(buf, jsonb_string(&b, buf, sizeof(buf), strs[i], len), 0);
        ASSERT_STR_EQ(expect[i], buf + prev_pos);
    }
    ASSERT_EQm(buf, JSONB_END, jsonb_array_pop(&b, buf, sizeof(buf)));

    PASS();
}

TEST
check_string_escaping_bounds(void)
{
    static const char str[] = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
    static char buf[(sizeof("{}") - 1)
                    + 4 * ((sizeof("\"\"") - 1) + (sizeof(str) - 1))];
    jsonb b;

    jsonb_init(&b);
    jsonb_object(&b, buf, sizeof(buf));
    ASSERT_EQm(buf, JSONB_OK,
               jsonb_key(&b, buf, sizeof(buf), str, sizeof(str) - 1));
    ASSERT_EQm(buf, JSONB_OK,
               jsonb_string(&b, buf, sizeof(buf), str, sizeof(str) - 1));
    ASSERT_EQm(buf, JSONB_ERROR_NOMEM,
               jsonb_key(&b, buf, sizeof(buf), str, sizeof(str) - 1));

    PASS();
}

TEST
check_string_streaming(void)
{
    static const char expect[] =
        "{\"foo\":null,\"bar\":0,\"baz\":\"\",\"tuna\":{},\"spam\":[]}";
    static char buf[10] = { 0 }, dest[1024] = { 0 };
    enum jsonbcode code;
    int k;
    jsonb b;

    jsonb_init(&b);
    /* 1 < 10 : '{' */
    ASSERT_EQ(JSONB_OK, jsonb_object(&b, buf, sizeof(buf)));
    /* 7 < 10 : '{"foo":' */
    ASSERT_EQ(JSONB_OK, jsonb_key(&b, buf, sizeof(buf), "foo", 3));
    /* 11 > 10: '{"foo":null' */
    for (k = 0; (code = jsonb_null(&b, buf, sizeof(buf))); k = 1) {
        ASSERT_EQm("token can't fit alone in buffer", 0, k);
        ASSERT_EQ(JSONB_ERROR_NOMEM, code);
        strcat(dest, buf);
        jsonb_reset(&b);
    }
    /* 10 == 10 : 'null,"bar":' */
    for (k = 0; (code = jsonb_key(&b, buf, sizeof(buf), "bar", 3)); k = 1) {
        ASSERT_EQm("token can't fit alone in buffer", 0, k);
        ASSERT_EQ(JSONB_ERROR_NOMEM, code);
        strcat(dest, buf);
        jsonb_reset(&b);
    }
    /* 8 < 10 : ',"bar":0' */
    ASSERT_EQ(JSONB_OK, jsonb_number(&b, buf, sizeof(buf), 0));
    /* 15 > 10 : ',"bar":0,"baz":' */
    for (k = 0; (code = jsonb_key(&b, buf, sizeof(buf), "baz", 3)); k = 1) {
        ASSERT_EQm("token can't fit alone in buffer", 0, k);
        ASSERT_EQ(JSONB_ERROR_NOMEM, code);
        strcat(dest, buf);
        jsonb_reset(&b);
    }
    /* 9 < 10 : ',"baz":""' */
    ASSERT_EQ(JSONB_OK, jsonb_string(&b, buf, sizeof(buf), "", 0));
    /* 17 > 10 : ',"baz":"","tuna":' */
    for (k = 0; (code = jsonb_key(&b, buf, sizeof(buf), "tuna", 4)); k = 1) {
        ASSERT_EQm("token can't fit alone in buffer", 0, k);
        ASSERT_EQ(JSONB_ERROR_NOMEM, code);
        strcat(dest, buf);
        jsonb_reset(&b);
    }
    /* 9 < 10 : ',"tuna":{' */
    ASSERT_EQ(JSONB_OK, jsonb_object(&b, buf, sizeof(buf)));
    /* 10 == 10 : ',"tuna":{}' */
    for (k = 0; (code = jsonb_object_pop(&b, buf, sizeof(buf))); k = 1) {
        ASSERT_EQm("token can't fit alone in buffer", 0, k);
        ASSERT_EQ(JSONB_ERROR_NOMEM, code);
        strcat(dest, buf);
        jsonb_reset(&b);
    }
    /* 9 < 10 : '},"spam":' */
    ASSERT_EQ(JSONB_OK, jsonb_key(&b, buf, sizeof(buf), "spam", 4));
    /* 10 == 10 : '},"spam":[' */
    for (k = 0; (code = jsonb_array(&b, buf, sizeof(buf))); k = 1) {
        ASSERT_EQm("token can't fit alone in buffer", 0, k);
        ASSERT_EQ(JSONB_ERROR_NOMEM, code);
        strcat(dest, buf);
        jsonb_reset(&b);
    }
    /* 2 < 10 : '[]' */
    ASSERT_EQ(JSONB_OK, jsonb_array_pop(&b, buf, sizeof(buf)));
    /* 3 < 10 : '[]}' */
    ASSERT_EQ(JSONB_END, jsonb_object_pop(&b, buf, sizeof(buf)));
    strcat(dest, buf);

    ASSERT_STR_EQ(expect, dest);

    PASS();
}

SUITE(string)
{
    RUN_TEST(check_string_escaping);
    RUN_TEST(check_string_escaping_bounds);
    RUN_TEST(check_string_streaming);
}

TEST
check_invalid_top_level_tokens_in_sequence(void)
{
    static char buf[1028];
    jsonb b;

    jsonb_init(&b);
    jsonb_bool(&b, buf, sizeof(buf), 1);
    ASSERT_EQm(buf, JSONB_ERROR_INPUT, jsonb_bool(&b, buf, sizeof(buf), 0));

    jsonb_init(&b);
    jsonb_array(&b, buf, sizeof(buf));
    jsonb_array_pop(&b, buf, sizeof(buf));
    ASSERT_EQm(buf, JSONB_ERROR_INPUT, jsonb_array(&b, buf, sizeof(buf)));

    jsonb_init(&b);
    jsonb_array(&b, buf, sizeof(buf));
    jsonb_array_pop(&b, buf, sizeof(buf));
    ASSERT_EQm(buf, JSONB_ERROR_INPUT, jsonb_bool(&b, buf, sizeof(buf), 1));

    jsonb_init(&b);
    jsonb_bool(&b, buf, sizeof(buf), 1);
    ASSERT_EQm(buf, JSONB_ERROR_INPUT, jsonb_array(&b, buf, sizeof(buf)));

    jsonb_init(&b);
    jsonb_bool(&b, buf, sizeof(buf), 1);
    ASSERT_EQm(buf, JSONB_ERROR_INPUT,
               jsonb_string(&b, buf, sizeof(buf), "", 0));

    PASS();
}

TEST
check_not_enough_buffer_memory(void)
{
    static char buf[128];
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_ERROR_NOMEM, jsonb_bool(&b, buf, 0, 1));
    ASSERT_EQm(buf, JSONB_END, jsonb_bool(&b, buf, sizeof(buf), 1));

    PASS();
}

TEST
check_out_of_bounds_access(void)
{
    static char buf[JSONB_MAX_DEPTH * sizeof("[")];
    jsonb b;

    jsonb_init(&b);
    while (1) {
        if (JSONB_ERROR_STACK == jsonb_array(&b, buf, sizeof(buf))) break;
    }

    PASS();
}

TEST
check_single_no_operation_after_done(void)
{
    static char buf[1024];
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_null(&b, buf, sizeof(buf)));
    ASSERT_EQm(buf, JSONB_ERROR_INPUT, jsonb_null(&b, buf, sizeof(buf)));

    PASS();
}

TEST
check_array_no_operation_after_done(void)
{
    static char buf[1024];
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_array(&b, buf, sizeof(buf)));
    ASSERT_EQm(buf, JSONB_END, jsonb_array_pop(&b, buf, sizeof(buf)));
    ASSERT_EQm(buf, JSONB_ERROR_INPUT, jsonb_array(&b, buf, sizeof(buf)));

    PASS();
}

TEST
check_object_no_operation_after_done(void)
{
    static char buf[1024];
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_object(&b, buf, sizeof(buf)));
    ASSERT_EQm(buf, JSONB_END, jsonb_object_pop(&b, buf, sizeof(buf)));
    ASSERT_EQm(buf, JSONB_ERROR_INPUT, jsonb_object(&b, buf, sizeof(buf)));

    PASS();
}

SUITE(force_error)
{
    RUN_TEST(check_invalid_top_level_tokens_in_sequence);
    RUN_TEST(check_not_enough_buffer_memory);
    RUN_TEST(check_out_of_bounds_access);
    RUN_TEST(check_single_no_operation_after_done);
    RUN_TEST(check_array_no_operation_after_done);
    RUN_TEST(check_object_no_operation_after_done);
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
