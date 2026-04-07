#include "SinglyLinkedList.hpp"

SinglyLinkedList::SinglyLinkedList() {
    init();
}

void SinglyLinkedList::init() {
    pool.clear();
    headIndex = -1;
}

void SinglyLinkedList::addFirst(int val) {
    Node newNode;
    newNode.value = val;
    newNode.nextIndex = headIndex;
    
    pool.push_back(newNode);
    headIndex = pool.size() - 1;
}

void SinglyLinkedList::addLast(int val) {
    Node newNode;
    newNode.value = val;
    newNode.nextIndex = -1;
    
    pool.push_back(newNode);
    int newIndex = pool.size() - 1;

    if (headIndex == -1) {
        headIndex = newIndex;
        return;
    }

    int curr = headIndex;
    while (pool[curr].nextIndex != -1) {
        curr = pool[curr].nextIndex;
    }
    pool[curr].nextIndex = newIndex;
}

void SinglyLinkedList::deleteValue(int val) {
    int curr = headIndex;
    int prev = -1;

    while (curr != -1) {
        if (pool[curr].value == val) {
            if (prev == -1) {
                headIndex = pool[curr].nextIndex; 
            } else {
                pool[prev].nextIndex = pool[curr].nextIndex; 
            }
            return;
        }
        prev = curr;
        curr = pool[curr].nextIndex;
    }
}

void SinglyLinkedList::updateValue(int oldVal, int newVal) {
    int curr = headIndex;
    while (curr != -1) {
        if (pool[curr].value == oldVal) {
            pool[curr].value = newVal;
            return;
        }
        curr = pool[curr].nextIndex;
    }
}

int SinglyLinkedList::search(int val) const {
    int curr = headIndex;
    int logicalPosition = 0;
    
    while (curr != -1) {
        if (pool[curr].value == val) {
            return logicalPosition; 
        }
        curr = pool[curr].nextIndex;
        logicalPosition++;
    }
    return -1;
}

std::vector<int> SinglyLinkedList::getLogicalList() const {
    std::vector<int> result;
    int curr = headIndex;
    while (curr != -1) {
        result.push_back(pool[curr].value);
        curr = pool[curr].nextIndex;
    }
    return result;
}