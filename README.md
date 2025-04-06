# JSON-BUILD

json-build is a zero-allocation JSON serializer written in ANSI C. Its
tokenizer counterpart can be found at
[jsmn-find](https://github.com/lcsmuller/jsmn-find).

## Features

* compatible with C89
* no dependencies
* no dynamic memory allocation

## Usage

Download `json-build.h`, include it, done.

```c
#include "json-build.h"

...
jsonb b;
char buf[1024];

jsonb_init(&b);
jsonb_object(&b, buf, sizeof(buf));
{
    jsonb_key(&b, buf, sizeof(buf), "foo", strlen("foo"));
    jsonb_array(&b, buf, sizeof(buf));
    {
        jsonb_number(&b, buf, sizeof(buf), 1);
        jsonb_string(&b, buf, sizeof(buf), "hi", 2);
        jsonb_bool(&b, buf, sizeof(buf), 0);
        jsonb_null(&b, buf, sizeof(buf));
        jsonb_array_pop(&b, buf, sizeof(buf));
    }
    jsonb_object_pop(&b, buf, sizeof(buf));
}
printf("JSON: %s", buf); // JSON: {"foo":[1,"hi",false,null]}
```

## Automatic Buffer Management

For dynamic buffer management, json-build provides `_auto` counterparts for all serializer functions.
These functions will automatically reallocate the buffer when more space is needed:

```c
#include "json-build.h"
#include <stdlib.h>

...
jsonb b;
char *buf = malloc(64); // Initial small buffer
size_t bufsize = 64;

jsonb_init(&b);
jsonb_object_auto(&b, &buf, &bufsize); // Note: passing pointers to buffer and size
{
    jsonb_key_auto(&b, &buf, &bufsize, "foo", strlen("foo"));
    jsonb_array_auto(&b, &buf, &bufsize);
    {
        jsonb_number_auto(&b, &buf, &bufsize, 1);
        jsonb_string_auto(&b, &buf, &bufsize, "hi", 2);
        jsonb_bool_auto(&b, &buf, &bufsize, 0);
        jsonb_null_auto(&b, &buf, &bufsize);
        jsonb_array_pop_auto(&b, &buf, &bufsize);
    }
    jsonb_object_pop_auto(&b, &buf, &bufsize);
}
printf("JSON: %s (buffer size: %zu)\n", buf, bufsize);
free(buf);
```

**IMPORTANT**: Do not mix regular and `_auto` functions on the same buffer. Always use either the regular functions with a fixed-size buffer or the `_auto` functions with a dynamically allocated buffer throughout your code.

Since json-build is a single-header, header-only library, for more complex use
cases you might need to define additional macros. `#define JSONB_STATIC`hides all
json-build API symbols by making them static. Also, if you want to include `json-build.h`
for multiple C files, to avoid duplication of symbols you may define `JSONB_HEADER` macro.

```c
/* In every .c file that uses json-build include only declarations: */
#define JSONB_HEADER
#include "json-build.h"

/* Additionally, create one json-build.c file for json-build implementation: */
#include "json-build.h"
```

## API

* `jsonb_init()` - initialize a jsonb handle
* `jsonb_reset()` - reset the buffer's position tracker for streaming purposes
* `jsonb_object()` - push an object to the builder stack
* `jsonb_object_pop()` - pop an object from the builder stack
* `jsonb_key()` - push an object key field to the builder stack
* `jsonb_array()` - push an array to the builder stack
* `jsonb_array_pop()` - pop an array from the builder stack
* `jsonb_token()` - push a raw token to the builder stack
* `jsonb_bool()` - push a boolean token to the builder stack
* `jsonb_null()` - push a null token to the builder stack
* `jsonb_string()` - push a string token to the builder stack
* `jsonb_number()` - push a number token to the builder stack

The following are the possible return codes for the builder functions:
* `JSONB_OK` - operation was a success, user can proceed with the next operation
* `JSONB_END` - operation was a success, JSON is complete and expects no more operations
* `JSONB_ERROR_NOMEM` - buffer is not large enough, or `_auto` function couldn't reallocate
* `JSONB_ERROR_INPUT` - user action don't match expected next token
* `JSONB_ERROR_STACK` - user action would lead to out of boundaries access, increase `JSONB_MAX_DEPTH`!
* `JSONB_ERROR_OVERFLOW` - automatic buffer increase would lead to an overflow, only use with `_auto` functions

Its worth mentioning that all `JSONB_ERROR_` prefixed codes are negative.

If you get `JSONB_ERROR_NOMEM` you can either:
1. re-allocate a larger buffer and call the builder function once more
2. call `jsonb_reset()` to reset the buffer's position tracker and call the builder function once more (useful for streaming with a fixed sized buffer!)

## Other info

This software is distributed under [MIT license](www.opensource.org/licenses/mit-license.php),
so feel free to integrate it in your commercial products.
