/*
 * ale.h - A zero-dependency build system in pure C.
 * MIT Copyright (c) 2023 Joshua Kinder
 */
#ifndef ALE_H
#define ALE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <dirent.h>

#define ALE_VERBOSE_ALL  2
#define ALE_VERBOSE_LESS 1
#define ALE_VERBOSE_NONE 0
#define ALE_DFLT_VERBOSE ALE_VERBOSE_LESS

int verbose = ALE_VERBOSE_ALL;
int cmd_verbose_level = ALE_DFLT_VERBOSE;

char **collection = NULL;
size_t collection_cnt = 0;
size_t cur_collection_i = 0;

char *_mstrnew(char *src) {
    char *str = (char *)calloc(strlen(src) + 1, sizeof(char));
    strcpy(str, src);
    return str;
}

void _ale_clear_collect() {
    for (size_t i = 0; i < collection_cnt; i++)
        free(collection[i]);
    free(collection);
    collection = NULL;
    collection_cnt = 0;
    cur_collection_i = 0;
}

void _ale_prep_collect(int reset) {
    if (reset == 1)
        _ale_clear_collect();

    if (collection == NULL) {
        collection = (char **)calloc(1, sizeof(char *));
        collection_cnt = 0;
        cur_collection_i = 0;
    }
}

void _ale_add_to_collect(char *add) {
    _ale_prep_collect(0);
    assert(add != NULL);
    collection = (char **)realloc(collection, ++collection_cnt * sizeof(char *));
    assert(collection != NULL && "error: ale: _ale_add_to_collect(): could not allocate enough memory to add to collection");
    collection[collection_cnt - 1] = _mstrnew(add);
}

size_t ale_collect_size() {
    return collection_cnt;
}

char *ale_next_in_collect() {
    if (collection == NULL || collection_cnt == 0)
        return NULL;
    return collection[cur_collection_i++];
}

void _ale_system(size_t n, ...) {
}

void ale_cmd(size_t n, ...) {
    va_list vl;
    va_start(vl, n);

    char *cmd = (char *)calloc(1, sizeof(char));

    for (size_t i = 0; i < n; i++) {
        char *arg = va_arg(vl, char *);
        assert(arg != NULL);
        cmd = (char *)realloc(cmd, (strlen(cmd) + strlen(arg) + 2) * sizeof(char));
        if (i == 0)
            strcpy(cmd, arg);
        else {
            strcat(cmd, " ");
            strcat(cmd, arg);
        }
    }

    if (verbose >= cmd_verbose_level)
        printf("%s\n", cmd);

    system(cmd);
    free(cmd);

    cmd_verbose_level = ALE_DFLT_VERBOSE;

    va_end(vl);
}

char *ale_find_all(char *dir) {
    _ale_prep_collect(1);
    assert(dir != NULL);
    char *all = (char *)calloc(1, sizeof(char));
    size_t cnt = 0;

    char *dir_cpy = (char *)calloc(strlen(dir) + 2, sizeof(char));
    if (dir[strlen(dir) - 1] != '/')
        sprintf(dir_cpy, "%s/", dir);
    else
        strcpy(dir_cpy, dir);

    DIR *d;
    struct dirent *diren;
    d = opendir(dir);
    if (d) {
        while ((diren = readdir(d)) != NULL) {
            if (diren->d_type == DT_REG) {
                _ale_add_to_collect(diren->d_name);
                all = (char *)realloc(all, (strlen(all) + strlen(dir_cpy) + strlen(diren->d_name) + 2) * sizeof(char));

                if (cnt == 0)
                    sprintf(all, "%s%s", dir_cpy, diren->d_name);
                else {
                    strcat(all, " ");
                    strcat(all, dir_cpy);
                    strcat(all, diren->d_name);
                }
                cnt++;
            }
        }
        closedir(d);
    }
    else {
        fprintf(stderr, "error: ale_find_all(): unknown directory '%s'\n", dir);
        exit(EXIT_FAILURE);
    }

    free(dir_cpy);
    return all;
}

char *ale_find_all_by_ext(char *dir, char *ext) {
    _ale_prep_collect(1);
    assert(dir != NULL);
    assert(ext != NULL);

    char *all = ale_find_all(dir);
    _ale_clear_collect();

    char *wanted = (char *)calloc(1, sizeof(char));

    size_t cnt = 0;
    char *tok = strtok(all, " ");
    while (tok != NULL) {
        char *file_cpy = (char *)calloc(strlen(tok) + 1, sizeof(char));
        strcpy(file_cpy, tok);

        size_t period_pos;
        int period_not_found = 1;
        for (size_t i = 0; i < strlen(file_cpy); i++) {
            if (file_cpy[i] == '.') {
                period_pos = i;
                period_not_found = 0;
                break;
            }
        }

        if (period_not_found == 0) {
            char *file_ext = (char *)calloc(1, sizeof(char));
            size_t file_ext_size = 0;
            for (size_t i = period_pos + 1; i < strlen(file_cpy); i++) {
                file_ext[file_ext_size++] = file_cpy[i];
                file_ext = (char *)realloc(file_ext, (file_ext_size + 1) * sizeof(char));
            }

            assert(file_ext != NULL);
            if (strcmp(file_ext, ext) == 0) {
                _ale_add_to_collect(tok);

                wanted = (char *)realloc(wanted, (strlen(wanted) + strlen(tok) + 2) * sizeof(char));
                if (cnt == 0)
                    strcpy(wanted, tok);
                else {
                    strcat(wanted, " ");
                    strcat(wanted, tok);
                }
                cnt++;
            }
            free(file_ext);
        }

        free(file_cpy);
        tok = strtok(NULL, " ");
    }

    free(all);
    return wanted;
}

void ale_set_verbose(int level) {
    verbose = level;
}

int ale_file_exists(char *name) {
    FILE *fp = fopen(name, "r");
    if (fp == NULL)
        return 0;
    fclose(fp);
    return 1;
}

size_t ale_count_files(char *dir) {
    char *files = ale_find_all(dir);
    size_t cnt = 0;

    char *tok = strtok(files, " ");
    while (tok != NULL) {
        cnt++;
        tok = strtok(NULL, " ");
    }

    cnt++;
    free(files);
    return cnt;
}

size_t ale_count_files_by_ext(char *dir, char *ext) {
    char *files = ale_find_all_by_ext(dir, ext);
    size_t cnt = 0;

    char *tok = strtok(files, " ");
    while (tok != NULL) {
        cnt++;
        tok = strtok(NULL, " ");
    }

    cnt++;
    free(files);
    return cnt;
}

void ale_copy_file(char *filename, char *dst_name) {
    assert(filename != NULL);
    assert(dst_name != NULL);
    cmd_verbose_level = ALE_VERBOSE_ALL;
    ale_cmd(3, "cp", filename, dst_name);
}

char *ale_change_filetype(char *filename, char *new_type, int rename_file) {
    char *name = _mstrnew(filename);
    char *tok = strtok(name, ".");
    if (tok == NULL)
        tok = filename;

    char *new_name = (char *)calloc(strlen(tok) + strlen(new_type) + 2, sizeof(char));
    sprintf(new_name, "%s.%s", tok, new_type);

    if (rename_file == 1)
        rename(filename, new_name);
    free(name);
    return new_name;
}

char *ale_move_file_to_dir(char *file, char *dir, int move_file) {
    assert(file != NULL);
    assert(dir != NULL);
    char *path = _mstrnew(file);
    char *base;
    char *tok = strtok(path, "/");

    if (tok == NULL)
        base = file;
    while (tok != NULL) {
        base = tok;
        tok = strtok(NULL, "/");
    }

    char *new_path = (char *)calloc(strlen(dir) + strlen(base) + 2, sizeof(char));
    if (dir[strlen(dir) - 1] != '/')
        sprintf(new_path, "%s/%s", dir, base);
    else
        sprintf(new_path, "%s%s", dir, base);

    if (move_file == 1) {
        cmd_verbose_level = ALE_VERBOSE_ALL;
        ale_cmd(3, "mv", file, new_path);
    }

    return new_path;
}

void ale_move_file(char *file, char *path) {
    assert(file != NULL);
    assert(path != NULL);
    cmd_verbose_level = ALE_VERBOSE_ALL;
    ale_cmd(3, "mv", file, path);
}

#endif
