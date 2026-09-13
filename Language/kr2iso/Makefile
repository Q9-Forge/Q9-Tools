#===============================================================================
# Makefile for kr2iso - K&R to ISO C Format Converter
# Builds with QCC compiler for OS-9 system
#===============================================================================

CC ?= qcc
CFLAGS ?= -O2 -Wall
TARGET = kr2iso

SRCS = kr2iso.c
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
