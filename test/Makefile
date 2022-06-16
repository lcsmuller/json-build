TOP = ..
CC ?= gcc

EXES = test fuzz

CFLAGS += -Wall -Wextra -Wpedantic -g -I$(TOP) -std=c89

all: $(EXES)

clean:
	$(RM) $(EXES)

.PHONY : all clean

