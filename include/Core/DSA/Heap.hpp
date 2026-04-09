#pragma once
#include <vector>

namespace Core::DSA {

    class Heap {
    private:
        std::vector<int> pool;

        int parent(int i) const { return (i - 1) / 2; }
        int leftChild(int i) const { return 2 * i + 1; }
        int rightChild(int i) const { return 2 * i + 2; }
        void heapifyUp(int index);
        void heapifyDown(int index);

    public:
        Heap();
        void clear();
        void insert(int val);
        bool deleteValue(int val);
        bool updateValue(int oldVal, int newVal);
        int search(int val) const;
        std::vector<int> getLogicalList() const;
        const std::vector<int>& getPool() const { return pool; }
    };

}