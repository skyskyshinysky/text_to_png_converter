
CC = gcc
CFLAGS += -Wall -g -pedantic -Wextra -Wstrict-aliasing=2 -Wformat-security \
-Wc++-compat -Wbad-function-cast -Wstrict-overflow=5 -Wfloat-equal \
-Wformat-extra-args -Wshadow -Winit-self -Wconversion
LDFLAGS += -L/usr/local/lib -lfreetype -lpng
LIBS=$(STATIC_LIB_DIR)/* -lm
SHARED_LIBS=$(SHARED_LIB_DIR)/*

SOURCES=option.c app.c 
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_SHARED=$(SOURCES:.c=.lo)
INCLUDE=-I../lib/freetype-2.9/include -I../lib/freetype-2.9/include/freetype/config 

.PHONY: help all clean

all: $(SOURCES) static shared

shared: $(OBJECTS_SHARED)
	$(CC) $(OBJECTS_SHARED) $(SHARED_LIBS) -o app_shared -D DYNAMIC_APP

static: $(OBJECTS)
	$(CC) -static $(OBJECTS) $(LIBS) -o app_static 

%.lo : %.c
	$(CC) -DPIC -c $< $(INCLUDE) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< $(INCLUDE)

clean:
	rm -rf ./*.o ./*.lo  ./app_static ./app.output ./app_shared
