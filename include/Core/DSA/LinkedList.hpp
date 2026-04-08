#pragma once
#include <vector>

namespace Core::DSA {

    struct Node {
        int value;      // Đổi thành int
        int nextIndex;
        bool isActive; 
    };

    class LinkedList {
    private:
        std::vector<Node> pool;       
        std::vector<int> freeIndices; 
        int headIndex;                

        int allocateNode(int val, int nextIdx = -1); // Đổi thành int
        void freeNode(int index);

    public:
        LinkedList();
        
        void clear();

        // --- CÁC HÀM THÊM (INSERT) ---
        void insertHead(int val);
        void insertTail(int val);
        bool insertAt(int logicalPos, int val);

        // --- CÁC HÀM XÓA (DELETE) ---
        void deleteHead();
        void deleteTail();
        bool deleteAt(int logicalPos);
        bool deleteByValue(int val);

        // --- CÁC HÀM CẬP NHẬT (UPDATE) & TÌM KIẾM (SEARCH) ---
        bool updateAt(int logicalPos, int newVal);
        bool updateValue(int oldVal, int newVal);
        int search(int val) const;

        // --- CÁC HÀM XUẤT DỮ LIỆU ---
        std::vector<int> getLogicalList() const; // Trả về mảng int
        
        int getHeadIndex() const { return headIndex; }
        const std::vector<Node>& getPool() const { return pool; }
    };

} // namespace Core::DSA