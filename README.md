# ale

Zero-dependency build tool in pure C.

## About

Ale is a small zero-dependency build tool written in C, just for fun. All you will need is a C compiler for building your projects with this tool, no more make and cmake. It is called Ale because I would frequently mistype 'make' as 'ale', now I couldn't think of a more suitable name for a build tool.

## Usage

1. Create a file for your build process, maybe call it ```build.c```, and include the header file:

```c
#include "ale.h"
```

2. Then write your build process, you can see the [functions](#functions) section to see functions available to you to make it easier.

Here is an example build process:

```c
#include "ale.h"

int main(void) {
    // compile all C files in directory 'src' to a.out
    ale_cmd(2, "gcc -Wall -o a.out", ale_find_all_by_ext("src", "c"));
    //      ^ arg count
    return 0;
}
```

3. Finally, compile the C build file and run the executable:

Assuming the file is called ```build.c```:

```sh
$ cc build.c -o build.out
$ ./build.out
```

### Example

In the [example](./example/) directory, I ported my go-to makefile script to ale:

#### make

```make
CC := gcc
CFLAGS := -Wall -Wextra -std=c18 -g

SRC_DIR := src
BUILD_DIR := build
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

OUT := main.out 

.PHONY: all clean

all: $(OUT)

$(OUT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OUT)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(OUT)
```

#### ale

```c
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
```

### Functions

**Note:** you are welcome to modify ale and create your own forks to your own liking.

Here are the functions that come premade:

```void ale_cmd(size_t n, ...)``` - Append each string passed, ```n``` is the argument count

```char *ale_find_all(char *dir)``` - Find all files in the specified directory, not including subdirectories, also **creates a collection of the found files**

```char *ale_find_all_by_ext(char *dir, char *ext)``` - Find all files in the specified directory with the specified extension, also **creates a collection of the found files**

```void ale_set_verbose(int level)``` - (```ALE_VERBOSE_ALL``` by default) Tweak how many commands are printed before executing, ```ALE_VERBOSE_ALL``` prints all commands, ```ALE_VERBOSE_LESS``` prints most commands, ```ALE_VERBOSE_NONE``` doesn't print any

```int ale_file_exists(char *name)``` - Search for the specified file, return 1 if it exists, return 0 if not

```size_t ale_count_files(char *dir)``` - Return the count of files in the specified directory

```size_t ale_count_file_by_ext(char *dir)``` - Return the count of files that have the specified extension in the specified directory

```void ale_copy_file(char *filename, char *dst_name)``` - Copy the specified file and call it ```dst_name```

```char *ale_change_filetype(char *filename, char *new_type, int rename_file)``` - Return the filename with the new extension, if ```rename_file``` is 1, change the filetype of the specified file to the specified new type

```char *ale_move_file_to_dir(char *file, char *dir, int move_file)``` - Return the new path of the file after moved, if ```move_file``` is 1, move the file to the directory

```void ale_move_file(char *file, char *path)``` - Move the specified file to the specified directory

```size_t ale_collect_size()``` - Return the size of the collection

```char *ale_next_in_collect()``` - Return the next file in the collection

## License

[MIT License](./LICENSE).
