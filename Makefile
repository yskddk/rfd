PROGRAM := rfd

SRCS = main.c
OBJS = $(SRCS:%c=%o)

LIBS =
CFLAGS = -O2
LDFLAGS =

CC = gcc
RM = rm -f

%.o: %.c
	$(CC) -o $@ -c $(CFLAGS) $<

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $< $(LIBS) $(LDFLAGS)

.PHONY: all clean

all: $(PROGRAM)

clean:
	$(RM) *.o $(PROGRAM)
