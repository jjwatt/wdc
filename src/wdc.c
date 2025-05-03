#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>

#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define DELIM "|"
#define BM_FILENAME ".wdc"
#define MAX_LINE_LENGTH (PATH_MAX + 50)

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} Bookmarks;


void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [OPTIONS] <filename>\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help          Show this help message and exit\n");
    fprintf(stderr, "  -l, --list          List bookmarks\n");
    fprintf(stderr, "  -a, --add [name]    Add current directory with name\n");
}

FILE *open_bookmark_file(const char *mode) {
    char bookmark_path[PATH_MAX];
    snprintf(bookmark_path, sizeof(bookmark_path), "%s/%s", getenv("HOME"), BM_FILENAME);
    FILE *bookmark_file = fopen(bookmark_path, mode);
    if (bookmark_file == NULL) {
	fprintf(stderr, "Error: Could not open bookmarks file '%s'.\n", bookmark_path);
    }
    return bookmark_file;

}

void add_to_file(const char *name, const char *cwd_path, FILE *bookmark_file) {
    fprintf(bookmark_file, "%s%s%s\n", name, DELIM, cwd_path);
}

int add(const char *name) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
	perror("Error getting current directory");
	return 1;
    }

    FILE *bookmark_file = open_bookmark_file("a");
    if (bookmark_file == NULL) {
	return 1;
    }

    add_to_file(name, cwd, bookmark_file);
    fclose(bookmark_file);
    return 0;
}

Bookmarks get_bookmarks() {
    Bookmarks bookmarks = {0};

    FILE *bookmark_file = open_bookmark_file("r");
    if (bookmark_file == NULL) {
	return bookmarks;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), bookmark_file) != NULL) {
	line[strcspn(line, "\n")] = 0;
	char *bookmark = strdup(line);
	if (bookmark == NULL) {
	    perror("strdup() failed");
	    break;
	}
	da_append(&bookmarks, bookmark);
    }
    fclose(bookmark_file);
    return bookmarks;
}

int list_bookmarks() {
    Bookmarks bookmarks = get_bookmarks();
    for (int i = bookmarks.count - 1; i >= 0; i--) {
	printf("%s\n", bookmarks.items[i]);
    }
    da_free(bookmarks);
    return 0;
}

int main(int argc, char **argv) {
    int opt;
    opterr = 0;

    /* Define long options */
    struct option long_options[] = {
	{"help", no_argument, 0, 'h'},
	{"list", no_argument, 0, 'l'},
	{"add", required_argument, NULL, 'a'},
	{0, 0, 0, 0}
    };

    /* Process command line options */
    while ((opt = getopt_long(argc, argv, "hla:", long_options, NULL)) != -1) {
	switch (opt) {
	case 'h':
	    print_usage(argv[0]);
	    return 0;
	case 'l':
	    return list_bookmarks();
	case 'a':
	    if (add(optarg) != 0) {
		return 1;
	    }
	    break;
	default:
	    print_usage(argv[0]);
	    return 1;
	}
    }
    return 0;
}

