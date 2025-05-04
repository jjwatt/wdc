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
    nob_log(NOB_INFO, "%zu", bookmarks.count);
    nob_sb_free(file_sb);
    return bookmarks;
}

int main(void) {
    Bookmarks bookmarks = get_bookmarks();
    for (size_t i = 0; i < bookmarks.count; i++) {
	printf("%s\n", bookmarks.items[i].items);
	/* nob_log(NOB_INFO, "%s\n", bookmarks.items[i].items); */
    }
    nob_da_free(bookmarks);
    return 0;
}
