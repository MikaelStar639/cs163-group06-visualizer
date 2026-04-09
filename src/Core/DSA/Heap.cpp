#include "Core/DSA/Heap.hpp"
#include <algorithm>

namespace Core::DSA {

    Heap::Heap() {
        clear();
    }

    void Heap::clear() {
        pool.clear();
    }

    void Heap::heapifyUp(int index) {
        while (index > 0 && pool[parent(index)] < pool[index]) {
            std::swap(pool[parent(index)], pool[index]);
            index = parent(index);
        }
    }

    void Heap::heapifyDown(int index) {
        int maxIndex = index;
        int left = leftChild(index);
        int right = rightChild(index);
        int size = pool.size();

        if (left < size && pool[left] > pool[maxIndex]) maxIndex = left;
        if (right < size && pool[right] > pool[maxIndex]) maxIndex = right;
        if (index != maxIndex) {
            std::swap(pool[index], pool[maxIndex]);
            heapifyDown(maxIndex);
        }
    }

    void Heap::insert(int val) {
        pool.push_back(val);
        heapifyUp(pool.size() - 1);
    }

    int Heap::search(int val) const {
        for (size_t i = 0; i < pool.size(); ++i) {
            if (pool[i] == val) return i;
        }
        return -1;
    }

    bool Heap::deleteValue(int val) {
        int index = search(val);
        if (index == -1) return false;
        pool[index] = pool.back();
        pool.pop_back();

        if (index < pool.size()) {
            heapifyUp(index);
            heapifyDown(index);
        }
        return true;
    }

    bool Heap::updateValue(int oldVal, int newVal) {
        int index = search(oldVal);
        if (index == -1) return false;

        pool[index] = newVal;
        heapifyUp(index);
        heapifyDown(index);
        return true;
    }

    std::vector<int> Heap::getLogicalList() const {
        return pool;
    }

}