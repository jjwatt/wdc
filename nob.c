/* -*- mode: c; c-basic-offset: 4 -*- */
#define NOB_IMPLEMENTATION

#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"
#define TESTS_FOLDER "tests/"

const char *test_names[] = {
    "test_add"
};

bool build_wdc_lib(Nob_Cmd *cmd) {
    const char *bin_path = BUILD_FOLDER"wdc.o";
    const char *src_path = SRC_FOLDER"wdc.c";
    Nob_File_Paths source_paths = {0};
    nob_da_append(&source_paths, src_path);
    int rebuild_is_needed = nob_needs_rebuild(bin_path, source_paths.items, source_paths.count);
    if (rebuild_is_needed < 0) return false;
    if (!rebuild_is_needed) {
	nob_da_free(source_paths);
	return true;
    }
    nob_cmd_append(cmd, "cc", "-Wall", "-Wextra","-fsanitize=address", "-g", "-c", src_path, "-o", bin_path);
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    /* Create static lib */
    nob_cmd_append(cmd, "ar", "rcs", BUILD_FOLDER"libwdc.a", bin_path);
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    nob_da_free(source_paths);
    return true;
}

bool build_wdc_main(Nob_Cmd *cmd) {
    // Build lib if it's needed
    build_wdc_lib(cmd);
    const char *bin_path = BUILD_FOLDER"wdc";
    const char *src_path = SRC_FOLDER"main.c";
    Nob_File_Paths source_paths = {0};
    nob_da_append(&source_paths, src_path);
    // Rebuild if lib changed, too.
    nob_da_append(&source_paths, SRC_FOLDER"wdc.c");
    int rebuild_is_needed = nob_needs_rebuild(bin_path, source_paths.items, source_paths.count);
    if (rebuild_is_needed < 0) return false;
    if (!rebuild_is_needed) {
	nob_da_free(source_paths);
	return true;
    }
    /* Link static library with main */
    nob_cmd_append(cmd, "cc", "-Wall", "-Wextra", "-fsanitize=address", "-g", src_path, "-o", bin_path, "-L"BUILD_FOLDER, "-lwdc");
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    nob_da_free(source_paths);
    return true;
}

bool build_and_run_test(Nob_Cmd *cmd, const char *test_name) {
    size_t mark = nob_temp_save();
    const char *bin_path = nob_temp_sprintf("%s%s", BUILD_FOLDER, test_name);
    const char *src_path = nob_temp_sprintf("%s%s.c", TESTS_FOLDER, test_name);

    build_wdc_lib(cmd);
    nob_cmd_append(cmd, "cc", "-Wall", "-Wextra", "-fsanitize=address", "-g", src_path, "-o", bin_path, "-L"BUILD_FOLDER, "-lwdc");
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    nob_cmd_append(cmd, bin_path);
    if (!nob_cmd_run_sync_and_reset(cmd)) return false;
    nob_temp_rewind(mark);
    return true;
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    Nob_Cmd cmd = {0};
    /* Build the lib */
    if (!build_wdc_lib(&cmd)) return 1;
    if (!build_wdc_main(&cmd)) return 1;
    if (!build_and_run_test(&cmd, "test_add_and_get_bookmarks")) return 1; 
    if (!build_and_run_test(&cmd, "test_get_bookmark_path")) return 1;
   return 0;
}
