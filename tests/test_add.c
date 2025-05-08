#include <stdlib.h>
#include "greatest.h"
#include "../src/wdc.h"


TEST add_should_work(void) {
    // TODO: add "get_tempdir"
    char template[] = "/tmp/wdc-XXXXXX";
    char *temp_dir = mkdtemp(template);
    printf("tempdir: %s\n", temp_dir);
    size_t mark = nob_temp_save();
    const char *temp_path = nob_temp_sprintf("%s/%s", temp_dir, "test.wdc");
    printf("temp_path: %s\n", temp_path);
    setenv("WDC_BOOKMARK_FILE", temp_path, 1);
    printf("env: %s\n", getenv("WDC_BOOKMARK_FILE"));
    ASSERT_EQ(0, add("test"));
    list_bookmarks();
    nob_temp_rewind(mark);
    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_TEST(add_should_work);
    GREATEST_MAIN_END();
}
