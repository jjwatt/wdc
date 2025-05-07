#include <stdlib.h>
#include <limits.h>
#include "greatest.h"
#include "../src/wdc.h"

TEST get_bookmark_path_should_return_path(void) {
    Nob_String_Builder sb = get_bookmark_path();
    printf("%s\n", sb.items);
    char *home_dir = getenv("HOME");
    char buf[PATH_MAX];
    snprintf(buf, PATH_MAX, "%s/%s", home_dir, BM_FILENAME);
    printf("sb.count: %zu\n", sb.count);
    printf("strlen(sb.items): %lu\n", strlen(sb.items));
    printf("strlen(buf): %lu\n", strlen(buf));
    ASSERT_STR_EQ(buf, sb.items);
    sb.count = 0;
    setenv("WDC_BOOKMARK_FILE", "/tmp/test.wdc", 1);
    sb = get_bookmark_path();
    printf("%s\n", sb.items);
    PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_TEST(get_bookmark_path_should_return_path);
  GREATEST_MAIN_END();
}
