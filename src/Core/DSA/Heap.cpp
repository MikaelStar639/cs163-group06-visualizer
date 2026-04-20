#include "Core/DSA/Heap.hpp"
#include <algorithm>

namespace Core {
    namespace DSA {

        Heap::Heap() : onAction(nullptr) {}

        int Heap::parent(int i) { return (i - 1) / 2; }
        int Heap::leftChild(int i) { return 2 * i + 1; }
        int Heap::rightChild(int i) { return 2 * i + 2; }
        
        void Heap::clear() {
            pool.clear();
            // If you add other state variables later (like a 'size' cache 
            // or specific focus indices), reset them here.
        }

        void Heap::loadRawData(const std::vector<int>& data) {
            pool = data; // Direct assignment, no heapify logic, no notifications
        }
        
        void Heap::setObserver(std::function<void(HeapAction, int, int, int)> callback) {
            onAction = callback;
        }

        void Heap::notify(HeapAction action, int i, int j, int val) {
            if (onAction) onAction(action, i, j, val);
        }

        void Heap::heapifyUp(int index) {
        while (index > 0) {
            int p = parent(index);
            
            // 1. Determine winner SILENTLY
            // If child > parent, child is winner (index). Otherwise, parent is winner (p).
            int maxIdx = (pool[index] > pool[p]) ? index : p;

            // 2. Notify with the winner in the 'val' parameter
            notify(HeapAction::Compare, p, index, maxIdx);

            if (pool[p] < pool[index]) {
                std::swap(pool[p], pool[index]);
                notify(HeapAction::Swap, p, index);
                index = p;
            } else {
                // 3. No swap: notify Unfocus to clean up the highlighted winner
                notify(HeapAction::Unfocus, maxIdx, -1);
                break;
            }
        }
    }

        void Heap::heapifyDown(int index) {
            int n = static_cast<int>(pool.size());
            while (true) {
                int maxIdx = index;
                int l = leftChild(index);
                int r = rightChild(index);

                // 1. Determine the winner across all available family members SILENTLY
                if (l < n && pool[l] > pool[maxIdx]) {
                    maxIdx = l;
                }
                if (r < n && pool[r] > pool[maxIdx]) {
                    maxIdx = r;
                }

                // 2. NOW Notify once for the whole trio. 
                // We use 'index' as parent, 'l' as child start, and 'maxIdx' as the winner (val).
                // If l >= n, it's a leaf, but we only notify if there's actually a comparison to show.
                if (l < n) {
                    notify(HeapAction::Compare, index, l, maxIdx);
                }

                // 3. Perform the swap if the parent isn't the winner
                if (maxIdx != index) {
                    std::swap(pool[index], pool[maxIdx]);
                    notify(HeapAction::Swap, index, maxIdx);
                    index = maxIdx;
                } else {
                    notify(HeapAction::Unfocus, maxIdx, -1);
                    break;
                }
            }
        }

        void Heap::insert(int val) {
            pool.push_back(val);
            notify(HeapAction::Insert, static_cast<int>(pool.size()) - 1, -1, val);
            heapifyUp(static_cast<int>(pool.size()) - 1);
        }

        int Heap::top() {
            if (pool.empty()) return -1;
            notify(HeapAction::Focus, 0); 
            return pool[0];
        }

        void Heap::removeRoot() {
            if (pool.empty()) return;

            int lastIdx = static_cast<int>(pool.size()) - 1;
            if (lastIdx > 0) {
                pool[0] = pool[lastIdx];
                notify(HeapAction::Update, 0, -1, pool[0]);
            }

            pool.pop_back();
            notify(HeapAction::Remove, lastIdx);

            if (!pool.empty()) heapifyDown(0);
        }

        void Heap::buildHeap(const std::vector<int>& data) {
            pool = data;
            // Initial batch insert visualization
            for (int i = 0; i < static_cast<int>(pool.size()); ++i) {
                notify(HeapAction::Insert, i, -1, pool[i]);
            }

            for (int i = (static_cast<int>(pool.size()) / 2) - 1; i >= 0; --i) {
                notify(HeapAction::Focus, i);
                heapifyDown(i);
            }
        }

    } // namespace DSA
} // namespace Core