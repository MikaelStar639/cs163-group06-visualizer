#include "Core/DSA/MST.hpp"
#include <tuple>
#include <queue>
#include <algorithm>
#include <limits>

namespace Core::DSA {

    MST::DSU::DSU(int n) {
        reset(n);
    }

    void MST::DSU::reset(int n) {
        parent.resize(n);
        rank.assign(n, 0);
        for (int i = 0; i < n; ++i) parent[i] = i;
    }

    int MST::DSU::find(int x) {
        if (parent[x] == x) return x;
        return parent[x] = find(parent[x]);
    }

    bool MST::DSU::unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) return false;

        if (rank[a] < rank[b]) std::swap(a, b);
        parent[b] = a;
        if (rank[a] == rank[b]) rank[a]++;
        return true;
    }

    void MST::clear() {
        nodeCount = 0;
        nodeValues.clear();
        edges.clear();
    }

    void MST::setNodeCount(int n) {
        nodeCount = n;
    }

    int MST::getNodeCount() const {
        return nodeCount;
    }

    void MST::setNodeValues(const std::vector<int>& values) {
        nodeValues = values;
    }

    const std::vector<int>& MST::getNodeValues() const {
        return nodeValues;
    }

    void MST::setEdges(const std::vector<WeightedEdge>& newEdges) {
        edges = newEdges;
    }

    const std::vector<WeightedEdge>& MST::getEdges() const {
        return edges;
    }

    void MST::buildFromRaw(int n,
                           const std::vector<int>& values,
                           const std::vector<std::tuple<int,int,int>>& rawEdges) {
        clear();
        nodeCount = n;
        nodeValues = values;
        edges.clear();

        for (int i = 0; i < (int)rawEdges.size(); ++i) {
            auto [u, v, w] = rawEdges[i];
            edges.push_back({u, v, w, i});
        }
    }

    std::vector<MSTStep> MST::runKruskal() const {
        std::vector<MSTStep> steps;
        std::vector<WeightedEdge> sorted = edges;
        std::sort(sorted.begin(), sorted.end(), [](const WeightedEdge& a, const WeightedEdge& b) {
            if (a.w != b.w) return a.w < b.w;
            return a.id < b.id;
        });

        DSU dsu(nodeCount);

        for (const auto& e : sorted) {
            steps.push_back({MSTStep::Type::ConsiderEdge, e.id, e.u, e.v, e.w, -1, "consider"});
            if (dsu.unite(e.u, e.v)) {
                steps.push_back({MSTStep::Type::AcceptEdge, e.id, e.u, e.v, e.w, -1, "accept"});
            } else {
                steps.push_back({MSTStep::Type::RejectEdge, e.id, e.u, e.v, e.w, -1, "reject"});
            }
        }

        steps.push_back({MSTStep::Type::Finish, -1, -1, -1, 0, -1, "finish"});
        return steps;
    }

    std::vector<MSTStep> MST::runPrim(int startNode) const {
        std::vector<MSTStep> steps;
        if (nodeCount <= 0 || startNode < 0 || startNode >= nodeCount) {
            steps.push_back({MSTStep::Type::Finish, -1, -1, -1, 0, -1, "finish"});
            return steps;
        }

        std::vector<std::vector<WeightedEdge>> adj(nodeCount);
        for (const auto& e : edges) {
            adj[e.u].push_back(e);
            adj[e.v].push_back({e.v, e.u, e.w, e.id});
        }

        std::vector<bool> used(nodeCount, false);

        using PQItem = std::tuple<int,int,int,int>; // w, from, to, edgeId
        std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;

        used[startNode] = true;
        steps.push_back({MSTStep::Type::VisitNode, -1, -1, -1, 0, startNode, "visit_start"});

        for (const auto& e : adj[startNode]) {
            pq.push({e.w, startNode, e.v, e.id});
            steps.push_back({MSTStep::Type::RelaxEdge, e.id, startNode, e.v, e.w, -1, "push_edge"});
        }

        while (!pq.empty()) {
            auto [w, from, to, edgeId] = pq.top();
            pq.pop();

            steps.push_back({MSTStep::Type::ConsiderEdge, edgeId, from, to, w, -1, "consider"});

            if (used[to]) {
                steps.push_back({MSTStep::Type::RejectEdge, edgeId, from, to, w, -1, "reject"});
                continue;
            }

            used[to] = true;
            steps.push_back({MSTStep::Type::AcceptEdge, edgeId, from, to, w, -1, "accept"});
            steps.push_back({MSTStep::Type::VisitNode, -1, -1, -1, 0, to, "visit_node"});

            for (const auto& e : adj[to]) {
                if (!used[e.v]) {
                    pq.push({e.w, to, e.v, e.id});
                    steps.push_back({MSTStep::Type::RelaxEdge, e.id, to, e.v, e.w, -1, "push_edge"});
                }
            }
        }

        steps.push_back({MSTStep::Type::Finish, -1, -1, -1, 0, -1, "finish"});
        return steps;
    }

} // namespace Core::DSA