#include <stddef.h>

#include "json-build.h"

#ifndef _JSONB_DEBUG
#define STACK_PUSH(jsonb, state) *++(jsonb)->st_top = (state)
#define STACK_POP(jsonb)         --(jsonb)->st_top
#define STACK_HEAD(jsonb, state) *(jsonb)->st_top = (state)
#else
#include <stdio.h>
static const char *
str(enum jsonb_state state)
{
#define RETURN_CASE(macro)                                                    \
    case macro:                                                               \
        return #macro
    switch (state) {
        RETURN_CASE(JSONB_ARRAY_OR_OBJECT_OR_VALUE);
        RETURN_CASE(JSONB_OBJECT_KEY_OR_CLOSE);
        RETURN_CASE(JSONB_OBJECT_NEXT_KEY_OR_CLOSE);
        RETURN_CASE(JSONB_OBJECT_VALUE);
        RETURN_CASE(JSONB_ARRAY_VALUE_OR_CLOSE);
        RETURN_CASE(JSONB_ARRAY_NEXT_VALUE_OR_CLOSE);
        RETURN_CASE(JSONB_DONE);
        RETURN_CASE(JSONB_ERROR);
    default:
        return "UNKNOWN";
    }
#undef RETURN_CASE
}

#define STACK_PUSH(jsonb, state)                                              \
    do {                                                                      \
        *++(jsonb)->st_top = (state);                                         \
        fprintf(stderr, "push %s\n", str(*jsonb->st_top));                    \
    } while (0)
#define STACK_POP(jsonb)                                                      \
    do {                                                                      \
        --(jsonb)->st_top;                                                    \
        fprintf(stderr, "pop %s\n", str(*jsonb->st_top));                     \
    } while (0)
#define STACK_HEAD(jsonb, state)                                              \
    do {                                                                      \
        fprintf(stderr, "update %s ->", str(*jsonb->st_top));                 \
        *(jsonb)->st_top = (state);                                           \
        fprintf(stderr, " %s\n", str(*jsonb->st_top));                        \
    } while (0)
#endif

#define EXPECT_LEN(jsonb, len, bufsize)                                       \
                                                                              \
    do {                                                                      \
        if ((jsonb)->pos + (len) > (bufsize)) {                               \
            return JSONB_ERROR_NOMEM;                                         \
        }                                                                     \
    } while (0)
#define BUFFER_COPY_CHAR(jsonb, c, buf, bufsize)                              \
    do {                                                                      \
        (buf)[(jsonb)->pos++] = (c);                                          \
    } while (0)
#define BUFFER_COPY(jsonb, value, len, buf, bufsize)                          \
    do {                                                                      \
        size_t i;                                                             \
        for (i = 0; i < (len); ++i)                                           \
            (buf)[(jsonb)->pos + i] = (value)[i];                             \
        (jsonb)->pos += (len);                                                \
    } while (0)

void
jsonb_init(jsonb *builder)
{
    static jsonb empty_builder;
    *builder = empty_builder;
    builder->st_top = builder->st_stack;
    STACK_HEAD(builder, JSONB_DONE);
    STACK_PUSH(builder, JSONB_ARRAY_OR_OBJECT_OR_VALUE);
}

int
jsonb_push_object(jsonb *builder, char buf[], size_t bufsize)
{
    switch (*builder->st_top) {
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        EXPECT_LEN(builder, 2, bufsize);
        BUFFER_COPY_CHAR(builder, ',', buf, bufsize);
        /* fall-through */
    case JSONB_OBJECT_VALUE:
        STACK_HEAD(builder, JSONB_OBJECT_NEXT_KEY_OR_CLOSE);
        /* fall-through */
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        EXPECT_LEN(builder, 1, bufsize);
        BUFFER_COPY_CHAR(builder, '{', buf, bufsize);
        STACK_PUSH(builder, JSONB_OBJECT_KEY_OR_CLOSE);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    return JSONB_OK;
}

int
jsonb_pop_object(jsonb *builder, char buf[], size_t bufsize)
{
    switch (*builder->st_top) {
    case JSONB_OBJECT_KEY_OR_CLOSE:
    case JSONB_OBJECT_NEXT_KEY_OR_CLOSE:
        EXPECT_LEN(builder, 1, bufsize);
        BUFFER_COPY_CHAR(builder, '}', buf, bufsize);
        STACK_POP(builder);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    return JSONB_OK;
}

int
jsonb_push_key(
    jsonb *builder, const char key[], size_t len, char buf[], size_t bufsize)
{
    switch (*builder->st_top) {
    case JSONB_OBJECT_NEXT_KEY_OR_CLOSE:
        EXPECT_LEN(builder, len + 4, bufsize);
        BUFFER_COPY_CHAR(builder, ',', buf, bufsize);
    /* fall-through */
    case JSONB_OBJECT_KEY_OR_CLOSE:
        EXPECT_LEN(builder, len + 3, bufsize);
        BUFFER_COPY_CHAR(builder, '"', buf, bufsize);
        BUFFER_COPY(builder, key, len, buf, bufsize);
        BUFFER_COPY(builder, "\":", 2, buf, bufsize);
        STACK_HEAD(builder, JSONB_OBJECT_VALUE);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    return JSONB_OK;
}

int
jsonb_push_array(jsonb *builder, char buf[], size_t bufsize)
{
    switch (*builder->st_top) {
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        EXPECT_LEN(builder, 2, bufsize);
        BUFFER_COPY_CHAR(builder, ',', buf, bufsize);
        /* fall-through */
    case JSONB_OBJECT_VALUE:
        STACK_HEAD(builder, JSONB_OBJECT_NEXT_KEY_OR_CLOSE);
        /* fall-through */
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        EXPECT_LEN(builder, 1, bufsize);
        BUFFER_COPY_CHAR(builder, '[', buf, bufsize);
        STACK_PUSH(builder, JSONB_ARRAY_VALUE_OR_CLOSE);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    return JSONB_OK;
}

int
jsonb_pop_array(jsonb *builder, char buf[], size_t bufsize)
{
    switch (*builder->st_top) {
    case JSONB_ARRAY_VALUE_OR_CLOSE:
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        EXPECT_LEN(builder, 1, bufsize);
        BUFFER_COPY_CHAR(builder, ']', buf, bufsize);
        STACK_POP(builder);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    return JSONB_OK;
}

int
jsonb_push_token(
    jsonb *builder, const char token[], size_t len, char buf[], size_t bufsize)
{
    switch (*builder->st_top) {
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        EXPECT_LEN(builder, len, bufsize);
        BUFFER_COPY(builder, token, len, buf, bufsize);
        STACK_POP(builder);
        break;
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        EXPECT_LEN(builder, len + 1, bufsize);
        BUFFER_COPY_CHAR(builder, ',', buf, bufsize);
        /* fall-through */
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        EXPECT_LEN(builder, len, bufsize);
        BUFFER_COPY(builder, token, len, buf, bufsize);
        STACK_HEAD(builder, JSONB_ARRAY_NEXT_VALUE_OR_CLOSE);
        break;
    case JSONB_OBJECT_VALUE:
        EXPECT_LEN(builder, len, bufsize);
        BUFFER_COPY(builder, token, len, buf, bufsize);
        STACK_HEAD(builder, JSONB_OBJECT_NEXT_KEY_OR_CLOSE);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    return JSONB_OK;
}

int
jsonb_push_bool(jsonb *builder, int boolean, char buf[], size_t bufsize)
{
    int ret;
    if (boolean)
        ret = jsonb_push_token(builder, "true", 4, buf, bufsize);
    else
        ret = jsonb_push_token(builder, "false", 5, buf, bufsize);
    return ret;
}

int
jsonb_push_null(jsonb *builder, char buf[], size_t bufsize)
{
    return jsonb_push_token(builder, "null", 4, buf, bufsize);
}

int
jsonb_push_string(
    jsonb *builder, char str[], size_t len, char buf[], size_t bufsize)
{
    int ret;
    EXPECT_LEN(builder, len + 2, bufsize);
    BUFFER_COPY_CHAR(builder, '"', buf, bufsize);
    ret = jsonb_push_token(builder, str, len, buf, bufsize);
    BUFFER_COPY_CHAR(builder, '"', buf, bufsize);
    return ret;
}
