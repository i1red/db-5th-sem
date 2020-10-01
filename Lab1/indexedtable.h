//
// Created by ivan on 9/30/20.
//

#ifndef LAB1_INDEXEDTABLE_H
#define LAB1_INDEXEDTABLE_H

#include <stdbool.h>
#include <string.h>

#include "dbfiles.h"

#define IN_MEMORY_TABLE_CAPACITY 64

// Index table consists of two parts: in file and in memory (RAM)
// These parts are always sorted
// When in memory table is full it is merged in file and cleared



typedef struct IdToAddress {
    int id;
    int address;
} IdToAddress;


IdToAddress table[IN_MEMORY_TABLE_CAPACITY];
int curSize = 0;


int seekEndAndReturnPos(FILE* p) {
    fseek(p, 0, SEEK_END);
    return (int)ftell(p);
}


int bisectInMemoryTable(int id, int start, int end) {
    if (end < start) {
        return start;
    }

    int mid = (start + end) / 2;
    int midId = table[mid].id;

    if (id > midId) {
        return bisectInMemoryTable(id, mid + 1, end);
    }
    if (id < midId) {
        return bisectInMemoryTable(id, start, mid - 1);
    }
    return mid;
}


int binarySearchInMemoryTable(int id) {
    int possibleIndex = bisectInMemoryTable(id, 0, curSize - 1);
    return possibleIndex < curSize && id == table[possibleIndex].id ? possibleIndex : -1;
}


int _binarySearchAddressInFile(int id, long start, long end) {
    if (end < start) {
        return -1;
    }

    long recordSize = (long)sizeof(IdToAddress);
    long mid = (((start + end) / recordSize) / 2) * recordSize;
    fseek(promotionIndexFile, mid, SEEK_SET);

    IdToAddress midIdToAddress;
    fread(&midIdToAddress, sizeof(IdToAddress), 1, promotionIndexFile);

    if (id > midIdToAddress.id) {
        return _binarySearchAddressInFile(id, mid + recordSize, end);
    }

    if (id < midIdToAddress.id) {
        return _binarySearchAddressInFile(id, start, mid - recordSize);
    }

    return midIdToAddress.address;
}


int _binarySearchAddressOfIndex(int id, long start, long end) {
    if (end < start) {
        return -1;
    }

    long recordSize = (long)sizeof(IdToAddress);
    long mid = (((start + end) / recordSize) / 2) * recordSize;
    fseek(promotionIndexFile, mid, SEEK_SET);

    IdToAddress midIdToAddress;
    fread(&midIdToAddress, sizeof(IdToAddress), 1, promotionIndexFile);

    if (id > midIdToAddress.id) {
        return _binarySearchAddressInFile(id, mid + recordSize, end);
    }

    if (id < midIdToAddress.id) {
        return _binarySearchAddressInFile(id, start, mid - recordSize);
    }

    return (int)mid;
}




bool isInMemoryTableFull() {
    return curSize == IN_MEMORY_TABLE_CAPACITY;
}


void clearInMemoryTable() {
    curSize = 0;
}


void insertToInMemoryTable(int id, int address, int position) {
    if(curSize - position > 0) {
        memmove(table + position + 1, table + position, curSize - position);
    }
    table[position].id = id;
    table[position].address = address;
    ++curSize;
}


int getAddressFromInMemoryTable(int id) {
    int index = binarySearchInMemoryTable(id);
    return index != -1 ? table[index].address : -1;
}


int _getPromotionAddress(int id) {
    int inMemoryAddress = getAddressFromInMemoryTable(id);
    fseek(promotionIndexFile, 0, SEEK_END);
    return inMemoryAddress != -1 ? inMemoryAddress : _binarySearchAddressInFile(id, 0, ftell(promotionIndexFile) - (long)sizeof(IdToAddress));
}


void mergeTables() {
    updatePromotionCounts();

    if (curSize > 0) {
        FILE* fp = fopen(TMP_PROMOTION_INDEX, "w+b");

        int i = 0;

        int endOfFile = seekEndAndReturnPos(promotionIndexFile);
        int recordSize = (int)sizeof(IdToAddress);
        int j = 0;

        IdToAddress tmp;

        while (j < endOfFile && i < curSize) {
            fseek(promotionIndexFile, j, SEEK_SET);
            fread(&tmp, recordSize, 1, promotionIndexFile);

            if (tmp.id < table[i].id) {
                fwrite(&tmp, recordSize, 1, fp);
                j += recordSize;
            } else {
                fwrite(table + i, recordSize, 1, fp);
                ++i;
            }
        }

        while (j < endOfFile) {
            fseek(promotionIndexFile, j, SEEK_SET);
            fread(&tmp, recordSize, 1, promotionIndexFile);
            fwrite(&tmp, recordSize, 1, fp);
            j += recordSize;
        }

        while (i < curSize) {
            fwrite(table + i, recordSize, 1, fp);
            ++i;
        }

        clearInMemoryTable();
        fclose(fp);
        fclose(promotionIndexFile);

        remove(PROMOTION_INDEX);
        rename(TMP_PROMOTION_INDEX, PROMOTION_INDEX);
        promotionIndexFile = fopen(PROMOTION_INDEX, "r+b");
    }
}

void insertPromotionIndex(int id, int address) {
    ++promotionExistingRecordsCount;
    ++promotionTotalRecordsCount;
    insertToInMemoryTable(id, address, bisectInMemoryTable(id, 0, curSize - 1));
    if (isInMemoryTableFull()) {
        mergeTables();
    }
}


#endif //LAB1_INDEXEDTABLE_H
