#define NOB_IMPLEMENTATION
#include "nob.h"
#include <limits.h>
#include <stdbool.h>

#define DELIM "|"
#define BM_FILENAME ".wdc"
#define MAX_LINE_LENGTH (PATH_MAX + 50)

typedef struct {
    Nob_String_Builder *items;
    size_t count;
    size_t capacity;
} Bookmarks;

int main(void) {
    bool result = 0;
    Bookmarks bookmarks = {0};
    Nob_String_Builder file_sb = {0};

    char bookmark_path[PATH_MAX];
    snprintf(bookmark_path, sizeof(bookmark_path),
	     "%s/%s", getenv("HOME"), BM_FILENAME);
    if (!nob_read_entire_file(bookmark_path, &file_sb)) nob_return_defer(false);

    Nob_String_View content = {
	.data = file_sb.items,
	.count = file_sb.count
    };

    while (content.count > 0) {
	Nob_String_Builder bookmark = {0};
	Nob_String_View line = nob_sv_chop_by_delim(&content, '\n');
	/* nob_log(NOB_INFO, "`"SV_Fmt"`", SV_Arg(line)); */
	nob_sb_append_buf(&bookmark, line.data, line.count);
	nob_sb_append_null(&bookmark);
	nob_da_append(&bookmarks, bookmark);
    }
    nob_log(NOB_INFO, "%zu", bookmarks.count);
    for (size_t i = 0; i < bookmarks.count; i++) {
	nob_log(NOB_INFO, "%s\n", bookmarks.items[i].items);
    }

 defer:
    nob_sb_free(file_sb);
    return result;
    // nob_nob_read_entire_file will read the file into one long string builder.
    
    /* if (nob_read_entire_file(bookmark_path, &file_chars)) { */
	
	/* char *current_pos = file_chars.items; */
	/* char *line_start = file_chars.items; */

	/* while (current_pos < file_chars.items + file_chars.count) { */
	/*     if (*current_pos == '\n') { */
	/* 	Nob_String_Builder line_sb = {0}; */
	/* 	// The size is where we are at, '\n', back to the start of the line. */
	/* 	size_t line_len = current_pos - line_start; */
	/* 	// Add from start to '\n' to the line_sb. */
	/* 	nob_sb_append_buf(&line_sb, line_start, line_len); */
	/* 	// Add null to make it a cstr. */
	/* 	nob_sb_append_null(&line_sb); */
	/* 	// Append the line to the list of bookmarks. */
	/* 	nob_da_append(&bookmarks, line_sb); */
	/* 	line_start = current_pos + 1; */
	/*     } */
	/*     current_pos++; */
	/* } */
	// Handle the last line/rest of the data if it doesn't end with a newline
	// If the start of the line is still less than the total size of the file
	// (there's still more data to read).
	/* if (line_start < file_chars.items + file_chars.count) { */
	/*     Nob_String_Builder line_sb = {0}; */
	/*     size_t line_len = (file_chars.items + file_chars.count) - line_start; */
	/*     nob_sb_append_buf(&line_sb, line_start, line_len); */
	/*     nob_sb_append_null(&line_sb); */
	/*     nob_da_append(&bookmarks, line_sb); */
	/* } */
 /* } */
    /* nob_sb_free(file_chars); */
    return 0;
}
