//
// Created by ivan on 9/30/20.
//

#ifndef LAB1_UTILS_H
#define LAB1_UTILS_H

#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>

#include "settings.h"


bool pathExists(const char* path) {
    struct stat st = {0};
    return stat(path, &st) != -1;
}


bool createDirIfDoesNotExist(const char* path) {
    if (!pathExists(path)) {
        if (mkdir(path, 0777) == 0) {
            return true;
        }
        fprintf(stderr, "ERROR: Failed to create %s directory\n", path);
        return false;
    }
    return true;
}


bool createFileIfDoesNotExist(const char* path, bool counter) {
    if (!pathExists(path)) {
        FILE* fp = fopen(path, "wb");
        if (fp != NULL) {
            if (counter) {
                int counts[2] = {0, 0};
                fwrite(counts, sizeof(counts), 1, fp);
            }
            fclose(fp);
            return true;
        }
        fprintf(stderr, "ERROR: Failed to create %s file\n", path);
        printf("%d\n", errno);
        return false;
    }
    return true;
}


bool databaseOnStart() {
    return (
            createDirIfDoesNotExist(DATAFILES) &&

            createDirIfDoesNotExist(PROMOTION_TABLE) &&
            createFileIfDoesNotExist(PROMOTION_INDEX, false) &&
            createFileIfDoesNotExist(PROMOTION_COUNTER, true) &&
            createFileIfDoesNotExist(PROMOTION_DATA, false) &&

            createDirIfDoesNotExist(WEIGHT_CLASS_TABLE) &&
            createFileIfDoesNotExist(WEIGHT_CLASS_COUNTER, true) &&
            createFileIfDoesNotExist(WEIGHT_CLASS_DATA, false)
    );
}


#endif //LAB1_UTILS_H
