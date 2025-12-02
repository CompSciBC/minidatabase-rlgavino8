#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>   
#include <vector>
#include <algorithm>     
#include "BST.h"      
#include "Record.h"
//add header files as needed

using namespace std;

// Converts a string to lowercase (used for case-insensitive searches)
static inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ================== Index Engine ==================
// Acts like a small "database engine" that manages records and two BST indexes:
// 1) idIndex: maps student_id → record index (unique key)
// 2) lastIndex: maps lowercase(last_name) → list of record indices (non-unique key)
struct Engine {
    vector<Record> heap;                  // the main data store (simulates a heap file)
    BST<int, int> idIndex;                // index by student ID
    BST<string, vector<int>> lastIndex;   // index by last name (can have duplicates)

    // Inserts a new record and updates both indexes.
    // Returns the record ID (RID) in the heap.
    int insertRecord(const Record &recIn) {
        int recordID = heap.size();

        // 1. Adding the record to the heap
        heap.push_back(recIn);

        // 2. Adding the record to the idIndex BST
        idIndex.insert(recIn.id, recordID);

        // 3. Adding the record to the lastIndex BST
        string lastName = toLower(recIn.last);
        vector<int> *records = lastIndex.find(lastName);
        if(!records) 
        {
            // Case if there are no previous records with the same last name
            lastIndex.insert(lastName, vector<int>{recordID});
        }
        else 
        {
            // Case if a record with the same last name exists
            records->push_back(recordID);
        }
    }

    // Deletes a record logically (marks as deleted and updates indexes)
    // Returns true if deletion succeeded.
    bool deleteById(int id) {
        int *recordIDptr = idIndex.find(id);
        if(!recordIDptr) {
            return false;
        }

        // 1. Soft deleting the record from the heap by setting deleted to true
        int recordID = *recordIDptr;
        heap[recordID].deleted = true;

        // 2. Removing the record from idIndex
        idIndex.erase(id);

        // 3. Removing the record from lastIndex
        // Need to account for if the record is part of a list already or not
        string lastName = toLower(heap[recordID].last);
        vector<int> *records = lastIndex.find(lastName);
        if(records)
        {
            // Case if there are multiple records with the same last name already in the database
            records->erase(remove(records->begin(), records->end(), recordID), records->end());

            // Case if removing the record also removes the last instance of that last name in the database
            if(records->empty()) {
                lastIndex.erase(lastName);
            }
        }
    }

    // Finds a record by student ID.
    // Returns a pointer to the record, or nullptr if not found.
    // Outputs the number of comparisons made in the search.
    const Record *findById(int id, int &cmpOut) {
        // Zeroing out the comparisons value in idIndex
        cmpOut = 0;
        idIndex.resetMetrics();

        // Finding the record via the key 'id' inside idIndex
        // Setting cmpOut to the number of comparisons tracked inside idIndex
        int *idPtr = idIndex.find(id);
        cmpOut = idIndex.comparisons;

        // Case if the record doesn't exist inside idIndex
        if(!idPtr) {
            return nullptr;
        }

        // Case if the record does exist but was soft-deleted in the heap
        const Record &record = heap[*idPtr];
        if(record.deleted) {
            return nullptr;
        }

        // Otherwise, record has been found
        return &record;

    }

    // Returns all records with ID in the range [lo, hi].
    // Also reports the number of key comparisons performed.
    vector<const Record *> rangeById(int lo, int hi, int &cmpOut) {
        //TODO
    }

    // Returns all records whose last name begins with a given prefix.
    // Case-insensitive using lowercase comparison.
    vector<const Record *> prefixByLast(const string &prefix, int &cmpOut) {
        //TODO
    }
};

#endif
