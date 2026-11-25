#include <vector>
#include <iostream>
#include <cstdlib>
#include <iterator>

#ifndef CALVINS_MAP // header guard to avoid circular inclusions
#define CALVINS_MAP

class LLNode {
    // Each node has a 
    public: 
        int value;
        LLNode* next;
        LLNode* prev;
};

struct Bucket {
    int hashed;
    LLNode* nodeAddress;
    int rawKeyValue;
    // rule of 3 says I probably need a custom copy assignment operator, and a custom copy constructor too
    // RULE of 5 adds a move assignment operator, and a custom move constructor
    Bucket();
};

Bucket::Bucket() {
    this->hashed = -1;
    this->rawKeyValue = -1;
    this->nodeAddress = nullptr;
}

class CalvinsMap {
    public:
        LLNode* phantom_head; // default constructor takes no args
        std::vector<Bucket> buckets;
        // std::vector<std::vector<int>> buckets;
        size_t totalPopulation;
        size_t maxCapacity; // the total number of empty and filled buckets right now
        // size_t maxBucketSize;
        double loadFactor;

        CalvinsMap(); // TODO: I'll need a custom destructor, but for now don't worry about it 
        int insert(int key, int value);
        int get(int key);
        void print();
        size_t getBucketIndex(int key, size_t capacity, std::vector<Bucket>* vecToCheck); // on some level we'll need to check for collisions, so each bucket array item should also hold the raw key (LATER TODO)
    private:
        void resize();
};

CalvinsMap::CalvinsMap() {
    // need to allocate all of the pointers on the heap
    this->phantom_head = new(LLNode);
    this->totalPopulation = 0;
    this->maxCapacity = 1;
    // this->buckets = std::vector<int, LLNode*>(maxCapacity, nullptr);
    this->buckets = std::vector<Bucket>(maxCapacity);
    this->loadFactor = 0.5;
}

size_t CalvinsMap::getBucketIndex(int key, size_t capacity, std::vector<Bucket>* vecToCheck) {
    // use a quadratic hash instead (hash + i ^2)
    // simple implementation is just move one spot ahead
    size_t finalIndex = (size_t) (key % capacity);
    size_t i = 0;
    while ((*vecToCheck)[finalIndex].hashed != -1) { // if this isn't unreserved yet, keep probing
        finalIndex = (size_t) (key +  (i * i)) % capacity;
        i++;
    }
    return finalIndex;
}

void CalvinsMap::resize() {
    /*
    TODO: 
        calculate new capacity (using current capacity)
        allocate a vector of size new capacity
        re-hash the new elements based on the new capacity, and place them in the new vector
        delete the dynamic allocated space of the old vector
        update the vector pointer to now reference this new vector
    */
    size_t oldCapacity = this->maxCapacity;
    size_t newCapacity = (this->maxCapacity + 1) * 2;
    std::cout << "--[RESIZE] to capacity => " << newCapacity << "\n";
    std::vector<Bucket> newVector = std::vector<Bucket>(newCapacity);

    // For all old buckets, find them a new home
    for (size_t i = 0; i < oldCapacity; i++) {
        if (this->buckets[i].hashed == -1) {
            continue; // we haven't filled this in yet
        }
        int rawKeyValue = this->buckets[i].rawKeyValue;
        int newHash = getBucketIndex(rawKeyValue, newCapacity, &(newVector)); // TODO: this is failing bc it's vector is stale, it won't track with the updates I make to the new one
        // std::cout << "newHash-->" << newHash << "\n";
        // TODO: ADD COLLISION CHECK FOR IF THIS HASH SPOT IS ALREADY TAKEN UP ----------------------------------------------------------------------
        Bucket newBucket {};
        newBucket.hashed = 1;
        
        newBucket.nodeAddress = this->buckets[i].nodeAddress;
        newBucket.rawKeyValue = this->buckets[i].rawKeyValue;
        newVector[newHash] = newBucket;
    }
    
    // BUCKETS HAVE BEEN REMAPPED, DESTROY THE OLD VECTOR?? AND UPDATE THE CLASS POINTER
    this->buckets = newVector;
    this->maxCapacity = newCapacity;
}


int CalvinsMap::insert(int key, int value) {
    /*
    1. Make newNode(value, next, prev)
    2. update LLHead->newNode
    3. Add this newNode to a bucket (check for bucket resize)
    */
    std::cout << "--[INSERT] value ==> " << value << "\n";
    LLNode* newHead = new(LLNode);
    if (newHead == nullptr) {
        std::cout << "failed to make space for new node";
        exit(1);
    }
    // Fill in the LL node's attributes and make it the front
    newHead->prev = phantom_head;
    newHead->next = nullptr;
    newHead -> value = value;
    this->phantom_head->next = newHead;
    // Now we need to add it's pointer to the buckets
    // this is also where we would check for resize, and subsequently perform the resize if needed
    if ((this->maxCapacity * this->loadFactor) < (this->totalPopulation + 1)) {
        // RESIZE
        this->resize();
    }
    size_t hashIdx = this->getBucketIndex(key, this->maxCapacity, &this->buckets);
    Bucket insertedBucket;
    insertedBucket.rawKeyValue = key;
    insertedBucket.hashed = 1;
    insertedBucket.nodeAddress = newHead;
    this->buckets[hashIdx] = insertedBucket;
    this->totalPopulation++;
    std::cout << "=================after insertion\n";
    this->print();
    std::cout << "========================================================================\n";
    return 0;
}

void CalvinsMap::print() {
    std::cout << "--[PRINT]\n";
    std::cout << "(Size, Capacity) -->  (" << this->totalPopulation << ", " << this->maxCapacity << ")\n";
    if (this->phantom_head->next != nullptr) {
        std::cout << "First LL node: " << this->phantom_head->next->value << "\n\n";
    }
    for (size_t i = 0; i < this->maxCapacity; i++) {
        // std::cout << "Bucket[" << i << "] ==> " << this->buckets[i].rawKeyValue << '\n';
        if (this->buckets[i].hashed == -1) {
            std::cout << "Bucket[" << i << "] ==> NOT MAPPED (key = " << this->buckets[i].rawKeyValue << "\n" ;
            continue;
        }
        std::cout << "Bucket[" << i << "] ==> " << this->buckets[i].nodeAddress->value << '\n';
    }
}

int CalvinsMap::get(int key) {
    size_t hashIdx;
    // return -1 on failure, we'll only store positive numbers on our hashmap i guess
    int jumps = 0;
    while (1) {
        hashIdx = (size_t) ((key + (jumps * jumps)) % this->maxCapacity);
        if (this->buckets[hashIdx].hashed == -1) { // this would never be found, if it's not along it's path
            // std::cout << "NOT FOUND:" << this->buckets[hashIdx].rawKeyValue << "\n";
            return -1;
        }
        if (this->buckets[hashIdx].rawKeyValue == key) {
            return this->buckets[hashIdx].nodeAddress->value;
        }
        jumps++;
    }

}

#endif
