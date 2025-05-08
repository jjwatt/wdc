#include <stdlib.h>
#include "greatest.h"
#include "../src/wdc.h"

TEST add_should_work(void) {
    // TODO: use mkdtemp
    char template[] = "/tmp/wdc-XXXXXX";
    char *tempdir = mkdtemp(template);
    printf("tempdir: %s\n", tempdir);
    size_t mark = nob_temp_save();
    const char *temp_path = nob_temp_sprintf("%s/%s", tempdir, "test.wdc");
    printf("temp_path: %s\n", temp_path);
    setenv("WDC_BOOKMARK_FILE", temp_path, 1);
    ASSERT_EQ(0, add("test"));
    nob_temp_rewind(mark);
    PASS();
}

TEST list_bookmarks_should_work(void) {
    setenv("WDC_BOOKMARK_FILE", "/tmp/test.wdc", 1);
    Nob_String_Builder sb;
    sb = get_bookmark_path();
    printf("bookmark path: %s\n", sb.items);
    list_bookmarks();
    PASS();
}


GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_TEST(add_should_work);
    RUN_TEST(list_bookmarks_should_work);
    GREATEST_MAIN_END();
}
