/*
 * iso2kr - Convert ISO C to K&R format
 * 
 * Wrapper around clang-format with K&R style configuration.
 * Reads an ISO C formatted file and outputs K&R formatted code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024
#define MAX_ARGS 16

int main(int argc, char *argv[])
{
    char cmd[BUFSIZE];
    char *input_file = NULL;
    char *output_file = NULL;
    int i;

    if (argc < 2) {
        fprintf(stderr, "Usage: iso2kr <inputfile> [-o <outputfile>]\n");
        fprintf(stderr, "  Converts ISO C formatted code to K&R format\n");
        return 1;
    }

    /* Parse arguments */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_file = argv[++i];
        } else if (input_file == NULL) {
            input_file = argv[i];
        }
    }

    if (input_file == NULL) {
        fprintf(stderr, "Error: No input file specified\n");
        return 1;
    }

    /* Check if input file exists */
    if (access(input_file, R_OK) != 0) {
        fprintf(stderr, "Error: Cannot read file '%s'\n", input_file);
        return 1;
    }

    /* Build clang-format command */
    if (output_file != NULL) {
        snprintf(cmd, BUFSIZE,
                 "clang-format --style=file:/dd/PROJECTS/iso2kr/.clang-format \"%s\" > \"%s\"",
                 input_file, output_file);
    } else {
        snprintf(cmd, BUFSIZE,
                 "clang-format --style=file:/dd/PROJECTS/iso2kr/.clang-format \"%s\" > /tmp/iso2kr.tmp && "
                 "mv /tmp/iso2kr.tmp \"%s\"",
                 input_file, input_file);
    }

    return system(cmd);
}
