#include "greatest.h"
#include "../src/wdc.h"

// TODO: Use mktemp
#define BM_FILENAME "/tmp/test.wdc"

TEST add_should_work(void) {
  ASSERT_EQ(0, add("test"));
  PASS();
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv) {
  GREATEST_MAIN_BEGIN();
  RUN_TEST(add_should_work);
  GREATEST_MAIN_END();
}
