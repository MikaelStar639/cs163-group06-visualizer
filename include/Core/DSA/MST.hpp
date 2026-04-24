#pragma once
#include <vector>
#include <string>

namespace Core::DSA {

    struct WeightedEdge {
        int u;
        int v;
        int w;
        int id;
    };

    struct MSTStep {
        enum class Type {
            ConsiderEdge,
            AcceptEdge,
            RejectEdge,
            VisitNode,
            RelaxEdge,
            Finish
        };

        Type type;
        int edgeId = -1;
        int u = -1;
        int v = -1;
        int w = 0;
        int node = -1;
        std::string codeLabel;
    };

    class MST {
    private:
        int nodeCount = 0;
        std::vector<int> nodeValues;
        std::vector<WeightedEdge> edges;

        // DSU for Kruskal
        struct DSU {
            std::vector<int> parent, rank;
            explicit DSU(int n = 0);
            void reset(int n);
            int find(int x);
            bool unite(int a, int b);
        };

    public:
        void clear();

        void setNodeCount(int n);
        int getNodeCount() const;

        void setNodeValues(const std::vector<int>& values);
        const std::vector<int>& getNodeValues() const;

        void setEdges(const std::vector<WeightedEdge>& newEdges);
        const std::vector<WeightedEdge>& getEdges() const;

        void buildFromRaw(int n,
                          const std::vector<int>& values,
                          const std::vector<std::tuple<int,int,int>>& rawEdges);

        std::vector<MSTStep> runKruskal() const;
        std::vector<MSTStep> runPrim(int startNode = 0) const;
    };

} // namespace Core::DSA