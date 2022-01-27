TOP = ..
CC ?= gcc

EXES := test

CFLAGS += -Wall -Wextra -Wpedantic -std=c89 -g -I$(TOP)

all: $(EXES)

$(EXES): %: %.c
	$(CC) $(CFLAGS) $(XFLAGS) -o $@ $<

clean:
	rm -rf $(EXES) *.gcno *.gcda

.PHONY : all clean

