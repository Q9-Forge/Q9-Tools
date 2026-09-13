#===============================================================================
# Makefile for iso2kr - ISO C to K&R Format Converter
# Builds with QCC compiler for OS-9 system
#===============================================================================

CC ?= qcc
CFLAGS ?= -O2 -Wall
TARGET = iso2kr

SRCS = iso2kr.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean install

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET) *.o

install: $(TARGET)
	cp $(TARGET) /cmds/$(TARGET)
	chmod +x /cmds/$(TARGET)

#===============================================================================
# EOF
