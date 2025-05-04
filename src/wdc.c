#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>

#include "wdc.h"

#define NOB_IMPLEMENTATION
#include "nob.h"


FILE *open_bookmark_file(const char *mode) {
    char bookmark_path[PATH_MAX];
    snprintf(bookmark_path, sizeof(bookmark_path), "%s/%s", getenv("HOME"), BM_FILENAME);
    FILE *bookmark_file = fopen(bookmark_path, mode);
    if (bookmark_file == NULL) {
	fprintf(stderr, "Error: Could not open bookmarks file '%s'.\n", bookmark_path);
    }
    return bookmark_file;

}

/**
 * @brief Add the name and path to the file as a bookmark.
 *
 * @param name: The bookmark name.
 * @param cwd_path: The path to add under bookmark name.
 * @param bookmark_file: The file to append the bookmark to.
 */
void add_to_file(const char *name, const char *cwd_path, FILE *bookmark_file) {
    fprintf(bookmark_file, "%s%s%s\n", name, DELIM, cwd_path);
}

/**
 * @brief Add a bookmark to the bookmarks file.
 *
 * Append bookmark and path to the end of the bookmarks file.
 * @param name: The name of the bookmark.
 */
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

/**
 * @brief Get bookmarks
 *
 * Read the file and return the list of lines in the file.
 *
 * @return Bookmarks All the bookmarks from the file.
 */
Bookmarks get_bookmarks(void) {
    Bookmarks bookmarks = {0};
    Nob_String_Builder file_sb = {0};

    char bookmark_path[PATH_MAX];
    snprintf(bookmark_path, sizeof(bookmark_path),
	     "%s/%s", getenv("HOME"), BM_FILENAME);
    if (!nob_read_entire_file(bookmark_path, &file_sb)) return bookmarks;

    Nob_String_View content = {
	.data = file_sb.items,
	.count = file_sb.count
    };

    while (content.count > 0) {
	Nob_String_Builder bookmark = {0};
	Nob_String_View line = nob_sv_chop_by_delim(&content, '\n');
	nob_sb_append_buf(&bookmark, line.data, line.count);
	nob_sb_append_null(&bookmark);
	nob_da_append(&bookmarks, bookmark);
    }
    /* nob_log(NOB_INFO, "%zu", bookmarks.count); */
    nob_sb_free(file_sb);
    return bookmarks;
}

/**
 * @brief Get bookmarks in reverse order
 *
 * This will get them in the order added to the file.
 *
 * @return Bookmarks, the list of bookmarks in reverse order.
 */
Bookmarks get_bookmarks_reversed(void) {
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
	Nob_String_Builder tmp = bookmarks.items[i];
	bookmarks.items[i] = bookmarks.items[j];
	bookmarks.items[j] = tmp;
	i++;
	j--;
    }
    return bookmarks;
}

/**
 * @brief Print bookmarks in reverse order
*/
int list_bookmarks() {
    Bookmarks bookmarks = get_bookmarks_reversed();
    for (size_t i = 0; i < bookmarks.count; i++) {
	printf("%s\n", bookmarks.items[i].items);
	nob_sb_free(bookmarks.items[i]);
    }
    nob_da_free(bookmarks);
    return 0;
}

/**
 * @brief Find an exact match for name and return it.
 *
 * Search through bookmarks and return the first exact match for name.
 *
 * @param name The name to search for.
 *
 * @return The matching name|path in the file.
 */
char *find(const char *name) {
    // Search from the bottom. So, it's the last one added.
    Bookmarks bookmarks = get_bookmarks_reversed();
    char *found_path = NULL;

    // Simple linear search. Items can repeat. We get the last one added.
    for (size_t i = 0; i < bookmarks.count; i++) {
	Nob_String_Builder entry_sb = bookmarks.items[i];
	char *entry = entry_sb.items;
	char *delimiter = strstr(entry, DELIM);
	if (delimiter != NULL) {
	    size_t name_len = delimiter - entry;
	    if (strncmp(entry, name, name_len) == 0 && name_len == strlen(name)) {
		found_path = strdup(delimiter + strlen(DELIM));
		break;
	    }
	}
	nob_sb_free(entry_sb);
    }
    nob_da_free(bookmarks);
    return found_path;
}
