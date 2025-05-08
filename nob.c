#define NOB_IMPLEMENTATION

#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define TESTS_FOLDER "tests/"

const char *test_names[] = {
    "test_add"
};

bool build_wdc_lib(Nob_Cmd *cmd) {
    nob_cmd_append(cmd, "cc", "-Wall", "-Wextra", "-c", SRC_FOLDER"wdc.c", "-o", BUILD_FOLDER"wdc.o");
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    /* Create static lib */
    nob_cmd_append(cmd, "ar", "rcs", BUILD_FOLDER"libwdc.a", BUILD_FOLDER"wdc.o");
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    return true;
}

bool build_wdc_main(Nob_Cmd *cmd) {
    // TODO(jjwatt): Call build_wdc_lib if we need to?
    /* Link static library with main */
    nob_cmd_append(cmd, "cc", "-Wall", "-Wextra", SRC_FOLDER"main.c", "-o", BUILD_FOLDER"wdc", "-L"BUILD_FOLDER, "-lwdc");
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    return true;
}

bool build_and_run_test(Nob_Cmd *cmd, const char *test_name) {
    size_t mark = nob_temp_save();
    const char *bin_path = nob_temp_sprintf("%s%s", BUILD_FOLDER, test_name);
    const char *src_path = nob_temp_sprintf("%s%s.c", TESTS_FOLDER, test_name);

    build_wdc_lib(cmd);
    nob_cmd_append(cmd, "cc", "-Wall", "-Wextra", src_path, "-o", bin_path, "-L"BUILD_FOLDER, "-lwdc");
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    nob_cmd_append(cmd, bin_path);
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    return true;
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    Nob_Cmd cmd = {0};
    /* /\* Build the lib object *\/ */
    /* nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-c", SRC_FOLDER"wdc.c", "-o", BUILD_FOLDER"wdc.o"); */
    /* if (!nob_cmd_run_sync_and_reset(&cmd)) return 1; */
    build_wdc_lib(&cmd);
    build_wdc_main(&cmd);
    /* build_and_run_test(&cmd, "test_get_bookmark_path"); */
    build_and_run_test(&cmd, "test_add_and_get_bookmarks");
    return 0;
}
