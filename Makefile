lib_dir=lib
export STATIC_LIB_DIR=$(shell realpath $(addprefix lib/,static))
export SHARED_LIB_DIR=$(shell realpath $(addprefix lib/,shared))

all: clean create_lib_dir static shared

create_lib_dir:
	mkdir -pv $(STATIC_LIB_DIR)
	mkdir -pv $(SHARED_LIB_DIR)

static:
	cd $(lib_dir)/zlib && $(MAKE) static
	cd $(lib_dir)/libpng-1.6.36 && $(MAKE) static
	cd $(lib_dir)/freetype-2.9 && $(MAKE) static
	cp $(lib_dir)/zlib/libz.a $(STATIC_LIB_DIR)
	cp $(lib_dir)/libpng-1.6.36/libpng16.a $(STATIC_LIB_DIR)
	cp $(lib_dir)/freetype-2.9/libfreetype.a $(STATIC_LIB_DIR)
	cd app && $(MAKE) static
shared:
	cd $(lib_dir)/zlib && $(MAKE) shared
	cd $(lib_dir)/libpng-1.6.36 && $(MAKE) shared
	cd $(lib_dir)/freetype-2.9 && $(MAKE) shared
	cp $(lib_dir)/zlib/libz.so.1.2.11 $(SHARED_LIB_DIR)
	cp $(lib_dir)/libpng-1.6.36/libpng16.so $(SHARED_LIB_DIR)
	cp $(lib_dir)/freetype-2.9/libfreetype.so.6.12.6 $(SHARED_LIB_DIR)
	cd app && $(MAKE) shared
clean:
	cd $(lib_dir)/zlib && $(MAKE) clean
	cd $(lib_dir)/libpng-1.6.36 && $(MAKE) clean
	cd $(lib_dir)/freetype-2.9 && $(MAKE) clean
	cd app && $(MAKE) clean
	rm -rf $(STATIC_LIB_DIR) $(SHARED_LIB_DIR)
