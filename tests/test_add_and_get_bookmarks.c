#include <stdlib.h>
#include <limits.h>
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
    printf("bookmark: %s\n", bms.items[0].items);
    rmdir(temp_path);
    nob_temp_rewind(mark);
    free_bookmarks(&bms);
    PASS();
}

TEST add_many_pop_many(void) {
    char template[] = "/tmp/wdc-XXXXXX";
    char *temp_dir = mkdtemp(template);
    size_t mark = nob_temp_save();
    const char *temp_path = nob_temp_sprintf("%s/%s", temp_dir, "test.wdc");
    setenv("WDC_BOOKMARK_FILE", temp_path, 1);
    // Add the same thing a bunch of times.
    size_t count = 10;
    for (size_t i = 0; i < count; i++) {
      ASSERT_EQ(0, add("test"));
    }
    for (size_t i = 0; i < count; i++) {
      char *bm_path;
      bm_path = pop();
      free(bm_path);
    }
    rmdir(temp_path);
    nob_temp_rewind(mark);
    PASS();
}

TEST pop_should_pop_entry(void) {
    char template[] = "/tmp/wdc-XXXXXX";
    char *temp_dir = mkdtemp(template);
    size_t mark = nob_temp_save();
    const char *temp_path = nob_temp_sprintf("%s/%s", temp_dir, "test.wdc");
    setenv("WDC_BOOKMARK_FILE", temp_path, 1);
    ASSERT_EQ(0, add("test"));

    // Calling add from where we are should set the bookmark path to the cwd.
    // They should match.
    char cwd_buf[PATH_MAX];
    getcwd(cwd_buf, PATH_MAX);
    /* printf("cwd_buf: %s\n", cwd_buf); */
    char *bm_path;
    bm_path = pop();
    /* printf("pop path: %s\n", temp_path); */
    /* printf("*bm_path: %s\n", bm_path); */
    ASSERT_STR_EQ(cwd_buf, bm_path);
    free(bm_path);
    rmdir(temp_path);
    nob_temp_rewind(mark);
    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
    GREATEST_MAIN_BEGIN();
    RUN_TEST(add_should_add_to_file);
    RUN_TEST(add_many_pop_many);
    RUN_TEST(pop_should_pop_entry);
    GREATEST_MAIN_END();
}
