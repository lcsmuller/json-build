TOP = ..
CC ?= gcc

EXES := test fuzz

CFLAGS += -Wall -Wextra -Wpedantic -g -I$(TOP) -std=c89

all: $(EXES)

$(EXES): %: %.c
	$(CC) $(CFLAGS) $(XFLAGS) -o $@ $<

clean:
	rm -rf $(EXES) *.gcno *.gcda

.PHONY : all clean

