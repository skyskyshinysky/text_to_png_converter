CC=gcc
BUILT_SOURCES=pnglibconf.h
SOURCES=png.c pngerror.c pngget.c pngmem.c pngpread.c pngread.c pngrio.c pngrtran.c pngrutil.c pngset.c pngtrans.c \
	pngwio.c pngwrite.c pngwtran.c pngwutil.c
CFLAGS=-c -Wall -O2 -D_LARGEFILE64_SOURCE=1 -DHAVE_HIDDEN 
SFLAGS=-O3 -fPIC -D_LARGEFILE64_SOURCE=1 -DHAVE_HIDDEN 
INCLUDE=-I../zlib/
OBJECTS=$(SOURCES:.c=.o)
PIC_OBJECTS=$(SOURCES:.c=.lo)
STATICLIB=libpng16.a
SHAREDLIBV=libpng16.so

LDSHARED=gcc -shared -Wl,-soname,libpng16.so -L$(pwd)/zlib -lz -lm

HEADERS=pnglibconf.h png.h pngconf.h

all: static shared

shared: $(SOURCES) $(SHAREDLIBV) 

static: $(SOURCES) $(STATICLIB)

$(STATICLIB): $(OBJECTS)
	ar rc $@ $(OBJECTS)
	-@ ($(RANLIB) $@ || true) >/dev/null 2>&1

%.lo : %.c
	$(CC) $(SFLAGS) -DPIC -c $(INCLUDE) $< -o $@
%.o : %.c
	$(CC) $(CFLAGS) -DPIC -c $(INCLUDE) $< -o $@

$(SHAREDLIBV): $(PIC_OBJECTS)
	$(LDSHARED) $(SFLAGS) -o $@ $(PIC_OBJECTS) $(LDSHAREDLIBC)

clean:
	rm -rf *.o *.lo libpng16.a libpng16.so
