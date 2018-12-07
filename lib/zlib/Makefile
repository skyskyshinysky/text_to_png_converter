CC=gcc
CFLAGS=-c -Wall -O2 -D_LARGEFILE64_SOURCE=1 -DHAVE_HIDDEN 
SFLAGS=-O3 -fPIC -D_LARGEFILE64_SOURCE=1 -DHAVE_HIDDEN 

SOURCES=adler32.c crc32.c deflate.c infback.c inffast.c inflate.c inftrees.c trees.c zutil.c \
		compress.c uncompr.c gzclose.c gzlib.c gzread.c gzwrite.c

OBJECTS=$(SOURCES:.c=.o)
PIC_OBJECTS=adler32.lo crc32.lo deflate.lo infback.lo inffast.lo inflate.lo inftrees.lo trees.lo zutil.lo \
		compress.lo uncompr.lo gzclose.lo gzlib.lo gzread.lo gzwrite.lo
LDSHARED=gcc -shared -Wl,-soname,libz.so.1,--version-script,zlib.map
LDSHAREDLIBC=-lc

SHAREDLIBV=libz.so.1.2.11
STATICLIB=libz.a


all: static shared 

shared: $(SOURCES) $(SHAREDLIBV)
static: $(SOURCES) $(STATICLIB)

$(STATICLIB): $(OBJECTS)
	ar rc $@ $(OBJECTS)
	-@ ($(RANLIB) $@ || true) >/dev/null 2>&1

%.lo : %.c
	$(CC) $(SFLAGS) -DPIC -c $< -o $@

$(SHAREDLIBV): $(PIC_OBJECTS)
	$(LDSHARED) $(SFLAGS) -o $@ $(PIC_OBJECTS) $(LDSHAREDLIBC)

clean:
	rm -rf *.o *.lo libz.so.1.2.11 libz.a 
