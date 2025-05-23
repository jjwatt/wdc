/* -*- mode: c; c-basic-offset: 4 -*- */
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>

#include "wdc.h"

#define NOB_IMPLEMENTATION
#include "nob.h"


Nob_String_Builder get_bookmark_path(void) {
    Nob_String_Builder sb = {0};
    char bookmark_path[PATH_MAX];
    char *env_bookmark_path = getenv("WDC_BOOKMARK_FILE");
    if (env_bookmark_path == NULL) {
	// WDC_BOOKMARK_FILE not set. Use default.
	char *home_dir = getenv("HOME");
	if (home_dir == NULL) {
	    fprintf(stderr, "HOME env var not set. Using current directory for default bookmark file.");
	    strncpy(bookmark_path, BM_FILENAME, sizeof(bookmark_path) - 1);
	} else {
	    snprintf(bookmark_path, sizeof(bookmark_path), "%s/%s", getenv("HOME"), BM_FILENAME);
	}
    } else {
	strncpy(bookmark_path, env_bookmark_path, sizeof(bookmark_path) - 1);
    }
    nob_sb_append_cstr(&sb, bookmark_path);
    nob_sb_append_null(&sb);
    return sb;
}

FILE *open_bookmark_file(const char *mode) {
    /* char bookmark_path[PATH_MAX]; */
    /* snprintf(bookmark_path, sizeof(bookmark_path), "%s/%s", getenv("HOME"), BM_FILENAME); */
    Nob_String_Builder bookmark_path = get_bookmark_path();
    FILE *bookmark_file = fopen(bookmark_path.items, mode);
    if (bookmark_file == NULL) {
	fprintf(stderr, "Error: Could not open bookmarks file '%s'.\n", bookmark_path.items);
    }
    nob_sb_free(bookmark_path);
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

    Nob_String_Builder bookmark_path = get_bookmark_path();
    if (!nob_read_entire_file(bookmark_path.items, &file_sb)) return bookmarks;

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
    nob_sb_free(bookmark_path);
    nob_sb_free(file_sb);
    return bookmarks;
}

/**
 * @brief Loop through bookmark items and free them
 * @param bookmarks_ptr A pointer to a bookmarks struct to free
 */
void free_bookmarks(Bookmarks *bookmarks_ptr) {
    if (bookmarks_ptr == NULL || bookmarks_ptr->items == NULL) {
	// It's already free or never allocated.
	return;
    }
    // Iterate through each Nob_String_Builder item in the array.
    for (size_t i = 0; i < bookmarks_ptr->count; ++i) {
	nob_sb_free(bookmarks_ptr->items[i]);
    }
    // Free the main array buffer
    free(bookmarks_ptr->items);

    // Reset structure members to 0 values
    bookmarks_ptr->items = NULL;
    bookmarks_ptr->count = 0;
    bookmarks_ptr->capacity = 0;
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
int list_bookmarks(void) {
    Bookmarks bookmarks = get_bookmarks_reversed();
    for (size_t i = 0; i < bookmarks.count; i++) {
	printf("%s\n", bookmarks.items[i].items);
    }
    free_bookmarks(&bookmarks);
    return 0;
}

/**
 * @brief Pop bookmark off reversed list.
 * @details Pop the top bookmark and write the rest back to the file.
 * @return The bookmark path
 */
char *pop(void) {
    Bookmarks bookmarks = get_bookmarks_reversed();
    char *popped_path = NULL;
    if (bookmarks.items == NULL || bookmarks.count == 0) {
	free_bookmarks(&bookmarks);
	return NULL;
    }
    Nob_String_Builder bm_sb = {0};
    bm_sb = bookmarks.items[0];
    Nob_String_View bm_sv = nob_sb_to_sv(bm_sb);
    nob_sv_chop_by_delim(&bm_sv, '|');
    /* bookmarks.count--; */
    Nob_String_Builder bookmark_path = get_bookmark_path();
    Nob_String_Builder bookmarks_sb = {0};
    /* Write them back in reversed order because that's the original order */
    for (size_t i = bookmarks.count - 1; i > 0; i--) {
	nob_sb_append_buf(&bookmarks_sb, bookmarks.items[i].items, bookmarks.items[i].count);
	nob_sb_append_cstr(&bookmarks_sb, "\n");
    }
    int result = nob_write_entire_file(bookmark_path.items, bookmarks_sb.items, bookmarks_sb.count);
    popped_path = strndup(bm_sv.data, bm_sv.count);
    if (popped_path == NULL) {
	free_bookmarks(&bookmarks);
	return NULL;
    }
    nob_sb_free(bookmark_path);
    nob_sb_free(bookmarks_sb);
    free_bookmarks(&bookmarks);
    if (!result) {
	free(popped_path);
	return NULL;
    }
    return popped_path;
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
char *find(const char *needle) {
    // Search from the bottom. So, it's the last one added.
    Bookmarks bookmarks = get_bookmarks_reversed();
    char *found_path = NULL;

    if (bookmarks.items == NULL || bookmarks.count == 0) {
	free_bookmarks(&bookmarks);
	return NULL;
    }
    // Simple linear search. Items can repeat. We get the last one added.
    for (size_t i = 0; i < bookmarks.count; i++) {
	Nob_String_Builder entry_sb = bookmarks.items[i];
	Nob_String_View entry_sv = nob_sb_to_sv(entry_sb);
	Nob_String_View name_sv = nob_sv_chop_by_delim(&entry_sv, '|');

	Nob_String_Builder name_sb = {0};
	nob_sb_append_buf(&name_sb, name_sv.data, name_sv.count);
	nob_sb_append_null(&name_sb);
	size_t name_sb_len = strlen(name_sb.items);
	// name_sv should now contain the part before '|'
	if (strncmp(needle, name_sb.items, name_sb_len) == 0 && name_sb_len == strlen(needle)) {
	    // We're done with name_sb
	    nob_sb_free(name_sb);
	    found_path = strndup(entry_sv.data, entry_sv.count);
	}
    }
    free_bookmarks(&bookmarks);
    return found_path;
}
