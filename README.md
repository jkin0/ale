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

> You can find this example at [tests/build.c](./tests/build.c)

```c
#include "ale.h"

#define CC          "gcc"
#define CFLAGS      "-Wall"
#define OUTNAME     "example.out"
#define SRCS        ale_find_all_by_ext("src", "c") // find all C files in current directory

int main(void) {
    ale_cmd(5, CC, CFLAGS, "-o", OUTNAME, SRCS);
    //      ^ arg count
    return 0;
}
```

3. Finally, compile the C build file and run the executable:

Assuming the file is called ```build.c```:

```sh
$ cc build.c -o build
$ ./build
```

### Functions

**Note:** you are welcome to modify ale and create your own forks to your own liking.

Here are the functions that come premade:

```ale_find_all(char *dir)``` - Find all files in the specified directory, not including subdirectories

```ale_find_all_by_ext(char *dir, char *ext)``` - Find all files in the specified directory with the specified extension

```ale_set_verbose()``` - (enabled by default) Print each command before executing

```ale_set_noverbose()``` - DO NOT print each command before executing

## License

[MIT License](./LICENSE).
