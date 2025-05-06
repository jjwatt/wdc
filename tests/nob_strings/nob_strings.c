#define NOB_IMPLEMENTATION
#include "nob.h"
#include <limits.h>

#define DELIM "|"
#define BM_FILENAME ".wdc"
#define MAX_LINE_LENGTH (PATH_MAX + 50)

typedef struct {
    Nob_String_Builder *items;
    size_t count;
    size_t capacity;
} Bookmarks;

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

char *find(const char *needle) {
    // Search from the bottom. So, it's the last one added.
    Bookmarks bookmarks = get_bookmarks_reversed();
    char *found_path = NULL;

    // Simple linear search. Items can repeat. We get the last one added.
    nob_log(NOB_INFO, "Looking for: %s", needle);
    nob_log(NOB_INFO, "needle len: %lu", strlen(needle));
    for (size_t i = 0; i < bookmarks.count; i++) {
	Nob_String_Builder entry_sb = bookmarks.items[i];
	/* char *entry = entry_sb.items; */
	Nob_String_View entry_sv = nob_sb_to_sv(entry_sb);
	Nob_String_View name_sv = nob_sv_chop_by_delim(&entry_sv, '|');

	Nob_String_Builder name_sb = {0};
	nob_sb_append_buf(&name_sb, name_sv.data, name_sv.count);
	nob_sb_append_null(&name_sb);
	/* printf("name_sb.items: %s\n", name_sb.items); */
	/* printf("name_sb.count: %zu\n", name_sb.count); */
	/* printf("strlen(name_sb.items): %lu\n", strlen(name_sb.items)); */
	size_t name_sb_len = strlen(name_sb.items);
	// name_sv should now contain the part before '|'
	if (strncmp(needle, name_sb.items, name_sb_len) == 0 && name_sb_len == strlen(needle)) {
	    // We're done with name_sb
	    nob_sb_free(name_sb);
	    found_path = strndup(entry_sv.data, entry_sv.count);
	}
	nob_sb_free(entry_sb);
    }
    nob_da_free(bookmarks);
    return found_path;
}


int main(void) {
    /* Bookmarks bookmarks = get_bookmarks(); */
    /* for (size_t i = 0; i < bookmarks.count; i++) { */
    /* 	printf("%s\n", bookmarks.items[i].items); */
    /* 	/\* nob_log(NOB_INFO, "%s\n", bookmarks.items[i].items); *\/ */
    /* } */    
    /* nob_da_free(bookmarks); */
    char *path = find("play-shell");
    printf("path: %s\n", path);
    return 0;
}
