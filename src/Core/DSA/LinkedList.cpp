#include "Core/DSA/LinkedList.hpp"

namespace Core::DSA {

    LinkedList::LinkedList() {
        clear();
    }

    void LinkedList::clear() {
        pool.clear();
        freeIndices.clear();
        headIndex = -1;
    }

    int LinkedList::allocateNode(int val, int nextIdx) {
        int newIndex;
        if (!freeIndices.empty()) {
            newIndex = freeIndices.back();
            freeIndices.pop_back();
            pool[newIndex] = {val, nextIdx, true};
        } else {
            pool.push_back({val, nextIdx, true});
            newIndex = pool.size() - 1;
        }
        return newIndex;
    }

    void LinkedList::freeNode(int index) {
        if (index >= 0 && index < pool.size()) {
            pool[index].isActive = false;   
            pool[index].value = 0; 
            freeIndices.push_back(index);   
        }
    }

    void LinkedList::insertHead(int val) {
        headIndex = allocateNode(val, headIndex);
    }

    void LinkedList::insertTail(int val) {
        int newIndex = allocateNode(val, -1);
        if (headIndex == -1) {
            headIndex = newIndex;
            return;
        }
        int curr = headIndex;
        while (pool[curr].nextIndex != -1) curr = pool[curr].nextIndex;
        pool[curr].nextIndex = newIndex;
    }

    bool LinkedList::insertAt(int logicalPos, int val) {
        if (logicalPos < 0) return false;
        if (logicalPos == 0 || headIndex == -1) {
            insertHead(val);
            return true;
        }
        int curr = headIndex, currentPos = 0;
        while (curr != -1 && currentPos < logicalPos - 1) {
            curr = pool[curr].nextIndex;
            currentPos++;
        }
        if (curr == -1) return false; 
        int newIndex = allocateNode(val, pool[curr].nextIndex);
        pool[curr].nextIndex = newIndex;
        return true;
    }
    void LinkedList::deleteHead() {
        if (headIndex == -1) return;
        int temp = headIndex;
        headIndex = pool[headIndex].nextIndex;
        freeNode(temp);
    }

    void LinkedList::deleteTail() {
        if (headIndex == -1) return;
        
        if (pool[headIndex].nextIndex == -1) {
            freeNode(headIndex);
            headIndex = -1;
            return;
        }

        int curr = headIndex;
        int prev = -1;
        while (pool[curr].nextIndex != -1) {
            prev = curr;
            curr = pool[curr].nextIndex;
        }

        pool[prev].nextIndex = -1;
        freeNode(curr);
    }

    bool LinkedList::deleteAt(int logicalPos) {
        if (headIndex == -1 || logicalPos < 0) return false;
        
        if (logicalPos == 0) {
            deleteHead();
            return true;
        }

        int curr = headIndex;
        int prev = -1;
        int currentPos = 0;

        while (curr != -1 && currentPos < logicalPos) {
            prev = curr;
            curr = pool[curr].nextIndex;
            currentPos++;
        }

        if (curr == -1) return false;

        pool[prev].nextIndex = pool[curr].nextIndex;
        freeNode(curr);
        return true;
    }

    bool LinkedList::deleteByValue(int val) {
        int curr = headIndex, prev = -1;
        while (curr != -1) {
            if (pool[curr].value == val) {
                if (prev == -1) headIndex = pool[curr].nextIndex; 
                else pool[prev].nextIndex = pool[curr].nextIndex; 
                freeNode(curr);
                return true;
            }
            prev = curr;
            curr = pool[curr].nextIndex;
        }
        return false;
    }

    bool LinkedList::updateAt(int logicalPos, int newVal) {
        int curr = headIndex, currentPos = 0;
        while (curr != -1) {
            if (currentPos == logicalPos) {
                pool[curr].value = newVal;
                return true;
            }
            curr = pool[curr].nextIndex;
            currentPos++;
        }
        return false;
    }

    bool LinkedList::updateValue(int oldVal, int newVal) {
        int curr = headIndex;
        while (curr != -1) {
            if (pool[curr].value == oldVal) {
                pool[curr].value = newVal;
                return true; 
            }
            curr = pool[curr].nextIndex;
        }
        return false;
    }

    int LinkedList::search(int val) const {
        int curr = headIndex, logicalPosition = 0;
        while (curr != -1) {
            if (pool[curr].value == val) return logicalPosition; 
            curr = pool[curr].nextIndex;
            logicalPosition++;
        }
        return -1;
    }

    std::vector<int> LinkedList::getLogicalList() const {
        std::vector<int> result;
        int curr = headIndex;
        while (curr != -1) {
            result.push_back(pool[curr].value);
            curr = pool[curr].nextIndex;
        }
        return result;
    }

} // namespace Core::DSA