#include "greatest.h"
#include "../src/wdc.h"

TEST add_should_work(void) {
    // TODO: use mktemp
    setenv("WDC_BOOKMARK_FILE", "/tmp/test.wdc", 1);
    ASSERT_EQ(0, add("test"));
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
