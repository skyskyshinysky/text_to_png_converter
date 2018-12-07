CC=gcc
SOURCES=builds/unix/ftsystem.c src/base/ftdebug.c src/base/ftbase.c src/base/ftbbox.c \
src/base/ftbdf.c src/base/ftbitmap.c src/base/ftcid.c src/base/ftinit.c src/base/ftfntfmt.c src/base/ftfstype.c \
src/base/ftgasp.c src/base/ftglyph.c src/base/ftgxval.c src/base/ftlcdfil.c src/base/ftmm.c src/base/ \
src/base/ftotval.c src/base/ftpatent.c src/base/ftpfr.c src/base/ftstroke.c src/base/ftsynth.c src/base/fttype1.c \
src/base/ftwinfnt.c src/truetype/truetype.c src/type1/type1.c src/cff/cff.c src/cid/type1cid.c src/pfr/pfr.c \
src/type42/type42.c src/winfonts/winfnt.c src/pcf/pcf.c src/bdf/bdf.c src/sfnt/sfnt.c src/autofit/autofit.c \
src/pshinter/pshinter.c src/raster/raster.c src/smooth/smooth.c src/cache/ftcache.c src/gzip/ftgzip.c src/lzw/ftlzw.c \
src/bzip2/ftbzip2.c src/psaux/psaux.c src/psnames/psnames.c 
OBJECTS=$(SOURCES:.c=.o)
PIC_OBJECTS=$(SOURCES:.c=.lo)
STATICLIB=libfreetype.a
SHAREDLIBV=libfreetype.so.6.12.6

CFLAGS=-c -Wall -O2 -D_LARGEFILE64_SOURCE=1 -DHAVE_HIDDEN -DFT_CONFIG_OPTION_USE_PNG \
 "-DFT_CONFIG_CONFIG_H=<ftconfig.h>" -DFT2_BUILD_LIBRARY -DHAVE_FCNTL_H "-DFT_CONFIG_MODULES_H=<ftmodule.h>"
SFLAGS=-fPIC -DHAVE_FCNTL_H 

INCLUDE=-I./builds/unix -I./include \
-I../zlib/ -I./include/ \
-I./include/freetype/config/
BUILDDIR=build
LDSHARED=gcc -shared -Wl,-soname -Wl,libfreetype.so.6 -L../zlib -lz -lm 

all: static shared

build_dir:
	mkdir -pv $(BUILDDIR) 

shared: build_dir $(SOURCES) $(SHAREDLIBV)

$(SHAREDLIBV): $(PIC_OBJECTS)
	$(LDSHARED) $(SFLAGS) -o $@ $(BUILDDIR)/*.lo $(LDSHAREDLIBC)

%.lo : %.c
	$(CC) $(INCLUDE) "-DFT_CONFIG_CONFIG_H=<ftconfig.h>" -DFT2_BUILD_LIBRARY "-DFT_CONFIG_MODULES_H=<ftmodule.h>" $(SFLAGS) -DPIC -c $< -o $@
	mv -f $@ $(BUILDDIR)

static: build_dir $(SOURCES) $(STATICLIB)

$(STATICLIB): $(OBJECTS)
	ar cr $@ $(BUILDDIR)/*.o
	-@ ($(RANLIB) $@ || true) >/dev/null 2>&1

%.o : %.c
	$(CC) $(INCLUDE) $(CFLAGS) $< -o $@
	mv -f $@ $(BUILDDIR)
clean:
	rm -rf build libfreetype.a libfreetype.so.6.12.6
