# Makefile for h264 decoder module

CC = clang++ # CPP compiler
CFLAGS = -Wall -Wextra -O2 -g # CPP flags
LDFLAGS = -fPIC -shared  # linking flags
RM = rm -f  # rm command
TARGET_LIB = libh264dec.so # target lib

SRCS = parser.cpp h264_decoder.cpp # source files
OBJS = $(SRCS:.cpp=.o)

.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CC) ${LDFLAGS} -o $@ $^

$(SRCS:.c):%.c
	$(CC) $(CFLAGS) -MM $< >$@

include $(SRCS:.c)

clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c)
