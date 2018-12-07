CC = gcc
CFLAGS += -Wall -g -pedantic -Wextra -Wstrict-aliasing=2 -Wformat-security \
-Wc++-compat -Wbad-function-cast -Wstrict-overflow=5 -Wfloat-equal \
-Wformat-extra-args -Wshadow -Winit-self -Wconversion
LDFLAGS += -L/usr/local/lib -lfreetype -lpng
LIBS=$(if $(STATIC_LIB_DIR),$(STATIC_LIB_DIR)/*,) -lm
SHARED_LIBS=-ldl

SOURCES=option.c app.c 
OBJECTS=$(SOURCES:.c=.o)
OBJECTS_SHARED=$(SOURCES:.c=.lo)
INCLUDE=-I../lib/freetype-2.9/include -I../lib/freetype-2.9/include/freetype/config 

.PHONY: help all clean

all: $(SOURCES) static shared

shared: $(OBJECTS_SHARED)
	$(CC) $(OBJECTS_SHARED) $(SHARED_LIBS) -o app_shared 

static: $(OBJECTS)
	$(CC) -static $(OBJECTS) $(LIBS) -o app_static 

option.lo : option.c
	$(CC) -g -DPIC -c $< -o $@

app.lo : app.c
	$(CC) $(CFLAGS) -DPIC -c $< $(INCLUDE) -o $@ -DDYNAMIC_APP -DLIBZ_PATH=\"$(SHARED_LIBZ_PATH)\" \
	-DLIBPNG_PATH=\"$(SHARED_LIBPNG_PATH)\" -DLIBFREETYPE_PATH=\"$(SHARED_LIBFREETYPE_PATH)\"

app.o: app.c
	$(CC) $(CFLAGS) -c $< $(INCLUDE)

option.o: option.c
	$(CC) $(CFLAGS) -c $< 

clean:
	rm -rf ./*.o ./*.lo  ./app_static ./app.output ./app_shared
