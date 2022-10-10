CC=gcc # compilador
CFLAGS=-c -g -Wall -std=c17 #flags para el compilador
LDFLAGS= #flags para enlazador

SOURCES=my_lib.c test1.c test2a.c test2b.c
LIBRARIES=my_lib.o 
INCLUDES=my_lib.h
PROGRAMS=test1 test2a test2b

OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

#$(PROGRAMS): $(LIBRARIES) $(INCLUDES)
#	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

test1: test1.o $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $< -o $@

test2a: test2a.o $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $< -o $@
	
test2b: test2b.o $(LIBRARIES) $(INCLUDES)
	$(CC) $(LDFLAGS) $(LIBRARIES) $< -o $@

%.o: %.c $(INCLUDES) 
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o $(PROGRAMS)
