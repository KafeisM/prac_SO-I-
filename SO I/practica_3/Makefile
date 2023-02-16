CC=gcc
CFLAGS=-c -g -Wall -std=c17
LDFLAGS=-pthread

SOURCES=my_lib.c stack_counters.c reader.c
LIBRARIES=my_lib.o
INCLUDES=my_lib.h
PROGRAMS= stack_counters reader
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

#$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
#	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@


stack_counters: stack_counters.o $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $< -o $@

reader: reader.o $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $< -o $@

%.o: %.c $(INCLUDES) 
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)
 
