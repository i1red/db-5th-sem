//
// Created by ivan on 9/30/20.
//

#ifndef LAB1_COMMANDS_H
#define LAB1_COMMANDS_H

#include "models.h"
#include "dbfiles.h"
#include "indexedtable.h"


void saveData() {
    mergeTables();
}


Promotion _getPromotionByAddress(int address) {
    Promotion tmpPromotion;

    fseek(promotionDataFile, address, SEEK_SET);
    fread(&tmpPromotion, sizeof(Promotion), 1, promotionDataFile);

    return tmpPromotion;
}


void _writePromotion(Promotion promotion, int address) {
    fseek(promotionDataFile, address, SEEK_SET);
    fwrite(&promotion, sizeof(Promotion), 1, promotionDataFile);
}


bool insertPromotion(Promotion promotion) {
    int address = _getPromotionAddress(promotion.id);
    if (address != -1) {
        Promotion tmpPromotion = _getPromotionByAddress(address);
        if (!tmpPromotion.isDeleted) {
            return false;
        }

        _writePromotion(promotion, address);
        ++promotionExistingRecordsCount;
        return true;
    }

    insertPromotionIndex(promotion.id, seekEndAndReturnPos(promotionDataFile));
    fwrite(&promotion, sizeof(Promotion), 1, promotionDataFile);
    return true;
}


Promotion getPromotion(int id) {
    int address = _getPromotionAddress(id);

    if (address != -1) {
        Promotion result;
        fseek(promotionDataFile, address, SEEK_SET);
        fread(&result, sizeof(Promotion), 1, promotionDataFile);
        return !result.isDeleted ? result : PROMOTION_SENTINEL;
    }
    return PROMOTION_SENTINEL;
}


bool updatePromotion(int id, char name[NAME_LENGTH]) {
    int address = _getPromotionAddress(id);

    if (address != -1) {
        Promotion promotion = _getPromotionByAddress(address);
        if (!promotion.isDeleted) {
            strcpy(promotion.name, name);
            _writePromotion(promotion, address);
            return true;
        }
    }

    return false;
}


void printPromotions() {
    int recordSize = sizeof(Promotion);
    int endOfFile = seekEndAndReturnPos(promotionDataFile);

    Promotion promotion;
    for (int address = 0; address < endOfFile; address += recordSize) {
        fseek(promotionDataFile, address, SEEK_SET);
        fread(&promotion, recordSize, 1, promotionDataFile);
        if (!promotion.isDeleted) {
            printPromotion(promotion);
        }
    }
}


int countPromotions() {
    return promotionExistingRecordsCount;
}


int countPromotionWeightClasses(int id) {
    Promotion promotion = getPromotion(id);
    if (isPromotionSentinel(promotion)) {
        return -1;
    }
    int weightClassAddress = promotion.weightClassAddress;
    int counter = 0;

    WeightClass weightClass;
    int recordSize = sizeof(WeightClass);

    while (weightClassAddress != -1) {
        fseek(weightClassDataFile, weightClassAddress, SEEK_SET);
        fread(&weightClass, recordSize, 1, weightClassDataFile);
        weightClassAddress = weightClass.nextWeightClassAddress;
        if(!weightClass.isDeleted) {
            ++counter;
        }
    }

    return counter;
}


int _getWeightClassAddress(int id) {
    int recordSize = sizeof(WeightClass);
    long endOfFile = seekEndAndReturnPos(weightClassDataFile);

    WeightClass tmp;
    for (int address = 0; address < endOfFile; address += recordSize) {
        fseek(weightClassDataFile, 0, address);
        fread(&tmp, recordSize, 1, weightClassDataFile);
        if (tmp.id == id && !tmp.isDeleted) {
            return address;
        }
    }

    return -1;
}


WeightClass _getWeightClassByAddress(int address) {
    WeightClass tmpWeightClass;

    fseek(weightClassDataFile, address, SEEK_SET);
    fread(&tmpWeightClass, sizeof(WeightClass), 1, weightClassDataFile);

    return tmpWeightClass;
}


bool insertWeightClass(WeightClass weightClass) {
    int promotionAddress = _getPromotionAddress(weightClass.promotionId);

    if (promotionAddress != -1) {
        Promotion promotion = _getPromotionByAddress(promotionAddress);
        if (promotion.isDeleted) {
            return false;
        }

        int weightClassAddress = _getWeightClassAddress(weightClass.id);
        if (weightClassAddress != -1) {
            return false;
        }

        weightClass.nextWeightClassAddress = promotion.weightClassAddress;
        weightClassAddress = (int)seekEndAndReturnPos(weightClassDataFile);
        promotion.weightClassAddress = weightClassAddress;

        fwrite(&weightClass, sizeof(WeightClass), 1, weightClassDataFile);
        _writePromotion(promotion, promotionAddress);

        ++weightClassTotalRecordsCount;
        ++weightClassExistingRecordsCount;
        updateWeightClassCounts();
    }
    return false;
}


WeightClass getWeightClass(int id) {
    int address = _getWeightClassAddress(id);
    return address != -1 ? _getWeightClassByAddress(address) : WEIGHT_CLASS_SENTINEL;
}


int countWeightClasses() {
    return weightClassExistingRecordsCount;
}


void printWeightClasses() {
    int recordSize = sizeof(WeightClass);
    int endOfFile = seekEndAndReturnPos(weightClassDataFile);

    WeightClass weightClass;
    for (int address = 0; address < endOfFile; address += recordSize) {
        fseek(weightClassDataFile, address, SEEK_SET);
        fread(&weightClass, recordSize, 1, weightClassDataFile);
        if (!weightClass.isDeleted) {
            printWeightClass(weightClass);
        }
    }
}


void _writeWeightClass(WeightClass weightClass, int address) {
    fseek(weightClassDataFile, address, SEEK_SET);
    fwrite(&weightClass, sizeof(WeightClass), 1, weightClassDataFile);
}


bool updateWeightClass(int id, char name[NAME_LENGTH], int weightLimit) {
    int address = _getWeightClassAddress(id);

    if (address != -1) {
        WeightClass weightClass = _getWeightClassByAddress(address);
        if (!weightClass.isDeleted) {
            if (name != NULL) {
                strcpy(weightClass.name, name);
            }
            if (weightLimit != -1) {
                weightClass.weightLimit = weightLimit;
            }
            _writeWeightClass(weightClass, address);
            return true;
        }
    }

    return false;
}


bool deleteWeightClass(int id) {
    int address = _getWeightClassAddress(id);
    if (id != -1) {
        WeightClass weightClass = _getWeightClassByAddress(address);
        if (!weightClass.isDeleted) {
            weightClass.isDeleted = true;
            _writeWeightClass(weightClass, address);

            --weightClassExistingRecordsCount;
            updateWeightClassCounts();

            return true;
        }
    }
    return false;
}


bool deletePromotion(int id) {
    int address = _getPromotionAddress(id);

    if (address != -1) {
        Promotion promotion = _getPromotionByAddress(address);
        if (!promotion.isDeleted) {
            promotion.isDeleted = true;
            _writePromotion(promotion, address);
            --promotionExistingRecordsCount;

            int weightClassAddress = promotion.weightClassAddress;

            WeightClass weightClass;
            int recordSize = sizeof(WeightClass);

            while (weightClassAddress != -1) {
                fseek(weightClassDataFile, weightClassAddress, SEEK_SET);
                fread(&weightClass, recordSize, 1, weightClassDataFile);
                if(!weightClass.isDeleted) {
                    weightClass.isDeleted = true;
                    _writeWeightClass(weightClass, weightClassAddress);
                    --weightClassExistingRecordsCount;
                }
                weightClassAddress = weightClass.nextWeightClassAddress;
            }
            return true;
        }
    }

    return false;
}


void _cleanPromotions() {
    FILE* tmpData = fopen(TMP_PROMOTION_DATA, "w+b");
    FILE* tmpIndex = fopen(TMP_PROMOTION_INDEX, "w+b");

    int recordSize = sizeof(Promotion);
    int endOfFile = seekEndAndReturnPos(promotionDataFile);

    int indexSize = sizeof(IdToAddress);
    int endOfIndex = seekEndAndReturnPos(promotionIndexFile);

    Promotion promotion;
    IdToAddress idToAddress;
    int newAddress;
    int indexAddress;

    for (int promotionAddress = 0; promotionAddress < endOfFile; promotionAddress += recordSize) {
        newAddress = (int)ftell(tmpData);

        fseek(promotionDataFile, promotionAddress, SEEK_SET);
        fread(&promotion, recordSize, 1, promotionDataFile);

        indexAddress = _binarySearchAddressOfIndex(promotion.id, 0, endOfIndex - indexSize);
        fseek(promotionIndexFile, indexAddress, SEEK_SET);
        fread(&idToAddress, indexSize, 1, promotionIndexFile);

        if (!promotion.isDeleted) {
            idToAddress.address = newAddress;
            fwrite(&idToAddress, indexSize, 1, tmpIndex);
            fwrite(&promotion, recordSize, 1, tmpData);
        }
    }
    fclose(tmpData);
    fclose(promotionDataFile);

    remove(PROMOTION_DATA);
    rename(TMP_PROMOTION_DATA, PROMOTION_DATA);
    promotionDataFile = fopen(PROMOTION_DATA, "r+b");

    fclose(tmpIndex);
    fclose(promotionIndexFile);

    remove(PROMOTION_INDEX);
    rename(TMP_PROMOTION_INDEX, PROMOTION_INDEX);
    promotionIndexFile = fopen(PROMOTION_INDEX, "r+b");
}


void _cleanWeightClasses() {
    FILE* tmpData = fopen(TMP_WEIGHT_CLASS_DATA, "w+b");

    int promotionRecordSize = sizeof(Promotion);
    int endOfPromotionDataFile = seekEndAndReturnPos(promotionDataFile);

    int weightClassRecordSize = sizeof(WeightClass);
    int weightClassAddress;
    int addressOfNewNext;

    Promotion promotion;
    WeightClass weightClass;

    for (int promotionAddress = 0; promotionAddress < endOfPromotionDataFile; promotionAddress += promotionRecordSize) {
        fseek(promotionDataFile, promotionAddress, SEEK_SET);
        fread(&promotion, promotionRecordSize, 1, promotionDataFile);

        weightClassAddress = promotion.weightClassAddress;
        addressOfNewNext = -1;

        while (weightClassAddress != -1) {
            fseek(weightClassDataFile, weightClassAddress, SEEK_SET);
            fread(&weightClass, sizeof(WeightClass), 1, weightClassDataFile);
            weightClassAddress = weightClass.nextWeightClassAddress;

            if (!weightClass.isDeleted) {
                weightClass.nextWeightClassAddress = addressOfNewNext;
                addressOfNewNext = (int)ftell(tmpData);
                fwrite(&weightClass, weightClassRecordSize, 1, tmpData);
            }
        }
        promotion.weightClassAddress = addressOfNewNext;
        _writePromotion(promotion, promotionAddress);
    }

    fclose(tmpData);
    fclose(weightClassDataFile);

    remove(WEIGHT_CLASS_DATA);
    rename(TMP_WEIGHT_CLASS_DATA, WEIGHT_CLASS_DATA);
    weightClassDataFile = fopen(WEIGHT_CLASS_DATA, "r+b");
}


void cleanGarbage() {
    mergeTables();
    _cleanPromotions();
    promotionTotalRecordsCount = promotionExistingRecordsCount;
    updatePromotionCounts();
    _cleanWeightClasses();
    weightClassTotalRecordsCount = weightClassExistingRecordsCount;
    updateWeightClassCounts();
}

#endif //LAB1_COMMANDS_H
