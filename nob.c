#define NOB_IMPLEMENTATION

#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"

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

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    Nob_Cmd cmd = {0};
    /* /\* Build the lib object *\/ */
    /* nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-c", SRC_FOLDER"wdc.c", "-o", BUILD_FOLDER"wdc.o"); */
    /* if (!nob_cmd_run_sync_and_reset(&cmd)) return 1; */
    build_wdc_lib(&cmd);

    /* Link static library with main */
    nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", SRC_FOLDER"main.c", "-o", BUILD_FOLDER"wdc", "-L"BUILD_FOLDER, "-lwdc");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;
    return 0;
}
