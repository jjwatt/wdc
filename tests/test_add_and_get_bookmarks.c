#include <stdlib.h>
#include "greatest.h"
#include "../src/wdc.h"


TEST add_should_add_to_file(void) {
    char template[] = "/tmp/wdc-XXXXXX";
    char *temp_dir = mkdtemp(template);
    size_t mark = nob_temp_save();
    const char *temp_path = nob_temp_sprintf("%s/%s", temp_dir, "test.wdc");
    setenv("WDC_BOOKMARK_FILE", temp_path, 1);
    printf("env: %s\n", getenv("WDC_BOOKMARK_FILE"));
    ASSERT_EQ(0, add("test"));
    Bookmarks bms = get_bookmarks();
    printf("bookmark: %s", bms.items[0].items);
    rmdir(temp_path);
    nob_temp_rewind(mark);
    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_TEST(add_should_add_to_file);
    GREATEST_MAIN_END();
}
