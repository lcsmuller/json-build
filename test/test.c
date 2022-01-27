#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

#include "json-build.h"

int
main(void)
{
    char buf[5] = { 0 };
    jsonb b;

    jsonb_init(&b);

    jsonb_push_object(&b, buf, sizeof(buf));
    {
        jsonb_push_key(&b, "foo", 3, buf, sizeof(buf));
        jsonb_push_value(&b, "10", 2, buf, sizeof(buf));
        jsonb_push_key(&b, "bar", 3, buf, sizeof(buf));
        jsonb_push_value(&b, "20", 2, buf, sizeof(buf));
        jsonb_push_key(&b, "baz", 3, buf, sizeof(buf));
        jsonb_push_array(&b, buf, sizeof(buf));
        {
            jsonb_push_value(&b, "10", 2, buf, sizeof(buf));
        }
        jsonb_pop_array(&b, buf, sizeof(buf));
        jsonb_push_key(&b, "obj", 3, buf, sizeof(buf));
        jsonb_push_object(&b, buf, sizeof(buf));
        {
            jsonb_push_key(&b, "a", 1, buf, sizeof(buf));
            jsonb_push_value(&b, "1", 1, buf, sizeof(buf));
            jsonb_push_key(&b, "b", 1, buf, sizeof(buf));
            jsonb_push_value(&b, "2", 1, buf, sizeof(buf));
            jsonb_push_key(&b, "c", 1, buf, sizeof(buf));
            jsonb_push_value(&b, "3", 1, buf, sizeof(buf));
            jsonb_push_key(&b, "nest", 4, buf, sizeof(buf));
            jsonb_push_array(&b, buf, sizeof(buf));
            {
                jsonb_push_array(&b, buf, sizeof(buf));
                {
                    jsonb_push_array(&b, buf, sizeof(buf));
                    {
                        jsonb_push_value(&b, "true", 4, buf, sizeof(buf));
                    }
                    jsonb_pop_array(&b, buf, sizeof(buf));
                }
                jsonb_pop_array(&b, buf, sizeof(buf));
            }
            jsonb_pop_array(&b, buf, sizeof(buf));
        }
        jsonb_pop_object(&b, buf, sizeof(buf));
    }
    jsonb_pop_object(&b, buf, sizeof(buf));

    fprintf(stderr, "%s\n", buf);

    return EXIT_SUCCESS;
}
