#pragma once

#include <vector>
#include <functional>

namespace Core {
    namespace DSA {

        enum class HeapAction {
            Compare,
            Swap,
            Update,
            Insert,
            Remove,
            Focus,
            Unfocus
        };

        class Heap {
        private:
            std::vector<int> pool;
            std::function<void(HeapAction, int, int, int)> onAction;

            int parent(int i);
            int leftChild(int i);
            int rightChild(int i);
            void notify(HeapAction action, int i = -1, int j = -1, int val = 0);

        public:
            Heap();
            void clear();
            void loadRawData(const std::vector<int>& data);
            
            void setObserver(std::function<void(HeapAction, int, int, int)> callback);

            // Core Logic
            void heapifyUp(int index);
            void heapifyDown(int index);

            // High-level operations
            void insert(int val);
            void removeRoot();
            void buildHeap(const std::vector<int>& data);
            int top();

            // Utilities
            const std::vector<int>& getPool() const { return pool; }
            int size() const { return static_cast<int>(pool.size()); }
        };

    } // namespace DSA
} // namespace Core