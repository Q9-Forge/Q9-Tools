/*
 * kr2iso - Convert K&R to ISO C format
 * 
 * Wrapper around clang-format with ISO (LLVM) style configuration.
 * Reads a K&R formatted file and outputs ISO C formatted code.
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
        fprintf(stderr, "Usage: kr2iso <inputfile> [-o <outputfile>]\n");
        fprintf(stderr, "  Converts K&R formatted code to ISO C format\n");
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
                 "clang-format --style=file:/dd/PROJECTS/kr2iso/.clang-format \"%s\" > \"%s\"",
                 input_file, output_file);
    } else {
        snprintf(cmd, BUFSIZE,
                 "clang-format --style=file:/dd/PROJECTS/kr2iso/.clang-format \"%s\" > /tmp/kr2iso.tmp && "
                 "mv /tmp/kr2iso.tmp \"%s\"",
                 input_file, input_file);
    }

    return system(cmd);
}
