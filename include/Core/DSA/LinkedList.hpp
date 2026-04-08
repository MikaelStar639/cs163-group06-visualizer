#pragma once
#include <vector>

namespace Core::DSA {

    struct Node {
        int value;      
        int nextIndex;
        bool isActive; 
    };

    class LinkedList {
    private:
        std::vector<Node> pool;       
        std::vector<int> freeIndices; 
        int headIndex;                

        int allocateNode(int val, int nextIdx = -1); 
        void freeNode(int index);

    public:
        LinkedList();
        
        void clear();

        void insertHead(int val);
        void insertTail(int val);
        bool insertAt(int logicalPos, int val);

        void deleteHead();
        void deleteTail();
        bool deleteAt(int logicalPos);
        bool deleteByValue(int val);

        bool updateAt(int logicalPos, int newVal);
        bool updateValue(int oldVal, int newVal);
        int search(int val) const;

        std::vector<int> getLogicalList() const;
        
        int getHeadIndex() const { return headIndex; }
        const std::vector<Node>& getPool() const { return pool; }
    };

} // namespace Core::DSA