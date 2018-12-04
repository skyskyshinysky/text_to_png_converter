#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ft2build.h>
#include <ctype.h>
#include "option.h"

#include FT_FREETYPE_H
#define ADDITIONAL_ROW  6

int init_freetype(FT_Library *, FT_Face *, const char *);
int check_access_font_file(const char *);
int ft_load_char_and_render_glyph(const FT_Face, const char);
void draw(unsigned char **image, const int, const int, 
            const unsigned char *, const int, const int, const int);

void draw_glyph(const FT_GlyphSlot, unsigned char **, const int, const int);
void destruct(const int, unsigned char **, APP_OPTION *, FT_Face, FT_Library);


int main(int argc, char * argv [])
{
    APP_OPTION app_opt;
    FT_Library ft_library = NULL;
    FT_Face ft_face = NULL;
    FT_GlyphSlot glyph_slot = NULL;
    int count = 0, current_width = 0;
    size_t max_row = 0, max_column = 0;
    unsigned char **new_image = NULL; 


    if(parse_options(argc, argv, &app_opt))
    {
        fprintf(stderr, "[ERROR]: %s -- parse_options() fail!\n", __func__);
        return EXIT_FAILURE;
    }

    if((app_opt.output_file_ptr = fopen(app_opt.output_file, "w")) == NULL)
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
            max_column += 8;
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
        else {
            current_width += 8;
        }
    }

    for (int y = 0; y < (max_row+ADDITIONAL_ROW); ++y)
    {
        for (int i = 0; i < max_column; ++i)
        {
            printf("%c", new_image[y][i]);
        }
        printf("\n");
    }

    
    return EXIT_SUCCESS;
}

int ft_load_char_and_render_glyph(const FT_Face ft_face, const char char_code)
{
    if(FT_Load_Char(ft_face, (FT_ULong) char_code, FT_LOAD_RENDER))
    {
        fprintf(stderr, "[WARNING]: %c -- FT_Load_Char() fail!\n", char_code);
        return EXIT_FAILURE;
    }

    if((FT_Render_Glyph(ft_face->glyph, FT_RENDER_MODE_NORMAL)))
    {
        fprintf(stderr, "ERROR: fail function FT_Render_Glyph\n");
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}

int init_freetype(FT_Library *ft_library, FT_Face *ft_face, const char *font_file)
{
    FT_Error ft_error;

    if((ft_error = FT_Init_FreeType(ft_library)))
    {
    	fprintf(stderr, "[ERROR]: %s -- FT_Init_FreeType() fail!\n", __func__);
    	return EXIT_FAILURE;
    }

    if(check_access_font_file(font_file))
    {
        return EXIT_FAILURE;
    }

    if((ft_error = FT_New_Face(*ft_library, font_file, 0, ft_face)) == FT_Err_Unknown_File_Format)
    {
    	fprintf(stderr, "ERROR: %s -- FT_New_Face fail!\n", __func__);
    	return EXIT_FAILURE;
    }
    if((ft_error = FT_Set_Pixel_Sizes(*ft_face, 0, 25)))
    {
    	fprintf(stderr, "ERROR: %s -- FT_Set_Pixel_Sizes fail \n", __func__);
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

void draw(unsigned char **image, const int rows_glyph, const int width_glyph, 
            const unsigned char *bitmap_buffer, const int max_rows_image,
            const int origin, const int current_offset)
{
    for (int row = 0; row < rows_glyph; ++row)
    {
        for (int column = 0; column < width_glyph; ++column)
        {
            unsigned char c = bitmap_buffer[row * width_glyph + column]; 

            if(c > 128)
            {
                image[row+max_rows_image-rows_glyph+origin][column+current_offset] = '#';
            }      
        }
    }
}

void draw_glyph(const FT_GlyphSlot glyph_slot, unsigned char **image, 
    const int max_rows_image, const int current_offset)
{
    int origin = 0;

    if((glyph_slot->metrics.height >> 6) > glyph_slot->bitmap_top)
    {
        origin = (glyph_slot->metrics.height >> 6) - glyph_slot->bitmap_top;
    }

    draw(image, glyph_slot->bitmap.rows, glyph_slot->bitmap.width,
        glyph_slot->bitmap.buffer, max_rows_image, origin, current_offset);
}

void destruct(const int rows, unsigned char **image, APP_OPTION *app_opt, FT_Face ft_face, FT_Library ft_library)
{
    int count = 0;
    for(; count < rows; count++)
    {
        free(image[count]);
    }
    free(image);
    option_free(&app_opt);
    FT_Done_Face(ft_face); 
    FT_Done_FreeType(ft_library);
}