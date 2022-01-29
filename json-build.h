#ifndef JSON_BUILD_H
#define JSON_BUILD_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef JSONB_STATIC
#define JSONB_API static
#else
#define JSONB_API extern
#endif

/* if necessary should be increased to avoid segfault */
#ifndef JSONB_MAX_DEPTH
#define JSONB_MAX_DEPTH 512
#endif

/** @brief json-builder error codes */
enum jsonb_err {
    /** not enough tokens were provided */
    JSONB_ERROR_NOMEM = -1,
    /** token doesn't match expected value */
    JSONB_ERROR_INPUT = -2
};

/** @brief json-builder serializing state */
enum jsonb_state {
    JSONB_INIT = 0,
    JSONB_ARRAY_OR_OBJECT_OR_VALUE = JSONB_INIT,
    JSONB_OBJECT_KEY_OR_CLOSE,
    JSONB_OBJECT_VALUE,
    JSONB_OBJECT_NEXT_KEY_OR_CLOSE,
    JSONB_ARRAY_VALUE_OR_CLOSE,
    JSONB_ARRAY_NEXT_VALUE_OR_CLOSE,
    JSONB_ERROR
};

/** @brief Handle for building a JSON string */
typedef struct jsonb {
    /** expected next input */
    enum jsonb_state st_stack[JSONB_MAX_DEPTH + 1];
    /** pointer to stack top */
    enum jsonb_state *st_top;
    /** offset in the JSON buffer (current length) */
    size_t pos;
} jsonb;

/**
 * @brief Initialize a jsonb handle
 *
 * @param builder the handle to be initialized
 */
JSONB_API void jsonb_init(jsonb *builder);

/**
 * @brief Push an object to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return the amount of bytes written or a negative @ref jsonb_err value in
 *      case of error
 */
JSONB_API long jsonb_push_object(jsonb *builder, char buf[], size_t bufsize);

/**
 * @brief Pop an object from the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return the amount of bytes written or a negative @ref jsonb_err value in
 *      case of error
 */
JSONB_API long jsonb_pop_object(jsonb *builder, char buf[], size_t bufsize);

/**
 * @brief Push a key to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param key the key to be inserted
 * @param len the key length
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return the amount of bytes written or a negative @ref jsonb_err value in
 *      case of error
 */
JSONB_API long jsonb_push_key(
    jsonb *builder, const char key[], size_t len, char buf[], size_t bufsize);

/**
 * @brief Push an array to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return the amount of bytes written or a negative @ref jsonb_err value in
 *      case of error
 */
JSONB_API long jsonb_push_array(jsonb *builder, char buf[], size_t bufsize);

/**
 * @brief Pop an array from the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return the amount of bytes written or a negative @ref jsonb_err value in
 *      case of error
 */
JSONB_API long jsonb_pop_array(jsonb *builder, char buf[], size_t bufsize);

/**
 * @brief Push a raw JSON token to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param token the token to be inserted
 * @param len the token length
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return the amount of bytes written or a negative @ref jsonb_err value in
 *      case of error
 */
JSONB_API long jsonb_push_token(jsonb *builder,
                                const char token[],
                                size_t len,
                                char buf[],
                                size_t bufsize);

/**
 * @brief Push a boolean token to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param boolean the boolean to be inserted
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return the amount of bytes written or a negative @ref jsonb_err value in
 *      case of error
 */
JSONB_API long jsonb_push_bool(jsonb *builder,
                               int boolean,
                               char buf[],
                               size_t bufsize);

/**
 * @brief Push a null token to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return the amount of bytes written or a negative @ref jsonb_err value in
 *      case of error
 */
JSONB_API long jsonb_push_null(jsonb *builder, char buf[], size_t bufsize);

/**
 * @brief Push a string token to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param str the string to be inserted
 * @param len the string length
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return the amount of bytes written or a negative @ref jsonb_err value in
 *      case of error
 */
JSONB_API long jsonb_push_string(
    jsonb *builder, char str[], size_t len, char buf[], size_t bufsize);

/**
 * @brief Push a number token to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param number the number to be inserted
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return the amount of bytes written or a negative @ref jsonb_err value in
 *      case of error
 */
JSONB_API long jsonb_push_number(jsonb *builder,
                                 double number,
                                 char buf[],
                                 size_t bufsize);

#ifndef JSONB_HEADER
#include <stdio.h>
#ifndef JSONB_DEBUG
#define TRACE(prev, next) next
#define DECORATOR(a)
#else
static const char *
_jsonb_eval_state(enum jsonb_state state)
{
    switch (state) {
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        return "array or object or value";
    case JSONB_OBJECT_KEY_OR_CLOSE:
        return "object key or close";
    case JSONB_OBJECT_NEXT_KEY_OR_CLOSE:
        return "object next key or close";
    case JSONB_OBJECT_VALUE:
        return "object value";
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        return "array value or close";
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        return "array next value or close";
    case JSONB_ERROR:
        return "error";
    default:
        return "unknown";
    }
}
#define TRACE(prev, next)                                                     \
    do {                                                                      \
        enum jsonb_state _prev = prev, _next = next;                          \
        fprintf(stderr, "%s():L%d | %s -> %s\n", __func__, __LINE__,          \
                _jsonb_eval_state(_prev), _jsonb_eval_state(_next));          \
    } while (0)
#define DECORATOR(d) d
#endif /* JSONB_DEBUG */

#define STACK_PUSH(b, state) TRACE(*(b)->st_top, *++(b)->st_top = (state))
#define STACK_POP(b)         TRACE(*(b)->st_top, DECORATOR(*)--(b)->st_top)
#define STACK_HEAD(b, state) TRACE(*(b)->st_top, *(b)->st_top = (state))

#define BUFFER_COPY_CHAR(b, c, _pos, buf, bufsize)                            \
    do {                                                                      \
        if ((b)->pos + (_pos) + 1 > (bufsize)) {                              \
            return JSONB_ERROR_NOMEM;                                         \
        }                                                                     \
        (buf)[(b)->pos + (_pos)++] = (c);                                     \
    } while (0)
#define BUFFER_COPY(b, value, len, _pos, buf, bufsize)                        \
    do {                                                                      \
        size_t i;                                                             \
        if ((b)->pos + (_pos) + (len) > (bufsize)) {                          \
            return JSONB_ERROR_NOMEM;                                         \
        }                                                                     \
        for (i = 0; i < (len); ++i)                                           \
            (buf)[(b)->pos + (_pos) + i] = (value)[i];                        \
        (_pos) += (len);                                                      \
    } while (0)

void
jsonb_init(jsonb *builder)
{
    static jsonb empty_builder;
    *builder = empty_builder;
    builder->st_top = builder->st_stack;
}

long
jsonb_push_object(jsonb *builder, char buf[], size_t bufsize)
{
    size_t pos = 0;
    switch (*builder->st_top) {
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        BUFFER_COPY_CHAR(builder, ',', pos, buf, bufsize);
        /* fall-through */
    case JSONB_OBJECT_VALUE:
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        if (*builder->st_top <= JSONB_OBJECT_NEXT_KEY_OR_CLOSE)
            STACK_HEAD(builder, JSONB_OBJECT_NEXT_KEY_OR_CLOSE);
        else if (*builder->st_top <= JSONB_ARRAY_NEXT_VALUE_OR_CLOSE)
            STACK_HEAD(builder, JSONB_ARRAY_NEXT_VALUE_OR_CLOSE);
        /* fall-through */
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        BUFFER_COPY_CHAR(builder, '{', pos, buf, bufsize);
        STACK_PUSH(builder, JSONB_OBJECT_KEY_OR_CLOSE);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    builder->pos += pos;
    return pos;
}

long
jsonb_pop_object(jsonb *builder, char buf[], size_t bufsize)
{
    size_t pos = 0;
    switch (*builder->st_top) {
    case JSONB_OBJECT_KEY_OR_CLOSE:
    case JSONB_OBJECT_NEXT_KEY_OR_CLOSE:
        BUFFER_COPY_CHAR(builder, '}', pos, buf, bufsize);
        STACK_POP(builder);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    builder->pos += pos;
    return pos;
}

long
jsonb_push_key(
    jsonb *builder, const char key[], size_t len, char buf[], size_t bufsize)
{
    size_t pos = 0;
    switch (*builder->st_top) {
    case JSONB_OBJECT_NEXT_KEY_OR_CLOSE:
        BUFFER_COPY_CHAR(builder, ',', pos, buf, bufsize);
    /* fall-through */
    case JSONB_OBJECT_KEY_OR_CLOSE:
        BUFFER_COPY_CHAR(builder, '"', pos, buf, bufsize);
        BUFFER_COPY(builder, key, len, pos, buf, bufsize);
        BUFFER_COPY(builder, "\":", 2, pos, buf, bufsize);
        STACK_HEAD(builder, JSONB_OBJECT_VALUE);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    builder->pos += pos;
    return pos;
}

long
jsonb_push_array(jsonb *builder, char buf[], size_t bufsize)
{
    size_t pos = 0;
    switch (*builder->st_top) {
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        BUFFER_COPY_CHAR(builder, ',', pos, buf, bufsize);
        /* fall-through */
    case JSONB_OBJECT_VALUE:
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        if (*builder->st_top <= JSONB_OBJECT_NEXT_KEY_OR_CLOSE)
            STACK_HEAD(builder, JSONB_OBJECT_NEXT_KEY_OR_CLOSE);
        else if (*builder->st_top <= JSONB_ARRAY_NEXT_VALUE_OR_CLOSE)
            STACK_HEAD(builder, JSONB_ARRAY_NEXT_VALUE_OR_CLOSE);
        /* fall-through */
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        BUFFER_COPY_CHAR(builder, '[', pos, buf, bufsize);
        STACK_PUSH(builder, JSONB_ARRAY_VALUE_OR_CLOSE);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    builder->pos += pos;
    return pos;
}

long
jsonb_pop_array(jsonb *builder, char buf[], size_t bufsize)
{
    size_t pos = 0;
    switch (*builder->st_top) {
    case JSONB_ARRAY_VALUE_OR_CLOSE:
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        BUFFER_COPY_CHAR(builder, ']', pos, buf, bufsize);
        STACK_POP(builder);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    builder->pos += pos;
    return pos;
}

long
jsonb_push_token(
    jsonb *builder, const char token[], size_t len, char buf[], size_t bufsize)
{
    size_t pos = 0;
    switch (*builder->st_top) {
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        BUFFER_COPY(builder, token, len, pos, buf, bufsize);
        STACK_POP(builder);
        break;
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        BUFFER_COPY_CHAR(builder, ',', pos, buf, bufsize);
        /* fall-through */
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        BUFFER_COPY(builder, token, len, pos, buf, bufsize);
        STACK_HEAD(builder, JSONB_ARRAY_NEXT_VALUE_OR_CLOSE);
        break;
    case JSONB_OBJECT_VALUE:
        BUFFER_COPY(builder, token, len, pos, buf, bufsize);
        STACK_HEAD(builder, JSONB_OBJECT_NEXT_KEY_OR_CLOSE);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    builder->pos += pos;
    return pos;
}

long
jsonb_push_bool(jsonb *builder, int boolean, char buf[], size_t bufsize)
{
    if (boolean) return jsonb_push_token(builder, "true", 4, buf, bufsize);
    return jsonb_push_token(builder, "false", 5, buf, bufsize);
}

long
jsonb_push_null(jsonb *builder, char buf[], size_t bufsize)
{
    return jsonb_push_token(builder, "null", 4, buf, bufsize);
}

static long
_jsonb_escape(size_t pos, char str[], size_t len, char buf[], size_t bufsize)
{
    char *esc_tok = NULL, _esc_tok[8] = "\\u00";
    char *esc_buf = NULL;
    int extra_bytes = 0;
    size_t i;

/* 1st iteration, esc_buf is NULL and count extra_bytes needed for escaping
 * 2st iteration, esc_buf is not NULL, and does escaping.  */
second_iter:
    for (i = 0; i < len; ++i) {
        unsigned char c = str[i];
        esc_tok = NULL;
        switch (c) {
        case 0x22:
            esc_tok = "\\\"";
            break;
        case 0x5C:
            esc_tok = "\\\\";
            break;
        case '\b':
            esc_tok = "\\b";
            break;
        case '\f':
            esc_tok = "\\f";
            break;
        case '\n':
            esc_tok = "\\n";
            break;
        case '\r':
            esc_tok = "\\r";
            break;
        case '\t':
            esc_tok = "\\t";
            break;
        default:
            if (c <= 0x1F) {
                static const char tohex[] = "0123456789abcdef";
                _esc_tok[4] = tohex[c >> 4];
                _esc_tok[5] = tohex[c & 0xF];
                _esc_tok[6] = 0;
                esc_tok = _esc_tok;
            }
        }

        if (esc_tok) {
            int j;
            for (j = 0; esc_tok[j]; j++) {
                if (!esc_buf) /* count how many extra bytes are needed */
                    continue;
                *esc_buf = esc_tok[j];
                ++esc_buf;
            }
            extra_bytes += j - 1;
        }
        else if (esc_buf) {
            *esc_buf = c;
            ++esc_buf;
        }
    }

    if (pos + len + extra_bytes > bufsize) {
        return JSONB_ERROR_NOMEM;
    }
    if (esc_buf) {
        return len + extra_bytes;
    }
    if (!extra_bytes) {
        size_t j;
        for (j = 0; j < len; ++j)
            buf[pos + j] = str[j];
        return len;
    }

    esc_buf = buf + pos;
    extra_bytes = 0;

    goto second_iter;
}

long
jsonb_push_string(
    jsonb *builder, char str[], size_t len, char buf[], size_t bufsize)
{
    size_t pos = 0;
    long ret;
    switch (*builder->st_top) {
    case JSONB_ARRAY_OR_OBJECT_OR_VALUE:
        BUFFER_COPY_CHAR(builder, '"', pos, buf, bufsize);
        ret = _jsonb_escape(pos + builder->pos, str, len, buf, bufsize);
        if (ret < 0) return ret;
        pos += ret;
        BUFFER_COPY_CHAR(builder, '"', pos, buf, bufsize);
        STACK_POP(builder);
        break;
    case JSONB_ARRAY_NEXT_VALUE_OR_CLOSE:
        BUFFER_COPY_CHAR(builder, ',', pos, buf, bufsize);
        /* fall-through */
    case JSONB_ARRAY_VALUE_OR_CLOSE:
        BUFFER_COPY_CHAR(builder, '"', pos, buf, bufsize);
        ret = _jsonb_escape(pos + builder->pos, str, len, buf, bufsize);
        if (ret < 0) return ret;
        pos += ret;
        BUFFER_COPY_CHAR(builder, '"', pos, buf, bufsize);
        STACK_HEAD(builder, JSONB_ARRAY_NEXT_VALUE_OR_CLOSE);
        break;
    case JSONB_OBJECT_VALUE:
        BUFFER_COPY_CHAR(builder, '"', pos, buf, bufsize);
        ret = _jsonb_escape(pos + builder->pos, str, len, buf, bufsize);
        if (ret < 0) return ret;
        pos += ret;
        BUFFER_COPY_CHAR(builder, '"', pos, buf, bufsize);
        STACK_HEAD(builder, JSONB_OBJECT_NEXT_KEY_OR_CLOSE);
        break;
    default:
        STACK_HEAD(builder, JSONB_ERROR);
        return JSONB_ERROR_INPUT;
    }
    builder->pos += pos;
    return pos;
}

long
jsonb_push_number(jsonb *builder, double number, char buf[], size_t bufsize)
{
    char token[32];
    long ret = sprintf(token, "%.17G", number);
    if (ret < 0) return JSONB_ERROR_INPUT;
    return jsonb_push_token(builder, token, ret, buf, bufsize);
}
#endif /* JSONB_HEADER */

#ifdef __cplusplus
}
#endif

#endif /* JSON_BUILD_H */
