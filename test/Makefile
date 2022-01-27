TOP = ..
CC ?= gcc

SRC := $(TOP)/json-build.c

EXES := test

CFLAGS += -Wall -Wextra -Wpedantic -std=c89 -g -I$(TOP)

all: $(EXES)

$(EXES): %: %.c $(SRC)
	$(CC) $(CFLAGS) $(XFLAGS) -o $@ $^

clean:
	rm -rf $(EXES) *.gcno *.gcda

.PHONY : all clean

