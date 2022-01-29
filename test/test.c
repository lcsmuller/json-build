#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

#include "json-build.h"

#include "greatest.h"

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

SUITE(deep_nesting)
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
        "begin",
        "\nhi\n",
        "\n\t\t",
        "\b\a\a\ttest\n",
        "end"
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
    fprintf(stderr, "%s\n", buf);
    PASS();
}

SUITE(string_escaping)
{
    RUN_TEST(check_string_escaping);
}

/* TODO */
SUITE(memory)
{
}

GREATEST_MAIN_DEFS();

int
main(int argc, char *argv[])
{
    GREATEST_MAIN_BEGIN();

    RUN_SUITE(deep_nesting);
    RUN_SUITE(string_escaping);
    RUN_SUITE(memory);

    GREATEST_MAIN_END();
}
