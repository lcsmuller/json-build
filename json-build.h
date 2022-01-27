#ifndef JSON_BUILD_H
#define JSON_BUILD_H

#ifndef JSONB_MAX_DEPTH
#define JSONB_MAX_DEPTH 512
#endif

enum jsonb_err {
    /** everything went as expected */
    JSONB_OK = 0,
    /** not enough tokens were provided */
    JSONB_ERROR_NOMEM = -1,
    /** token doesn't match expected value */
    JSONB_ERROR_INPUT = -2
};

enum jsonb_state {
    JSONB_INIT = 0,
    JSONB_ARRAY_OR_OBJECT_OR_VALUE = JSONB_INIT,
    JSONB_OBJECT_KEY_OR_CLOSE,
    JSONB_OBJECT_NEXT_KEY_OR_CLOSE,
    JSONB_OBJECT_VALUE,
    JSONB_ARRAY_VALUE_OR_CLOSE,
    JSONB_ARRAY_NEXT_VALUE_OR_CLOSE,
    JSONB_ERROR,
    JSONB_DONE
};

/** @brief Handle for building a JSON string */
typedef struct jsonb {
    /** expected next input */
    enum jsonb_state st_stack[JSONB_MAX_DEPTH];
    /** pointer to stack top */
    enum jsonb_state *st_top;
    /** offset in the JSON buffer (current length) */
    unsigned int pos;
    /** next token to allocate */
    unsigned int toknext;
} jsonb;

/**
 * @brief Initialize a jsonb handle
 *
 * @param builder the handle to be initialized
 */
void jsonb_init(jsonb *builder);

/**
 * @brief Push an object to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return a negative @ref jsonb_err value in case of error, @ref JSONB_OK otherwise
 */
int jsonb_push_object(jsonb *builder, char buf[], size_t bufsize);

/**
 * @brief Pop an object from the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return a negative @ref jsonb_err value in case of error, @ref JSONB_OK otherwise
 */
int jsonb_pop_object(jsonb *builder, char buf[], size_t bufsize);

/**
 * @brief Push a key to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param key the key to be inserted
 * @param len the key length
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return a negative @ref jsonb_err value in case of error, @ref JSONB_OK otherwise
 */
int jsonb_push_key(
    jsonb *builder, const char key[], size_t len, char buf[], size_t bufsize);

/**
 * @brief Push an array to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return a negative @ref jsonb_err value in case of error, @ref JSONB_OK otherwise
 */
int jsonb_push_array(jsonb *builder, char buf[], size_t bufsize);

/**
 * @brief Pop an array from the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return a negative @ref jsonb_err value in case of error, @ref JSONB_OK otherwise
 */
int jsonb_pop_array(jsonb *builder, char buf[], size_t bufsize);

/**
 * @brief Push a value to the builder
 *
 * @param builder the builder initialized with jsonb_init()
 * @param value the value to be inserted
 * @param len the value length
 * @param buf the JSON buffer
 * @param bufsize the JSON buffer size
 * @return a negative @ref jsonb_err value in case of error, @ref JSONB_OK otherwise
 */
int jsonb_push_value(jsonb *builder,
                     const char value[],
                     size_t len,
                     char buf[],
                     size_t bufsize);

#endif /* JSON_BUILD_H */
