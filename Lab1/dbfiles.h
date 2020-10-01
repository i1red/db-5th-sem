//
// Created by ivan on 9/30/20.
//

#ifndef LAB1_DBFILES_H
#define LAB1_DBFILES_H

#include <stdio.h>
#include <stdbool.h>

#include "settings.h"

FILE* promotionIndexFile = NULL;
FILE* promotionCounterFile = NULL;
FILE* promotionDataFile = NULL;

FILE* weightClassCounterFile = NULL;
FILE* weightClassDataFile = NULL;


int promotionExistingRecordsCount;
int promotionTotalRecordsCount;

int weightClassExistingRecordsCount;
int weightClassTotalRecordsCount;


bool openFiles() {
    promotionIndexFile = fopen(PROMOTION_INDEX, "r+b");
    promotionCounterFile = fopen(PROMOTION_COUNTER, "r+b");
    promotionDataFile = fopen(PROMOTION_DATA, "r+b");

    weightClassCounterFile = fopen(WEIGHT_CLASS_COUNTER, "r+b");
    weightClassDataFile = fopen(WEIGHT_CLASS_DATA, "r+b");

    return (
            promotionIndexFile != NULL &&
            promotionCounterFile != NULL &&
            promotionDataFile != NULL &&

            weightClassCounterFile != NULL &&
            weightClassDataFile != NULL
    );
}


void initCount(FILE* counter, int* existing, int* total) {
    fseek(counter, 0, SEEK_SET);
    fread(existing, sizeof(int), 1, counter);
    fread(total, sizeof(int), 1, counter);
}


void updateCount(FILE* counter, int* existing, int* total) {
    fseek(counter, 0, SEEK_SET);
    fwrite(existing, sizeof(int), 1, counter);
    fwrite(total, sizeof(int), 1, counter);
}

void updatePromotionCounts() {
    updateCount(promotionCounterFile, &promotionExistingRecordsCount, &promotionTotalRecordsCount);
}

void updateWeightClassCounts() {
    updateCount(weightClassCounterFile, &weightClassExistingRecordsCount, &weightClassTotalRecordsCount);
}

void initCounts() {
    initCount(promotionCounterFile, &promotionExistingRecordsCount, &promotionTotalRecordsCount);
    initCount(weightClassCounterFile, &weightClassExistingRecordsCount, &weightClassTotalRecordsCount);
}


void closeIfNotNull(FILE* p) {
    if (p != NULL) {
        fclose(p);
    }
}


void closeFiles() {
    closeIfNotNull(promotionIndexFile);
    closeIfNotNull(promotionCounterFile);
    closeIfNotNull(promotionDataFile);

    closeIfNotNull(weightClassCounterFile);
    closeIfNotNull(weightClassDataFile);

}

#endif //LAB1_DBFILES_H
