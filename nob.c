#define NOB_IMPLEMENTATION

#include "nob.h"

#define BUILD_FOLDER "build/"
#define SRC_FOLDER "src/"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!nob_mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    Nob_Cmd cmd = {0};
    /* nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-o", BUILD_FOLDER"wdc", SRC_FOLDER"wdc.c", SRC_FOLDER"main.c"); */
    /* Build the lib object */
    nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", "-c", SRC_FOLDER"wdc.c", "-o", BUILD_FOLDER"wdc.o");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;
    /* Create static lib */
    nob_cmd_append(&cmd, "ar", "rcs", BUILD_FOLDER"libwdc.a", BUILD_FOLDER"wdc.o");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;
    /* Link static library with main */
    nob_cmd_append(&cmd, "cc", "-Wall", "-Wextra", SRC_FOLDER"main.c", "-o", BUILD_FOLDER"wdc", "-L"BUILD_FOLDER, "-lwdc");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;
    return 0;
}
