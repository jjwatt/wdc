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
	// Find the location of the first occurrence of newline,
	// And replace it with NULL terminator.
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

/**
 * @brief Get bookmarks in reverse order
 *
 * This will get them in the order added to the file
 */
Bookmarks get_bookmarks_reversed() {
    Bookmarks bookmarks = get_bookmarks();
    // If there's none or one just return that.
    if (bookmarks.items == NULL || bookmarks.count <= 1) {
	return bookmarks;
    }

    // Reverse the list.
    // Left side.
    size_t i = 0;
    // Rigth side.
    size_t j = bookmarks.count - 1;
    while (i < j) {
	// Swap items[i] and items[j]
	char *tmp = bookmarks.items[i];
	bookmarks.items[i] = bookmarks.items[j];
	bookmarks.items[j] = tmp;
	i++;
	j--;
    }
    return bookmarks;
}

/**
   @brief Print bookmarks in reverse order
*/
int list_bookmarks() {
    Bookmarks bookmarks = get_bookmarks_reversed();
    for (size_t i = 0; i < bookmarks.count; i++) {
	printf("%s\n", bookmarks.items[i]);
        // free items[i]. rn we're leaking memory probably.
    }
    da_free(bookmarks);
    return 0;
}

char *find(const char *name) {
    // Search from the bottom. So, it's the last one added.
    Bookmarks bookmarks = get_bookmarks_reversed();
    char *found_path = NULL;

    // Simple linear search. Items can repeat. We get the last one added.
    for (size_t i = 0; i < bookmarks.count; i++) {
	char *entry = bookmarks.items[i];
	char *delimiter = strstr(entry, DELIM);
	if (delimiter != NULL) {
	    size_t name_len = delimiter - entry;
	    if (strncmp(entry, name, name_len) == 0 && name_len == strlen(name)) {
		found_path = strdup(delimiter + strlen(DELIM));
		break;
	    }
	}
    }
    // TODO: free items[i], free bookmarks
    return found_path;
}

int main(int argc, char **argv) {
    char *find_name = NULL;

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

    /* Check for non-option arguments after getopt_long */
    if (optind < argc) {
	find_name = argv[optind];
    }
    if (find_name != NULL) {
	char *path = find(find_name);
	if (path != NULL) {
	    printf("%s\n", path);
	    free(path);
	} else {
	    // TODO: Maybe don't print anything
	    printf("Bookmark '%s' not found\n", find_name);
	    return 1;
	}
    } else if (argc == 1) {
	print_usage(argv[0]);
    }
    return 0;
}

