#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <assert.h>

#include "option.h"

int init_option(APP_OPTION *);
void option_free(APP_OPTION **);

#define SHORT_OPTIONS_APP   "hdl:t:m:f:g:"


int create_and_copy_buffer(char **buffer, const char *dst_buffer)
{
    size_t size = strlen(dst_buffer) + 1;

    if(*buffer != NULL)
    {
        free(*buffer);
    }

    if((*buffer = (char *)malloc(sizeof(char) * size)) == NULL)
    {
        return EXIT_FAILURE;
    }

    memset(*buffer, 0x00, size);
    strncpy(*buffer, dst_buffer, strlen(dst_buffer));

    return EXIT_SUCCESS;
}

struct option long_option_app[] =
{
    { "help", no_argument, NULL, 'h'},
    { "debug", no_argument, NULL, 'd'},
    { "log-file", required_argument, NULL, 'l'},
    { "ttf-file", required_argument, NULL, 't'},
    { "message", required_argument, NULL, 'm'},
    { "file-output", required_argument, NULL, 'f'}
};

int parse_options(int argc, char * argv [], APP_OPTION *opt)
{
    int current_option = 0;
    const char * option_logfile = NULL;
    const char * option_ttf_file = NULL;
    const char * option_message = NULL;
    const char * option_output_file = NULL;

    assert(argv != NULL);
    assert(opt != NULL);

    if(init_option(opt))
    {
        return EXIT_FAILURE;
    }

    while((current_option = getopt_long(argc, argv, SHORT_OPTIONS_APP, long_option_app, NULL)) != -1)
    {
        switch(current_option)
        {
            case 'h':
                help(argv[0]);
                return EXIT_FAILURE;
            case 'd':
                opt->debug_flag = true;
                break;
            case 'l':
                option_logfile = optarg;
                break;
            case 't':
                option_ttf_file = optarg;
                break;
            case 'm':
                option_message = optarg;
                break;
            case 'f':
                option_output_file = optarg;
                break;
            default:
                option_free(&opt);
                return EXIT_FAILURE;
        }
    }

    if(option_logfile)
    {
        if(create_and_copy_buffer(&opt->logfile, option_logfile))
        {
            fprintf(stderr, "[ERROR]: %s() failed --> not enough memory for logfile buffer\n", __func__);
            option_free(&opt);
            return EXIT_FAILURE;
        }
    }

    if(option_ttf_file)
    {
        if(create_and_copy_buffer(&opt->ttf_file, option_ttf_file))
        {
            fprintf(stderr, "[ERROR]: %s() failed --> not enough memory for TTF file buffer\n", __func__);
            option_free(&opt);
            return EXIT_FAILURE;
        }
    }

    if(option_message)
    {
        if(create_and_copy_buffer(&opt->message, option_message))
        {
            fprintf(stderr, "[ERROR]: %s() failed --> not enough memory for message buffer\n", __func__);
            option_free(&opt);
            return EXIT_FAILURE;
        }
    }

    if(option_output_file)
    {
        if(create_and_copy_buffer(&opt->output_file, option_output_file))
        {
            fprintf(stderr, "[ERROR]: %s() failed --> not enough memory for output file buffer\n", __func__);
            option_free(&opt);
            return EXIT_FAILURE;
        }
    }

    fprintf(stdout, "Log file:      %s\n"
                    "TTF file:      %s\n"
                    "Message:       %s\n"
                    "Output file:   %s\n", opt->logfile, opt->ttf_file,
                    opt->message, opt->output_file );

    return EXIT_SUCCESS;
}

int init_option(APP_OPTION *opt)
{
    memset(opt, 0x00, sizeof(APP_OPTION));

    opt->debug_flag = false;

    if(create_and_copy_buffer(&(opt->logfile), DEFAULT_PATH_LOGFILE))
    {
        return EXIT_FAILURE;
    }

    if(create_and_copy_buffer(&(opt->output_file), DEFAULT_PATH_OUTPUT_FILE))
    {
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

void help(char *prog_name)
{
    fprintf(stdout, "Usage %s [OPTIONS] \n"
        "OPTIONS: \n"
        "           -d, --debug         <-- Enable debug mode\n"
        "           -h, --help          <-- Print help message and exit\n"
        "           -l, --log-file      <-- Set log file path\n"
        "           -t, --ttf-file      <-- Set ttf file\n"
        "           -m, --message       <-- Set output message\n"
        "           -f, --file-output   <-- Set name output file\n"
        "           -g, --grade         <-- Set grade log message\n", prog_name);
}

void option_free(APP_OPTION ** opt)
{
    APP_OPTION *opt_ptr = *opt;
    free(opt_ptr->logfile);
    free(opt_ptr->output_file);
    free(opt_ptr->ttf_file);
    free(opt_ptr->message);
    opt_ptr = NULL;
}
