#pragma once

#include "Core/AppContext.hpp"
#include "Core/DSA/Heap.hpp"
#include "UI/DSA/Graph.hpp"
#include <string>

namespace {

enum class HeapStepType {
    Compare,
    InsertLastVisual,
    OverwriteValue,
    RemoveLastVisual,
    ApplyState
};

struct HeapStep {
    HeapStepType type;
    int i = -1;
    int j = -1;
    int value = 0;
    std::vector<int> state;
};

int findFirst(const std::vector<int>& a, int val) {
    for (int i = 0; i < static_cast<int>(a.size()); ++i) {
        if (a[i] == val) return i;
    }
    return -1;
}

void heapifyUpTrace(std::vector<int>& a, int index, std::vector<HeapStep>& steps) {
    while (index > 0) {
        int p = (index - 1) / 2;

        steps.push_back({HeapStepType::Compare, p, index});

        if (a[p] < a[index]) {
            std::swap(a[p], a[index]);

            HeapStep s;
            s.type = HeapStepType::ApplyState;
            s.state = a;
            steps.push_back(std::move(s));

            index = p;
        } else {
            break;
        }
    }
}

void heapifyDownTrace(std::vector<int>& a, int index, std::vector<HeapStep>& steps) {
    int n = static_cast<int>(a.size());

    while (true) {
        int maxIndex = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < n) {
            steps.push_back({HeapStepType::Compare, index, left});
            if (a[left] > a[maxIndex]) maxIndex = left;
        }

        if (right < n) {
            steps.push_back({HeapStepType::Compare, maxIndex, right});
            if (a[right] > a[maxIndex]) maxIndex = right;
        }

        if (maxIndex != index) {
            std::swap(a[index], a[maxIndex]);

            HeapStep s;
            s.type = HeapStepType::ApplyState;
            s.state = a;
            steps.push_back(std::move(s));

            index = maxIndex;
        } else {
            break;
        }
    }
}

std::vector<HeapStep> buildInsertSteps(const std::vector<int>& initial, int val) {
    std::vector<int> a = initial;
    std::vector<HeapStep> steps;

    a.push_back(val);
    steps.push_back({HeapStepType::InsertLastVisual, -1, -1, val});

    heapifyUpTrace(a, static_cast<int>(a.size()) - 1, steps);
    return steps;
}

std::vector<HeapStep> buildDeleteSteps(const std::vector<int>& initial, int val) {
    std::vector<int> a = initial;
    std::vector<HeapStep> steps;

    int index = findFirst(a, val);
    if (index == -1) return steps;

    int lastIndex = static_cast<int>(a.size()) - 1;

    if (index != lastIndex) {
        steps.push_back({HeapStepType::Compare, index, lastIndex});

        a[index] = a.back();
        steps.push_back({HeapStepType::OverwriteValue, index, -1, a[index]});
    }

    steps.push_back({HeapStepType::RemoveLastVisual});
    a.pop_back();

    // Lưu ý: phải mô phỏng đúng logic model hiện tại của teammate:
    // heapifyUp(index) rồi heapifyDown(index) với cùng index gốc
    if (index < static_cast<int>(a.size())) {
        int originalIndex = index;
        heapifyUpTrace(a, originalIndex, steps);
        heapifyDownTrace(a, originalIndex, steps);
    }

    return steps;
}

std::vector<HeapStep> buildUpdateSteps(const std::vector<int>& initial, int oldVal, int newVal) {
    std::vector<int> a = initial;
    std::vector<HeapStep> steps;

    int index = findFirst(a, oldVal);
    if (index == -1) return steps;

    a[index] = newVal;
    steps.push_back({HeapStepType::OverwriteValue, index, -1, newVal});

    // Mô phỏng đúng model teammate:
    // heapifyUp(index) rồi heapifyDown(index) với index gốc
    int originalIndex = index;
    heapifyUpTrace(a, originalIndex, steps);
    heapifyDownTrace(a, originalIndex, steps);

    return steps;
}

} // namespace

namespace Controllers {

    class HeapController {
    private:
        AppContext& ctx;
        UI::DSA::Graph& graph;
        Core::DSA::Heap& model;

        float centerX  = 800.f;
        float startY   = 260.f;
        float levelGap = 120.f;
        float baseGap  = 90.f;

        bool pendingRebuild = false;

        void syncGraphEdges();
        void triggerLayout(float duration = 0.5f);
        void rebuildGraphFromModel();

        // helper
        

        void applyStateToGraph(const std::vector<int>& state);
        

    public:
        HeapController(AppContext& context, UI::DSA::Graph& g, Core::DSA::Heap& m);

        void processDeferredActions();

        void handleCreateRandom(int size);
        void handleCreateFromFile();
        void handleEditDataFile();
        void handleInsert(int val);
        void handleRemove(int val);
        void handleSearch(int targetValue);
        void handleUpdate(int oldVal, int newVal);
        void handleClearAll();
    };

}