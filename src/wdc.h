#ifndef WDC_H
#define WDC_H

#include <limits.h>
#include "nob.h"

#define DELIM "|"
#define BM_FILENAME ".wdc"
#define MAX_LINE_LENGTH (PATH_MAX + 50)

typedef struct {
    Nob_String_Builder *items;
    size_t count;
    size_t capacity;
} Bookmarks;

Nob_String_Builder get_bookmark_path(void);
FILE *open_bookmark_file(const char *mode);
void add_to_file(const char *name, const char *cwd_path, FILE *bookmark_file);
int add(const char *name);
Bookmarks get_bookmarks(void);
int free_bookmarks(Bookmarks bookmarks);
Bookmarks get_bookmarks_reversed(void);
int list_bookmarks(void);
char *find(const char *name);
const char *pop(void);

#endif
