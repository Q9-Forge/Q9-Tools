/*
 * Q9 functional directory tool.
 * The source/module name remains "dir"; the delivered file is Qdir.
 */

#include <stdio.h>
#include <dir.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <UNIX/stat.h>

extern int stat(const char *, struct stat *);

struct name_node {
    char *name;
    long address;
    struct name_node *next;
};

struct list_options {
    int all_files;
    int unformatted;
    int sorted;
    int extended;
    int show_dirs;
    int recursive;
    int no_descend;
    int max_depth;
};

static void usage(void)
{
    fputs("Syntax: dir [<opts>] {<dir names> [<opts>]}\015", stdout);
    fputs("Options:\015", stdout);
    fputs("    -a        show all files\015", stdout);
    fputs("    -u        unformatted listing\015", stdout);
    fputs("    -s        unsorted dir listing\015", stdout);
    fputs("    -e        extended dir listing\015", stdout);
    fputs("    -n        treat dirs like files\015", stdout);
    fputs("    -r        recursive dir listings\015", stdout);
    fputs("    -r=<num>  recursive dir listing to depth <num>\015", stdout);
    fputs("    -x        directory is execution dir\015", stdout);
    fputs("    -d        show directories with a slash\015", stdout);
    fputs("    -z=<path> get list of dir names from <path>\015", stdout);
    fputs("    -z        get list of dir names from standard input\015", stdout);
    fputs("Function: display directory contents\015", stdout);
}

static struct name_node *add_name(struct name_node *head, const char *name,
                                   long address, int sorted)
{
    struct name_node *node;
    struct name_node **place;

    node = (struct name_node *)malloc(sizeof *node);
    if (node == NULL)
        return head;
    node->name = (char *)malloc(strlen(name) + 1);
    if (node->name == NULL) {
        free((char *)node);
        return head;
    }
    strcpy(node->name, name);
    node->address = address;
    node->next = NULL;

    if (!sorted || head == NULL) {
        node->next = head;
        return node;
    }

    place = &head;
    while (*place != NULL && strcmp((*place)->name, name) < 0)
        place = &(*place)->next;
    node->next = *place;
    *place = node;
    return head;
}

static void free_names(struct name_node *head)
{
    struct name_node *next;

    while (head != NULL) {
        next = head->next;
        free(head->name);
        free((char *)head);
        head = next;
    }
}

static int make_path(char *buffer, int size, const char *directory,
                     const char *name)
{
    int length;

    length = strlen(directory);
    if (length > 0 && directory[length - 1] == '/')
        return sprintf(buffer, "%s%s", directory, name) < size;
    return sprintf(buffer, "%s/%s", directory, name) < size;
}

static int entry_is_directory(const char *directory, const char *name)
{
    char path[256];
    struct stat info;

    if (!make_path(path, sizeof path, directory, name))
        return 0;
    if (stat(path, &info) != 0)
        return 0;
    /* OS-9 keeps the permission bits in the same mode word.  A directory
       therefore has S_IFDIR set in addition to its access attributes; an
       exact masked comparison would reject values such as $00bf. */
    return (info.st_mode & S_IFDIR) != 0;
}

static void format_attributes(unsigned short mode, char *attributes)
{
    attributes[0] = (mode & S_IFDIR) ? 'd' : '-';
    attributes[1] = (mode & S_ISHARE) ? 's' : '-';
    attributes[2] = (mode & S_IOEXEC) ? 'e' : '-';
    attributes[3] = (mode & S_IOWRITE) ? 'w' : '-';
    attributes[4] = (mode & S_IOREAD) ? 'r' : '-';
    attributes[5] = (mode & S_IEXEC) ? 'e' : '-';
    attributes[6] = (mode & S_IWRITE) ? 'w' : '-';
    attributes[7] = (mode & S_IREAD) ? 'r' : '-';
    attributes[8] = 0;
}

static void print_directory_header(const char *directory)
{
    time_t now;
    struct tm *current;

    now = time((time_t *)0);
    current = localtime(&now);
    if (current == NULL)
        return;
    printf("\015                           Directory of %s %02d:%02d:%02d\015",
           directory, current->tm_hour, current->tm_min, current->tm_sec);
}

static void print_extended_header(void)
{
    fputs(" Owner    Last modified  Attributes Sector Bytecount Name\015",
          stdout);
    fputs("-------   -------------  ---------- ------ --------- ----\015",
          stdout);
}

static void print_names(struct name_node *head, const char *directory,
                        const struct list_options *options)
{
    int column;
    char path[256];
    char display[80];
    char full_name[256];
    char attributes[9];
    struct stat info;
    struct tm *modified;
    int is_directory;

    column = 0;
    while (head != NULL) {
        is_directory = entry_is_directory(directory, head->name);
        if (options->show_dirs && is_directory)
            sprintf(display, "%s/", head->name);
        else
            strcpy(display, head->name);

        if (options->extended && make_path(path, sizeof path, directory,
                                            head->name) &&
            stat(path, &info) == 0) {
            format_attributes(info.st_mode, attributes);
            modified = info.st_mtime == 0 ? NULL : localtime(&info.st_mtime);
            /* The OS-9 time library represents an unset date as its
               1999-11-30 epoch; dir prints that value as 00/00/00. */
            if (modified != NULL && modified->tm_year == 99 &&
                modified->tm_mon == 10 && modified->tm_mday == 30)
                modified = NULL;
            if (modified != NULL) {
                printf("%3u.%-3u %02d/%02d/%02d %02d%02d   ",
                       info.st_uid, info.st_gid,
                       modified->tm_year % 100, modified->tm_mon + 1,
                       modified->tm_mday, modified->tm_hour,
                       modified->tm_min);
            } else {
                printf("%3u.%-3u 00/00/00 0000   ",
                       info.st_uid, info.st_gid);
            }
            printf("%-8s %6lX %9lu %-s\015", attributes,
                   (unsigned long)head->address, info.st_size, display);
        } else if (options->unformatted) {
            if (make_path(full_name, sizeof full_name, directory,
                          head->name))
                printf("%s\015", full_name);
            else
                printf("%s\015", display);
        } else if (options->extended) {
            printf("%s\015", display);
        } else {
            printf("%-15s", display);
            ++column;
            if (column == 5) {
                putchar('\015');
                column = 0;
            }
        }
        head = head->next;
    }
    if (!options->unformatted && !options->extended && column != 0)
        putchar('\015');
}

static int list_directory(const char *path, const struct list_options *options,
                          int depth)
{
    DIR *directory;
    struct direct *entry;
    struct name_node *names;
    struct name_node *item;
    char child[256];

    if (depth > 32)
        return 1;
    directory = opendir(path);
    if (directory == NULL) {
        fprintf(stderr, "dir: cannot open %s\015", path);
        return 1;
    }

    names = NULL;
    while ((entry = readdir(directory)) != NULL) {
        if (!options->all_files && entry->d_name[0] == '.')
            continue;
        if (options->recursive &&
            (strcmp(entry->d_name, ".") == 0 ||
             strcmp(entry->d_name, "..") == 0))
            continue;
            names = add_name(names, entry->d_name, entry->d_addr,
                             options->sorted);
    }
    closedir(directory);

    if (options->no_descend && options->recursive) {
        printf("%s\015", path);
        free_names(names);
        return 0;
    }

    if (!options->unformatted) {
        print_directory_header(path);
        if (options->extended)
            print_extended_header();
    }
    print_names(names, path, options);

    if (options->recursive && !options->no_descend && depth < options->max_depth) {
        item = names;
        while (item != NULL) {
            if (entry_is_directory(path, item->name) &&
                make_path(child, sizeof child, path, item->name))
                list_directory(child, options, depth + 1);
            item = item->next;
        }
    }
    free_names(names);
    return 0;
}

static void strip_line(char *line)
{
    char *end;

    end = line + strlen(line);
    while (end > line && (end[-1] == '\015' || end[-1] == '\012'))
        --end;
    *end = 0;
}

int main(int argc, char **argv)
{
    struct list_options options;
    const char *paths[64];
    int path_count;
    FILE *path_list;
    char line[256];
    int status;

    options.all_files = 0;
    options.unformatted = 0;
    options.sorted = 1;
    options.extended = 0;
    options.show_dirs = 0;
    options.recursive = 0;
    options.no_descend = 0;
    options.max_depth = 32;
    path_count = 0;
    path_list = NULL;

    while (--argc > 0) {
        ++argv;
        if (strcmp(*argv, "-?") == 0) {
            usage();
            return 0;
        }
        if (strcmp(*argv, "-a") == 0) {
            options.all_files = 1;
            continue;
        }
        if (strcmp(*argv, "-u") == 0) {
            options.unformatted = 1;
            continue;
        }
        if (strcmp(*argv, "-s") == 0) {
            options.sorted = 0;
            continue;
        }
        if (strcmp(*argv, "-e") == 0) {
            options.extended = 1;
            continue;
        }
        if (strcmp(*argv, "-d") == 0) {
            options.show_dirs = 1;
            continue;
        }
        if (strcmp(*argv, "-r") == 0) {
            options.recursive = 1;
            continue;
        }
        if (strncmp(*argv, "-r=", 3) == 0) {
            char *end;
            long value;

            value = strtol(*argv + 3, &end, 10);
            if ((*argv)[3] == 0 || *end != 0 || value < 1 || value > 32) {
                fputs("dir: illegal size for -r option\015", stderr);
                return 1;
            }
            options.recursive = 1;
            options.max_depth = (int)value;
            continue;
        }
        if (strcmp(*argv, "-n") == 0) {
            options.no_descend = 1;
            continue;
        }
        if (strcmp(*argv, "-x") == 0)
            continue;
        if (strcmp(*argv, "-z") == 0) {
            path_list = stdin;
            continue;
        }
        if (strncmp(*argv, "-z=", 3) == 0) {
            path_list = fopen(*argv + 3, "r");
            if (path_list == NULL)
                return 1;
            continue;
        }
        if (**argv == '-') {
            usage();
            return 1;
        }
        if (path_count == 64) {
            usage();
            return 1;
        }
        paths[path_count++] = *argv;
    }

    status = 0;
    if (path_list != NULL) {
        while (fgets(line, sizeof line, path_list) != NULL) {
            strip_line(line);
            if (line[0] != 0 && list_directory(line, &options, 0) != 0)
                status = 1;
        }
        if (path_list != stdin)
            fclose(path_list);
    } else {
        int index;

        if (path_count == 0)
            paths[path_count++] = ".";
        for (index = 0; index < path_count; ++index) {
            if (list_directory(paths[index], &options, 0) != 0)
                status = 1;
        }
    }
    return status;
}
