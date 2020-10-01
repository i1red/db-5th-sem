//
// Created by ivan on 9/30/20.
//

#ifndef LAB1_MODELS_H
#define LAB1_MODELS_H

#include <stdbool.h>
#include <string.h>
#include "settings.h"

#define NAME_LENGTH 32

typedef struct Promotion {
    int id;
    char name[NAME_LENGTH];
    int weightClassAddress;
    bool isDeleted;
} Promotion;


Promotion createPromotion(int id, const char name[NAME_LENGTH]) {
    Promotion newPromotion;

    newPromotion.id = id;
    strcpy(newPromotion.name, name);
    newPromotion.weightClassAddress = -1;
    newPromotion.isDeleted = false;

    return newPromotion;
}


void printPromotion(Promotion promotion) {
    printf("Promotion { id = %d, name = %s }\n", promotion.id, promotion.name);
}


typedef struct WeightClass {
    int id;
    int promotionId;
    char name[NAME_LENGTH];
    int weightLimit;
    int nextWeightClassAddress;
    bool isDeleted;
} WeightClass;


WeightClass createWeightClass(int id, int promotionId, const char name[NAME_LENGTH], int weightLimit) {
    WeightClass newWeightClass;

    newWeightClass.id = id;
    newWeightClass.promotionId = promotionId;
    strcpy(newWeightClass.name, name);
    newWeightClass.weightLimit = weightLimit;
    newWeightClass.nextWeightClassAddress = -1;
    newWeightClass.isDeleted = false;

    return newWeightClass;
}


void printWeightClass(WeightClass weightClass) {
    printf("WeightClass { id = %d, promotionId = %d, name = %s, weightLimit = %d }\n",
            weightClass.id, weightClass.promotionId, weightClass.name, weightClass.weightLimit);
}

Promotion PROMOTION_SENTINEL = {-1, "PROMOTION_SENTINEL"};
WeightClass WEIGHT_CLASS_SENTINEL = {-1, -1,"WEIGHT_CLASS_SENTINEL"};


bool isPromotionSentinel(Promotion promotion) {
    return promotion.id = PROMOTION_SENTINEL.id && strcmp(promotion.name, PROMOTION_SENTINEL.name) == 0;
}

bool isWeightClassSentinel(WeightClass weightClass) {
    return weightClass.id = WEIGHT_CLASS_SENTINEL.id && strcmp(weightClass.name, WEIGHT_CLASS_SENTINEL.name) == 0;
}

#endif //LAB1_MODELS_H
