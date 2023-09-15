#include "../ale.h"

#define CC          "gcc"
#define CFLAGS      "-Wall"
#define OUTNAME     "example.out"
#define SRCS        ale_find_all_by_ext("src", "c") // find all C files in the current directory

int main(void) {
   ale_cmd(5, CC, CFLAGS, "-o", OUTNAME, SRCS);
    //      ^ arg count
    return 0;
}
