#ifndef ALE_H
#define ALE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

int verbose = 1;

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

    if (verbose == 1)
        printf("%s\n", cmd);
    system(cmd);
    free(cmd);

    va_end(vl);
}

char *ale_find_all(char *dir) {
    assert(dir != NULL);
    char *all = (char *)calloc(1, sizeof(char));
    size_t cnt = 0;

    char *dir_cpy = (char *)calloc(strlen(dir) + 2, sizeof(char));
    if (dir[strlen(dir) - 1] != '/')
        sprintf(dir_cpy, "%s/", dir);
    else
        strcpy(dir_cpy, dir);

#ifdef _WIN32
#include <windows.h>
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;

    hFind = FindFirstFile(dir, &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "error: ale_find_all(): FindFirstFile failed (%d)\n", GetLastError());
        exit(EXIT_FAILURE);
    } 
    else {
        do {
            all = (char *)realloc(all, (strlen(all) + strlen(dir_cpy) + strlen(FindFileData.cFileName) + 2) * sizeof(char));
            if (cnt == 0) {
                sprintf(all, "%s%s", dir_cpy, FindFileData.cFileName);
            }
            else {
                strcat(all, " ");
                strcat(all, dir_cpy);
                strcat(all, FindFileData.cFileName);
            }
            cnt++;
        } while (FindNextFile(hFind, &FindFileData) != 0);

        FindClose(hFind);
    }
#else
#include <dirent.h>
    DIR *d;
    struct dirent *diren;
    d = opendir(dir);
    if (d) {
        while ((diren = readdir(d)) != NULL) {
            if (diren->d_type == DT_REG) {
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
#endif 
    free(dir_cpy);
    return all;
}

char *ale_find_all_by_ext(char *dir, char *ext) {
    assert(dir != NULL);
    assert(ext != NULL);
    char *all = ale_find_all(dir);
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
                wanted = (char *)realloc(wanted, (strlen(wanted) + strlen(tok) + 1) * sizeof(char));
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

void ale_set_verbose() {
    verbose = 1;
}

void ale_set_noverbose() {
    verbose = 0;
}

#endif
