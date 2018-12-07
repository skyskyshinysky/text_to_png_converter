#ifndef _OPTION_H
#define _OPTION_H

#include <stdbool.h>
#include <stdio.h>

#define DEFAULT_PATH_LOGFILE        "app.log"
#define DEFAULT_PATH_OUTPUT_FILE    "app.output"

struct app_option
{
    FILE *logfile_ptr;
    FILE *output_file_ptr;
    bool debug_flag;
    char *logfile;
    char *ttf_file;
    char *message;
    char *output_file;
};
typedef struct app_option APP_OPTION;

void help(char *);
int parse_options(int, char **, APP_OPTION *);
void option_free(APP_OPTION **);

#endif