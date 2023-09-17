#include "../ale.h"
#include <stdio.h>
#include <string.h>

#define CC        "gcc"
#define CFLAGS    "-Wall -Wextra -std=c18 -g"
#define SRC_DIR   "src"
#define BUILD_DIR "build"
#define OUT       "main.out"

int main(int argc, char **argv) {
    ale_set_verbose(ALE_VERBOSE_LESS);

    if (argc > 1 && strcmp(argv[1], "clean") == 0) {
        ale_cmd(1, "rm -rf", BUILD_DIR);
        return 0;
    }

    ale_cmd(2, "mkdir -p", BUILD_DIR);                      // create build dir
    ale_find_all_by_ext(SRC_DIR, "c");                      // find all C files and store in the collection

    for (size_t i = 0; i < ale_collect_size(); i++) {
        char *file = ale_next_in_collect();                 // get each C file
        char *obj_file = ale_change_filetype(file, "o", 0);

        ale_cmd(6, CC, CFLAGS, "-c", file, "-o", obj_file); // compile each C file to O file
        ale_move_file_to_dir(obj_file, BUILD_DIR, 1);       // move to build dir
    }

    // compile all O files in the build dir to an executable
    ale_cmd(5, CC, CFLAGS, ale_find_all_by_ext(BUILD_DIR, "o"), "-o", OUT);
    return 0;
}
