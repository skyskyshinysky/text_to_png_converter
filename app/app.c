#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ft2build.h>
#include <png.h>
#include <ctype.h>
#include "option.h"
#include FT_FREETYPE_H

#ifdef DYNAMIC_APP
#include <dlfcn.h>

void *d_freetype = NULL, *d_libpng = NULL, *d_zlib = NULL;

int init_zlib_shared(void);

#endif

#define ADDITIONAL_ROW  25
#define SPACE_ROW       15

int init_freetype(FT_Library *, FT_Face *, const char *);
int check_access_font_file(const char *);
int ft_load_char_and_render_glyph(const FT_Face, const char);
void draw(unsigned char **image, const size_t, const size_t, 
            const unsigned char *, const size_t, const FT_Pos, const size_t);
void draw_glyph(const FT_GlyphSlot, unsigned char **, const size_t, const size_t);
int destruct(const size_t, unsigned char **, APP_OPTION *, FT_Face, FT_Library);
int render_png_file(FILE *, const size_t, const size_t, unsigned char **);

int main(int argc, char * argv [])
{
    APP_OPTION app_opt;
    FT_Library ft_library = NULL;
    FT_Face ft_face = NULL;
    FT_GlyphSlot glyph_slot = NULL;
    size_t count = 0, max_row = 0, max_column = 0, current_width = 0;
    unsigned char **new_image = NULL;

#ifdef DYNAMIC_APP
    if(init_zlib_shared())
    {
        return EXIT_FAILURE;
    }
#endif

    if(parse_options(argc, argv, &app_opt))
    {
        fprintf(stderr, "[ERROR]: %s -- parse_options() fail!\n", __func__);
        return EXIT_FAILURE;
    }

    if((app_opt.output_file_ptr = fopen(app_opt.output_file, "wb")) == NULL)
    {
        fprintf(stderr, "[ERROR]: %s -- fopen() fail!\n", __func__);
        return EXIT_FAILURE;
    }
    
    if(init_freetype(&ft_library, &ft_face, app_opt.ttf_file))
    {
        fprintf(stderr, "[ERROR]: %s -- init_freetype() fail!\n", __func__);
        return EXIT_FAILURE;
    }
    
    glyph_slot = ft_face->glyph;

    for(; count < strlen(app_opt.message); ++count)
    {
        if(isspace(app_opt.message[count]))
        {
            max_column += SPACE_ROW;
            continue;
        }
        
        if(ft_load_char_and_render_glyph(ft_face, app_opt.message[count]))
        {
            continue;
        }

        if(max_row < glyph_slot->bitmap.rows)
        {
            max_row = glyph_slot->bitmap.rows;
        }

        max_column += glyph_slot->bitmap.width;
    }

    new_image = (unsigned char **) malloc(sizeof(unsigned char*) * (max_row + ADDITIONAL_ROW));

    for(count = 0; count < (max_row + ADDITIONAL_ROW); count++)
    {
        new_image[count] = (unsigned char*)malloc(sizeof(unsigned char) * max_column); 
        memset(new_image[count], ' ', sizeof(unsigned char) * max_column);
    }

    for(count = 0; count < strlen(app_opt.message); ++count)
    {
        if(ft_load_char_and_render_glyph(ft_face, app_opt.message[count]))
        {
            continue;
        }

        if(!isspace(app_opt.message[count]))
        {
            draw_glyph(glyph_slot, new_image, max_row, current_width);
            current_width += glyph_slot->bitmap.width;   
        }
        else 
        {
            current_width += SPACE_ROW;
        }
    }
    render_png_file(app_opt.output_file_ptr, max_column, max_row+ADDITIONAL_ROW, new_image);
    destruct(max_row+ADDITIONAL_ROW, new_image, &app_opt, ft_face, ft_library);
    return EXIT_SUCCESS;
}

int ft_load_char_and_render_glyph(const FT_Face ft_face, const char char_code)
{
#ifdef DYNAMIC_APP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
    FT_Error (*FT_Render_Glyph)(FT_GlyphSlot, FT_Render_Mode) = NULL;
    FT_Error (*FT_Load_Char)(FT_Face, FT_ULong, FT_Int32) = NULL;
    char * error_message = NULL;
    
    *(void **) (&FT_Load_Char) = dlsym(d_freetype, "FT_Load_Char");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_freetype, \"FT_Load_Char\"); failed : %s\n", __func__, error_message);
    	return EXIT_FAILURE;
    }

    *(void **) (&FT_Render_Glyph) = dlsym(d_freetype, "FT_Render_Glyph");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_freetype, \"FT_Render_Glyph\"); failed : %s\n", __func__, error_message);
    	return EXIT_FAILURE;
    }
#pragma GCC diagnostic pop
#endif

    if(FT_Load_Char(ft_face, (FT_ULong) char_code, FT_LOAD_RENDER))
    {
        fprintf(stderr, "[WARNING]: %c -- FT_Load_Char() fail!\n", char_code);
        return EXIT_FAILURE;
    }

    if((FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL)))
    {
        fprintf(stderr, "ERROR: fail function FT_Render_Glyph\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int init_freetype(FT_Library *ft_library, FT_Face *ft_face, const char *font_file)
{
    FT_Error ft_error;

#ifdef DYNAMIC_APP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
    char * error_message = NULL;
    FT_Error (*FT_Init_FreeType)(FT_Library*) = NULL;
    FT_Error (*FT_New_Face)(FT_Library, const char*, FT_Long, FT_Face *) = NULL;
    FT_Error (*FT_Set_Pixel_Sizes)(FT_Face, FT_UInt, FT_UInt) = NULL;

    d_freetype = dlopen(LIBFREETYPE_PATH, RTLD_LAZY);
    if(!d_freetype)
    {
    	fprintf(stderr, "[ERROR]: %s -- dlopen fail : %s\n", __func__, dlerror());
    	return EXIT_FAILURE;
    }
    *(void **) (&FT_Init_FreeType) = dlsym(d_freetype, "FT_Init_FreeType");
    error_message = dlerror();
    if(error_message != NULL) 
    {
    	fprintf(stderr, "[ERROR]: %s -- dlsym(d_freetype, \"FT_Init_FreeType\"); fail : %s\n", __func__, error_message);
    	return EXIT_FAILURE;
    }
    *(void **) (&FT_New_Face) = dlsym(d_freetype, "FT_New_Face");
    error_message = dlerror();
    if(error_message != NULL) 
    {
    	fprintf(stderr, "[ERROR]: %s -- dlsym(d_freetype, \"FT_New_Face\"); failed : %s\n", __func__, error_message);
    	return EXIT_FAILURE;
    }
    *(void **) (&FT_Set_Pixel_Sizes) = dlsym(d_freetype, "FT_Set_Pixel_Sizes");
    error_message = dlerror();
    if(error_message != NULL) 
    {
    	printf("[ERROR]: %s -- dlsym(d_freetype, \"FT_Set_Pixel_Sizes\"); failed : %s\n", __func__, error_message);
    	return EXIT_FAILURE;
    }
#pragma GCC diagnostic pop
#endif

    if((ft_error = FT_Init_FreeType(ft_library)))
    {
    	fprintf(stderr, "[ERROR]: %s() -- FT_Init_FreeType() fail!\n", __func__);
    	return EXIT_FAILURE;
    }

    if(check_access_font_file(font_file))
    {
        return EXIT_FAILURE;
    }

    if((ft_error = FT_New_Face(*ft_library, font_file, 0, ft_face)) == FT_Err_Unknown_File_Format)
    {
    	fprintf(stderr, "[ERROR]: %s() -- FT_New_Face fail!\n", __func__);
    	return EXIT_FAILURE;
    }
    if((ft_error = FT_Set_Pixel_Sizes(*ft_face, 0, 100)))
    {
    	fprintf(stderr, "[ERROR]: %s() -- FT_Set_Pixel_Sizes fail!\n", __func__);
    	return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int check_access_font_file(const char *path_file)
{
    if(access(path_file, R_OK) != 0)
	{
		fprintf(stderr, "[ERROR]: %s() -- %s is not readable (access denied)\n", 
            __func__, path_file);
		return EXIT_FAILURE;
	}
    return EXIT_SUCCESS;
}

void draw(unsigned char **image, const size_t rows_glyph, const size_t width_glyph, 
            const unsigned char *bitmap_buffer, const size_t max_rows_image,
            const FT_Pos origin, const size_t current_offset)
{

    for (size_t row = 0; row < rows_glyph; ++row)
    {
        for (size_t column = 0; column < width_glyph; ++column)
        {
            unsigned char c = bitmap_buffer[row * width_glyph + column]; 

            if(c > 128)
            {
                image[row+max_rows_image-rows_glyph+(size_t)origin][column+current_offset] = 255;
            }      
        }
    }
}

void draw_glyph(const FT_GlyphSlot glyph_slot, unsigned char **image, 
    const size_t max_rows_image, const size_t current_offset)
{
    FT_Pos origin = 0;

    if((glyph_slot->metrics.height >> 6) > glyph_slot->bitmap_top)
    {
        origin = (glyph_slot->metrics.height >> 6) - glyph_slot->bitmap_top;
    }

    draw(image, glyph_slot->bitmap.rows, glyph_slot->bitmap.width,
        glyph_slot->bitmap.buffer, max_rows_image, origin, current_offset);
}

int destruct(const size_t rows, unsigned char **image, APP_OPTION *app_opt, FT_Face ft_face, FT_Library ft_library)
{
    size_t count = 0;

#ifdef DYNAMIC_APP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
    FT_Error (*FT_Done_FreeType)(FT_Library) = NULL;
 	FT_Error (*FT_Done_Face)( FT_Face) = NULL; 
    char * error_message = NULL;
    *(void **) (&FT_Done_Face) = dlsym(d_freetype, "FT_Done_Face");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_freetype, \"FT_Done_Face\"); failed : %s\n", __func__, error_message);
    	return EXIT_FAILURE;
    }
    *(void **) (&FT_Done_FreeType) = dlsym(d_freetype, "FT_Done_FreeType");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_freetype, \"FT_Done_FreeType\"); failed : %s\n", __func__, error_message);
    	return EXIT_FAILURE;
    }
#pragma GCC diagnostic pop
#endif
    
    for(; count < rows; count++)
    {
        free(image[count]);
    }
    free(image);
    option_free(&app_opt);
    FT_Done_Face(ft_face); 
    FT_Done_FreeType(ft_library);

    return EXIT_SUCCESS;
}

int render_png_file(FILE *output_file, const size_t width, const size_t height, unsigned char **image)
{
    png_structp png_output = NULL;
    png_infop png_info = NULL;
    size_t count = 0;
    unsigned char* rowptr = NULL;

#ifdef DYNAMIC_APP
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
    char * error_message = NULL;
    png_structp (*png_create_write_struct)(png_const_charp, png_voidp, png_error_ptr, png_error_ptr) = NULL;
    png_infop (*png_create_info_struct)(png_structp) = NULL;
    jmp_buf* (*png_set_longjmp_fn)(png_structp, png_longjmp_ptr, size_t) = NULL;
    void (*png_init_io)(png_structp, png_FILE_p) = NULL;
    void (*png_set_IHDR)(png_structp, png_infop, png_uint_32, png_uint_32, int, int, int, int, int) = NULL;
    void (*png_write_info)(png_structp, png_infop) = NULL;
    void (*png_write_row)(png_structp, png_const_bytep) = NULL;
    void (*png_write_end)(png_structp, png_infop) = NULL;
    void (*png_destroy_write_struct)(png_structpp, png_infopp) = NULL;

    d_libpng = dlopen(LIBPNG_PATH, RTLD_LAZY);
    if(!d_libpng)
    {
        
    	fprintf(stderr, "[ERROR]: %s -- dlopen fail : %s\n", __func__, dlerror());
    	return EXIT_FAILURE;
    }
    *(void **) (&png_create_write_struct) = dlsym(d_libpng, "png_create_write_struct");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_libpng, \"png_create_write_struct\"); failed : %s\n", __func__, error_message);
        return EXIT_FAILURE;
    }
    *(void **) (&png_create_info_struct) = dlsym(d_libpng, "png_create_info_struct");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_libpng, \"png_create_info_struct\"); failed : %s\n", __func__, error_message);
        return EXIT_FAILURE;
    }
    *(void **) (&png_init_io) = dlsym(d_libpng, "png_init_io");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_libpng, \"png_init_io\"); failed : %s\n", __func__, error_message);
        return EXIT_FAILURE;
    }
    *(void **) (&png_set_IHDR) = dlsym(d_libpng, "png_set_IHDR");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_libpng, \"png_set_IHDR\"); failed : %s\n", __func__, error_message);
        return EXIT_FAILURE;
    }
    *(void **) (&png_write_info) = dlsym(d_libpng, "png_write_info");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_libpng, \"png_write_info\"); failed : %s\n", __func__, error_message);
        return EXIT_FAILURE;
    }
    *(void **) (&png_write_row) = dlsym(d_libpng, "png_write_row");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_libpng, \"png_write_row\"); failed : %s\n", __func__, error_message);
        return EXIT_FAILURE;
    }
    *(void **) (&png_set_longjmp_fn) = dlsym(d_libpng, "png_set_longjmp_fn");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_libpng, \"png_set_longjmp_fn\"); failed : %s\n", __func__, error_message);
        return EXIT_FAILURE;
    }
    *(void **) (&png_write_end) = dlsym(d_libpng, "png_write_end");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_libpng, \"png_write_end\"); failed : %s\n", __func__, error_message);
        return EXIT_FAILURE;
    }
    *(void **) (&png_destroy_write_struct) = dlsym(d_libpng, "png_destroy_write_struct");
    error_message = dlerror();
    if(error_message != NULL) 
    {
        fprintf(stderr, "[ERROR]: %s -- dlsym(d_libpng, \"png_destroy_write_struct\"); failed : %s\n", __func__, error_message);
        return EXIT_FAILURE;
    }
#pragma GCC diagnostic pop
#endif
    
    if((png_output = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) == NULL)
    {
        fprintf(stderr, "[ERROR]: %s -- png_create_write_struct() fail!\n", __func__);
        return EXIT_FAILURE;
    }

    if((png_info = png_create_info_struct(png_output)) == NULL)
    {
        fprintf(stderr, "[ERROR]: %s -- png_create_info_struct() fail!\n", __func__);
        png_destroy_write_struct (&png_output, (png_infopp)NULL);
        return EXIT_FAILURE;
    }

    if(setjmp(png_jmpbuf(png_output)))
    {
        fprintf(stderr, "[ERROR]: %s -- png init io fail!\n", __func__);
        png_destroy_write_struct (&png_output, (png_infopp)NULL);
        return EXIT_FAILURE;
    }
    png_init_io(png_output, output_file);
    
    if (setjmp(png_jmpbuf(png_output)))
	{
        fprintf(stderr, "[ERROR]: %s -- IHDR write fail!\n", __func__);
		png_destroy_write_struct (&png_output, (png_infopp)NULL);
        return EXIT_FAILURE;
	}

    png_set_IHDR(png_output, png_info, (png_uint_32) width, (png_uint_32) height, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    if (setjmp(png_jmpbuf(png_output)))
	{
        fprintf(stderr, "[ERROR]: %s -- png write fail!\n", __func__);
		png_destroy_write_struct (&png_output, (png_infopp)NULL);
        return EXIT_FAILURE;
	}

    png_write_info(png_output, png_info);

    if (setjmp(png_jmpbuf(png_output)))
	{
        fprintf(stderr, "[ERROR]: %s -- png write fail!\n", __func__);
		png_destroy_write_struct (&png_output, (png_infopp)NULL);
        return EXIT_FAILURE;
	}

    for (; count < height; ++count)
	{
		rowptr = image[count];
		png_write_row(png_output, rowptr);
	}

	if (setjmp(png_jmpbuf(png_output)))
	{
        fprintf(stderr, "[ERROR]: %s -- png_write_end fail!\n", __func__);
		png_destroy_write_struct (&png_output, (png_infopp)NULL);
        return EXIT_FAILURE;
	}
	png_write_end(png_output, NULL);
    png_destroy_write_struct (&png_output, (png_infopp)NULL);
    return EXIT_SUCCESS;
}

#ifdef DYNAMIC_APP
int init_zlib_shared(void)
{
    d_zlib = dlopen(LIBZ_PATH, RTLD_NOW | RTLD_GLOBAL);
	if(!d_zlib) 
	{
		fprintf(stderr, "[ERROR]: %s -- dlopen fail : %s\n", __func__, dlerror());
		return EXIT_FAILURE;
	}
    return EXIT_SUCCESS;
}
#endif