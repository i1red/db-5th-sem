#include <stdio.h>
#include <string.h>
#include <time.h>
#include <zconf.h>
#include "settings.h"
#include "utils.h"
#include "models.h"
#include "dbfiles.h"
#include "commands.h"


void fillDB() {
    for (int i = 0; i < 80; i += 1) {
        insertPromotion(createPromotion(i, "UFC"));
    }

    for (int i = 0; i < 80; i += 1) {
        insertWeightClass(createWeightClass(i, i % 10, "Welterweight", 77));
    }
}


void printGet() {
    Promotion p;
    for (int i = 0; i < 100; ++i) {
        p = getPromotion(i);
        printPromotion(p);
    }
}


void printCount() {
    printf("%d, %d\n", countPromotions(), countWeightClasses());
}


void deletePromotions() {
    deletePromotion(0);
    deletePromotion(8);
    deletePromotion(10);
    deletePromotion(80);
}


void deleteWeightClasses() {
    for (int i = 0; i < 80; i += 4) {
        deleteWeightClass(i);
    }
}


int main() {
    if(!databaseOnStart()) {
        fprintf(stderr, "%s", "ERROR: Failed to start database\n");
        return 1;
    }

    if (!openFiles()) {
        fprintf(stderr, "%s", "ERROR: Failed to open database files\n");
        closeFiles();
        return 1;
    }
    initCounts();

    //fillDB();
    //deleteWeightClasses();
    //deletePromotions();


    printPromotions();
    printWeightClasses();
    printCount();

    saveData();
    closeFiles();
    return 0;
}
