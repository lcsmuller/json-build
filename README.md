JSON-BUILD
==========

json-build is a zero-allocation JSON serializer made in C.

Features
--------

* compatible with C89
* no dependencies
* no dynamic memory allocation

Usage
-----

Download `json-build.c` and `json-build.h`.

```c
#include "json-build.h"

...
jsonb b;
char buf[1024] = { 0 };

jsonb_init(&b);
jsonb_push_object(&b, buf, sizeof(buf));
{
    jsonb_push_key(&b, "foo", strlen("foo"), buf, sizeof(buf));
    jsonb_push_array(&b, buf, sizeof(buf));
    {
        jsonb_push_token(&b, "1", 1, buf, sizeof(buf));
        jsonb_push_string(&b, "hi", 2, buf, sizeof(buf));
        jsonb_push_bool(&b, 0, buf, sizeof(buf));
        jsonb_push_null(&b, buf, sizeof(buf));
    }
    json_pop_array(&b, buf, sizeof(buf));
}
jsonb_pop_object(&b, buf, sizeof(buf));

printf("JSON: %s", buf); // JSON: {"foo":[1,"hi",false,null]}
```

API
---

* `jsonb_init()` - initialize a jsonb handle
* `jsonb_push_object()` - push an object to the builder stack
* `jsonb_pop_object()` - pop an object from the builder stack
* `jsonb_push_key()` - push an object key field to the builder stack
* `jsonb_push_array()` - push an array to the builder stack
* `jsonb_pop_array()` - pop an array from the builder stack
* `jsonb_push_token()` - push a raw token to the builder stack
* `jsonb_push_bool()` - push a boolean token to the builder stack
* `jsonb_push_null()` - push a null token to the builder stack
* `jsonb_push_string()` - push a string token to the builder stack

The following are the possible return values for the builder functions:
* `JSONB_OK` - everything went as expected, user can proceed
* `JSONB_ERROR_NOMEM` - buffer is not large enough
* `JSONB_ERROR_INPUT` - user action don't match expect next token

If you get `JSONB_ERROR_NOMEM` you can re-allocate a larger buffer and call
the builder function once more.
