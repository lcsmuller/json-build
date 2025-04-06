#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#define JSONB_MAX_DEPTH 1028
#include "json-build.h"

#include "greatest.h"

TEST
check_auto_singles(void)
{
    char *buf = malloc(16);
    size_t bufsize = 16;
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_array_auto(&b, &buf, &bufsize));
    ASSERT_EQm(buf, JSONB_END, jsonb_array_pop_auto(&b, &buf, &bufsize));
    ASSERT_STR_EQ("[]", buf);

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_object_auto(&b, &buf, &bufsize));
    ASSERT_EQm(buf, JSONB_END, jsonb_object_pop_auto(&b, &buf, &bufsize));
    ASSERT_STR_EQ("{}", buf);

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_bool_auto(&b, &buf, &bufsize, 0));
    ASSERT_STR_EQ("false", buf);

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_bool_auto(&b, &buf, &bufsize, 1));
    ASSERT_STR_EQ("true", buf);

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_number_auto(&b, &buf, &bufsize, 10));
    ASSERT_STR_EQ("10", buf);

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_string_auto(&b, &buf, &bufsize, "hi", 2));
    ASSERT_STR_EQ("\"hi\"", buf);

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END, jsonb_null_auto(&b, &buf, &bufsize));
    ASSERT_STR_EQ("null", buf);

    free(buf);
    PASS();
}

TEST
check_auto_array_realloc(void)
{
    char *buf = malloc(4);
    size_t bufsize = 4;
    size_t original_size = bufsize;
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_array_auto(&b, &buf, &bufsize));
    {
        ASSERT_EQm(buf, JSONB_OK, jsonb_bool_auto(&b, &buf, &bufsize, 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_bool_auto(&b, &buf, &bufsize, 0));
        ASSERT_EQm(buf, JSONB_OK, jsonb_null_auto(&b, &buf, &bufsize));
        ASSERT_EQm(buf, JSONB_OK, jsonb_number_auto(&b, &buf, &bufsize, 10));
        ASSERT_EQm(buf, JSONB_OK,
                   jsonb_string_auto(&b, &buf, &bufsize, "foo", 3));
        ASSERT_EQm(buf, JSONB_OK, jsonb_object_auto(&b, &buf, &bufsize));
        ASSERT_EQm(buf, JSONB_OK, jsonb_object_pop_auto(&b, &buf, &bufsize));
        ASSERT_EQm(buf, JSONB_END, jsonb_array_pop_auto(&b, &buf, &bufsize));
    }

    ASSERT_STR_EQ("[true,false,null,10,\"foo\",{}]", buf);
    ASSERT_GT(bufsize, original_size);

    free(buf);
    PASS();
}

TEST
check_auto_object_realloc(void)
{
    char *buf = malloc(4);
    size_t bufsize = 4;
    size_t original_size = bufsize;
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_object_auto(&b, &buf, &bufsize));
    {
        ASSERT_EQm(buf, JSONB_OK, jsonb_key_auto(&b, &buf, &bufsize, "a", 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_bool_auto(&b, &buf, &bufsize, 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key_auto(&b, &buf, &bufsize, "b", 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_bool_auto(&b, &buf, &bufsize, 0));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key_auto(&b, &buf, &bufsize, "c", 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_null_auto(&b, &buf, &bufsize));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key_auto(&b, &buf, &bufsize, "d", 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_number_auto(&b, &buf, &bufsize, 10));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key_auto(&b, &buf, &bufsize, "e", 1));
        ASSERT_EQm(buf, JSONB_OK,
                   jsonb_string_auto(&b, &buf, &bufsize, "foo", 3));
        ASSERT_EQm(buf, JSONB_OK, jsonb_key_auto(&b, &buf, &bufsize, "f", 1));
        ASSERT_EQm(buf, JSONB_OK, jsonb_array_auto(&b, &buf, &bufsize));
        ASSERT_EQm(buf, JSONB_OK, jsonb_array_pop_auto(&b, &buf, &bufsize));
        ASSERT_EQm(buf, JSONB_END, jsonb_object_pop_auto(&b, &buf, &bufsize));
    }

    ASSERT_STR_EQ(
        "{\"a\":true,\"b\":false,\"c\":null,\"d\":10,\"e\":\"foo\",\"f\":[]}",
        buf);
    ASSERT_GT(bufsize, original_size);

    free(buf);
    PASS();
}

TEST
check_auto_string_escaping(void)
{
    char *buf = malloc(8);
    size_t bufsize = 8;
    static const char *const strs[] = {
        "begin", "\nhi\n", "\n\t\t", "\b\a\a\ttest\n", "end",
    };
    size_t i;
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_array_auto(&b, &buf, &bufsize));
    for (i = 0; i < sizeof(strs) / sizeof(char *); ++i) {
        size_t len = strlen(strs[i]);
        ASSERT_GTEm(buf, jsonb_string_auto(&b, &buf, &bufsize, strs[i], len),
                    0);
    }
    ASSERT_EQm(buf, JSONB_END, jsonb_array_pop_auto(&b, &buf, &bufsize));

    ASSERT_STR_EQ("[\"begin\",\"\\nhi\\n\",\"\\n\\t\\t\","
                  "\"\\b\\u0007\\u0007\\ttest\\n\",\"end\"]",
                  buf);

    free(buf);
    PASS();
}

TEST
check_auto_nested_with_realloc(void)
{
    char *buf = malloc(4);
    size_t bufsize = 4;
    size_t original_size = bufsize;
    jsonb b;

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_OK, jsonb_object_auto(&b, &buf, &bufsize));
    {
        ASSERT_EQm(buf, JSONB_OK,
                   jsonb_key_auto(&b, &buf, &bufsize, "nested", 6));
        ASSERT_EQm(buf, JSONB_OK, jsonb_object_auto(&b, &buf, &bufsize));
        {
            ASSERT_EQm(buf, JSONB_OK,
                       jsonb_key_auto(&b, &buf, &bufsize, "array", 5));
            ASSERT_EQm(buf, JSONB_OK, jsonb_array_auto(&b, &buf, &bufsize));
            {
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_number_auto(&b, &buf, &bufsize, 1));
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_number_auto(&b, &buf, &bufsize, 2));
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_number_auto(&b, &buf, &bufsize, 3));
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_array_pop_auto(&b, &buf, &bufsize));
            }
            ASSERT_EQm(buf, JSONB_OK,
                       jsonb_key_auto(&b, &buf, &bufsize, "object", 6));
            ASSERT_EQm(buf, JSONB_OK, jsonb_object_auto(&b, &buf, &bufsize));
            {
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_key_auto(&b, &buf, &bufsize, "x", 1));
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_string_auto(&b, &buf, &bufsize, "y", 1));
                ASSERT_EQm(buf, JSONB_OK,
                           jsonb_object_pop_auto(&b, &buf, &bufsize));
            }
            ASSERT_EQm(buf, JSONB_OK,
                       jsonb_object_pop_auto(&b, &buf, &bufsize));
        }
        ASSERT_EQm(buf, JSONB_END, jsonb_object_pop_auto(&b, &buf, &bufsize));
    }

    ASSERT_STR_EQ("{\"nested\":{\"array\":[1,2,3],\"object\":{\"x\":\"y\"}}}",
                  buf);
    ASSERT_GT(bufsize, original_size);

    free(buf);
    PASS();
}

TEST
check_auto_large_string(void)
{
    char *longString = malloc(1000);
    char *buf = malloc(8);
    size_t bufsize = 8;
    size_t original_size = bufsize;
    jsonb b;
    int i;

    /* Fill the string with repeating pattern */
    for (i = 0; i < 999; i++) {
        longString[i] = 'a' + (i % 26);
    }
    longString[999] = '\0';

    jsonb_init(&b);
    ASSERT_EQm(buf, JSONB_END,
               jsonb_string_auto(&b, &buf, &bufsize, longString, 999));

    ASSERT_GT(bufsize, original_size);
    /* Verify the first and last characters to confirm the string was stored
     *  correctly */
    ASSERT_EQ('"', buf[0]);
    ASSERT_EQ('"', buf[1000]);
    ASSERT_EQ('\0', buf[1001]);

    free(longString);
    free(buf);
    PASS();
}

SUITE(auto_functions)
{
    RUN_TEST(check_auto_singles);
    RUN_TEST(check_auto_array_realloc);
    RUN_TEST(check_auto_object_realloc);
    RUN_TEST(check_auto_string_escaping);
    RUN_TEST(check_auto_nested_with_realloc);
    RUN_TEST(check_auto_large_string);
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(auto_functions);

    GREATEST_MAIN_END();
}
