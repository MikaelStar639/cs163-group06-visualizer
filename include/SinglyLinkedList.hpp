#pragma once
#include <vector>

struct Node {
    int value;
    int nextIndex;
};

struct SinglyLinkedList {
    std::vector<Node> pool; 
    int headIndex; 

    SinglyLinkedList();
    
    void init();
    void addLast(int val);
    void addFirst(int val);
    void deleteValue(int val); 
    void updateValue(int oldVal, int newVal);
    int search(int val) const;

    std::vector<int> getLogicalList() const; 
};