TOP = ..
CC ?= gcc

EXES := test

CFLAGS += -Wall -Wextra -Wpedantic -g -I$(TOP)

all: $(EXES)

$(EXES): %: %.c
	$(CC) $(CFLAGS) $(XFLAGS) -o $@ $<

clean:
	rm -rf $(EXES) *.gcno *.gcda

.PHONY : all clean

