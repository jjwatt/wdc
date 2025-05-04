#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "wdc.h"

void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [OPTIONS]\n", program_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help          Show this help message and exit\n");
    fprintf(stderr, "  -l, --list          List bookmarks\n");
    fprintf(stderr, "  -a, --add [name]    Add current directory with name\n");
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

