#include "Controllers/MSTController.hpp"

#include "Core/DSA/PseudoCodeData.hpp"
#include "UI/Animations/Core/SequenceAnimation.hpp"
#include "UI/Animations/Core/CallbackAnimation.hpp"
#include "UI/Animations/Core/WaitAnimation.hpp"
#include "UI/Animations/Core/ParallelAnimation.hpp"
#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include "UI/Animations/Node/NodeMoveAnimation.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
#include "UI/Animations/Edge/EdgeColorAnimation.hpp"
#include "UI/Animations/Edge/EdgeScaleAnimation.hpp"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <set>
#include <cstdlib>
#include <cmath>
#include <unordered_map>
#include <map>

#include <unordered_map>

namespace Controllers {

    namespace {
        constexpr float DEFAULT_WAIT = 0.28f;

        const sf::Color MST_EDGE_ACCEPT = sf::Color(50, 220, 120);
        const sf::Color MST_EDGE_REJECT = sf::Color(220, 80, 80);
        const sf::Color MST_NODE_VISITED = sf::Color(80, 140, 255);
        const sf::Color MST_NODE_VISITED_TEXT = sf::Color::White;

        constexpr float MST_EDGE_THICK = 5.5f;
        constexpr float MST_EDGE_CONSIDER_THICK = 4.0f;

        std::pair<int,int> normPair(int a, int b) {
            if (a > b) std::swap(a, b);
            return {a, b};
        }

        std::string trimCopy(const std::string& s) {
            std::size_t l = s.find_first_not_of(" \t\r\n");
            if (l == std::string::npos) return "";
            std::size_t r = s.find_last_not_of(" \t\r\n");
            return s.substr(l, r - l + 1);
        }

        bool parseAutoGraphLines(const std::vector<std::string>& lines,
                                std::vector<int>& nodeValues,
                                std::vector<std::tuple<int,int,int>>& edges,
                                std::string& outError) {
            nodeValues.clear();
            edges.clear();
            outError.clear();

            std::unordered_map<int, int> labelToIndex;
            std::map<std::pair<int,int>, int> edgeIndexByPair;

            auto ensureNode = [&](int label) -> int {
                auto it = labelToIndex.find(label);
                if (it != labelToIndex.end()) return it->second;

                int newIndex = static_cast<int>(nodeValues.size());
                nodeValues.push_back(label);
                labelToIndex[label] = newIndex;
                return newIndex;
            };

            for (std::size_t lineNo = 0; lineNo < lines.size(); ++lineNo) {
                std::string line = trimCopy(lines[lineNo]);
                if (line.empty()) continue;
                if (!line.empty() && line[0] == '#') continue;

                std::stringstream ss(line);
                std::vector<int> nums;
                std::string token;

                while (ss >> token) {
                    try {
                        std::size_t pos = 0;
                        int val = std::stoi(token, &pos);
                        if (pos != token.size()) {
                            outError = "Line " + std::to_string(lineNo + 1) + ": invalid token";
                            return false;
                        }
                        nums.push_back(val);
                    } catch (...) {
                        outError = "Line " + std::to_string(lineNo + 1) + ": invalid integer";
                        return false;
                    }
                }

                if (nums.empty()) continue;

                if (nums.size() == 1) {
                    ensureNode(nums[0]);
                }
                else if (nums.size() == 2) {
                    // dòng đang gõ dở / thiếu weight -> bỏ qua
                    continue;
                }
                else if (nums.size() == 3) {
                    int uLabel = nums[0];
                    int vLabel = nums[1];
                    int w      = nums[2];

                    int u = ensureNode(uLabel);
                    int v = ensureNode(vLabel);

                    if (u == v) continue;

                    auto key = std::minmax(u, v);

                    auto it = edgeIndexByPair.find(key);
                    if (it != edgeIndexByPair.end()) {
                        edges[it->second] = std::make_tuple(u, v, w);
                    } else {
                        edgeIndexByPair[key] = static_cast<int>(edges.size());
                        edges.emplace_back(u, v, w);
                    }
                }
                else {
                    outError = "Line " + std::to_string(lineNo + 1) + ": use 'x' or 'u v w'";
                    return false;
                }
            }

            return true;
        }

        
    }

    MSTController::MSTController(AppContext& context,
                                 UI::DSA::Graph& g,
                                 Core::DSA::MST& m,
                                 UI::Widgets::PseudoCodeViewer* viewer)
        : ctx(context), graph(g), model(m), codeViewer(viewer) {}

    UI::DSA::Edge* MSTController::getEdgeById(int edgeId) const {
        const auto& es = graph.getEdges();
        if (edgeId < 0 || edgeId >= static_cast<int>(es.size())) return nullptr;
        return es[edgeId].get();
    }

    void MSTController::forceSnapLayout() {
        int n = static_cast<int>(graph.getNodes().size());
        if (n == 0) return;

        if (n == 1) {
            auto* node = graph.getNode(0);
            if (node) node->setPosition({centerX, centerY});
            return;
        }

        for (int i = 0; i < n; ++i) {
            float angle = -3.14159265f / 2.f + i * 2.f * 3.14159265f / n;
            sf::Vector2f target{
                centerX + radius * std::cos(angle),
                centerY + radius * std::sin(angle)
            };

            auto* node = graph.getNode(i);
            if (node) node->setPosition(target);
        }
    }

    void MSTController::rebuildGraphFromModel() {
        // 1. Lưu vị trí node cũ (để nếu user đang kéo thả thì không bị reset)
        std::unordered_map<std::string, sf::Vector2f> oldPositions;
        std::unordered_set<std::string> oldLockedLabels;
        for (const auto& nodePtr : graph.getNodes()) {
            if (nodePtr) {
                std::string label = nodePtr->getLabel();
                oldPositions[label] = nodePtr->getPosition();
                
                if (graph.isNodeLocked(nodePtr.get())) {
                    oldLockedLabels.insert(label);
                }
            }
        }

        graph.clear();

        const auto& nodeValues = model.getNodeValues();
        int n = static_cast<int>(nodeValues.size());
        if (n <= 0) return;

        // 2. Khởi tạo vị trí nháp: Xếp vòng tròn để lò xo có đà bung ra
        std::vector<sf::Vector2f> positions(n);
        for (int i = 0; i < n; ++i) {
            std::string label = std::to_string(nodeValues[i]);
            auto it = oldPositions.find(label);
            if (it != oldPositions.end()) {
                positions[i] = it->second; 
            } else {
                float angle = -3.14159265f / 2.f + i * 2.f * 3.14159265f / n;
                positions[i] = { centerX + radius * std::cos(angle), centerY + radius * std::sin(angle) };
            }
        }

        // 3. Chạy thuật toán Lực "Ngầm" (Không Animation)
        if (n > 1) {
            const int ITERATIONS = 150;
            const float L = 140.f;              // Cạnh dài hơn một chút
            const float K_repel = L * L * 1.5f; // Lực đẩy mạnh hơn chống dính
            const float K_attract = 1.0f / L;
            float temperature = 100.f;

            const auto& edges = model.getEdges();

            for (int step = 0; step < ITERATIONS; ++step) {
                std::vector<sf::Vector2f> forces(n, {0.f, 0.f});

                // Lực Đẩy (Repulsion)
                for (int i = 0; i < n; ++i) {
                    for (int j = i + 1; j < n; ++j) {
                        sf::Vector2f delta = positions[i] - positions[j];
                        float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                        if (dist < 0.1f) dist = 0.1f;
                        float repulse = K_repel / dist;
                        sf::Vector2f f = (delta / dist) * repulse;
                        forces[i] += f;
                        forces[j] -= f;
                    }
                }

                // Lực Hút theo Cạnh (Attraction)
                for (const auto& e : edges) {
                    sf::Vector2f delta = positions[e.v] - positions[e.u];
                    float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                    if (dist < 0.1f) dist = 0.1f;
                    float attract = dist * dist * K_attract;
                    sf::Vector2f f = (delta / dist) * attract;
                    forces[e.u] += f;
                    forces[e.v] -= f;
                }

                // Cập nhật tọa độ
                for (int i = 0; i < n; ++i) {
                    float fMag = std::sqrt(forces[i].x * forces[i].x + forces[i].y * forces[i].y);
                    if (fMag > 0) {
                        positions[i] += (forces[i] / fMag) * std::min(fMag, temperature);
                    }
                    // Giới hạn để node không rớt ra ngoài cửa sổ
                    positions[i].x = std::max(centerX - 450.f, std::min(centerX + 450.f, positions[i].x));
                    positions[i].y = std::max(centerY - 280.f, std::min(centerY + 280.f, positions[i].y));
                }
                temperature *= 0.95f;
            }
        }

        // 4. Lắp ráp Node và Edge đã chuẩn bị sẵn vào Graph
        for (int i = 0; i < n; ++i) {
            std::string label = std::to_string(nodeValues[i]);
            graph.addNode(label, positions[i]);
            
            if (oldLockedLabels.find(label) != oldLockedLabels.end()) {
                graph.setNodeLocked(graph.getNodes().back().get(), true);
            }
        }
        for (const auto& e : model.getEdges()) {
            graph.addEdge(e.u, e.v, std::to_string(e.w));
        }

        // Ép đồ thị tự thu nhỏ lại 66% để nhìn thoáng hơn
        for (const auto& nodePtr : graph.getNodes()) {
            if (nodePtr) nodePtr->setScale(2.f / 3.f);
        }

        auto seq = std::make_unique<UI::Animations::SequenceAnimation>();
        seq->add(std::make_unique<UI::Animations::WaitAnimation>(0.20f));
        ctx.animManager.addAnimation(std::move(seq));
    }

    void MSTController::handleCreateRandom(int nodeCount, int edgeCount) {
        graph.clear();
        model.clear();
        if (codeViewer) codeViewer->hide();
        if (nodeCount <= 0) return;

        int maxEdges = nodeCount * (nodeCount - 1) / 2;
        edgeCount = std::clamp(edgeCount, std::max(0, nodeCount - 1), maxEdges);

        std::vector<int> nodeValues(nodeCount);
        for (int i = 0; i < nodeCount; ++i) {
            nodeValues[i] = i;
        }

        std::vector<std::tuple<int,int,int>> rawEdges;
        std::set<std::pair<int,int>> used;

        // tạo cây khung ngẫu nhiên trước để bảo đảm connected
        for (int v = 1; v < nodeCount; ++v) {
            int u = std::rand() % v;
            int w = 1 + std::rand() % 99;
            rawEdges.emplace_back(u, v, w);
            used.insert(normPair(u, v));
        }

        // thêm cạnh phụ
        while (static_cast<int>(rawEdges.size()) < edgeCount) {
            int u = std::rand() % nodeCount;
            int v = std::rand() % nodeCount;
            if (u == v) continue;

            auto p = normPair(u, v);
            if (used.count(p)) continue;

            used.insert(p);
            int w = 1 + std::rand() % 99;
            rawEdges.emplace_back(u, v, w);
        }

        lastAlgorithm = "None";
        liveMessage = "Random graph created";
        liveTotalWeight = 0;
        liveSelectedEdgeCount = 0;
        running = false;

        model.buildFromRaw(nodeCount, nodeValues, rawEdges);
        rebuildGraphFromModel();
    }

    void MSTController::handleEditDataFile() {
        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/MSTData.txt";

        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        std::string header =
            "# --- MST VISUALIZER DATA ---\n"
            "# FORMAT IS THE SAME AS MANUAL MODE:\n"
            "# - A line with 1 integer: create a node label\n"
            "# - A line with 3 integers: u v w -> create/update an edge\n"
            "#   If node label u or v does not exist yet, it will be created automatically.\n"
            "# - A line with 2 integers is considered incomplete and will be ignored.\n"
            "# - Duplicate edges update the weight.\n"
            "#\n"
            "# Example:\n"
            "#   10\n"
            "#   20\n"
            "#   10 20 7\n"
            "#   30 20 4\n"
            "#\n"
            "# This creates nodes: 10, 20, 30\n"
            "# and edges: (10,20,7), (30,20,4)\n"
            "# -----------------------------------\n";

        std::ifstream inFile(filePath);
        std::string userContent;

        if (inFile.is_open()) {
            std::string line;
            while (std::getline(inFile, line)) {
                std::size_t startPos = line.find_first_not_of(" \t\r\n");
                if (startPos == std::string::npos) {
                    if (!userContent.empty()) userContent += "\n";
                } else if (line[startPos] != '#') {
                    userContent += line + "\n";
                }
            }
            inFile.close();
        }

        std::ofstream outFile(filePath);
        if (outFile.is_open()) {
            outFile << header << userContent;
            outFile.close();
        }

        std::system(("start notepad " + filePath).c_str());
    }

    void MSTController::handleCreateFromFile() {
        if (codeViewer) codeViewer->hide();

        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/MSTData.txt";

        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::ofstream outFile(filePath);
            if (outFile.is_open()) {
                outFile << "# --- MST VISUALIZER DATA ---\n"
                        << "# Same format as manual mode:\n"
                        << "# x       -> node label\n"
                        << "# u v w   -> weighted edge between labels u and v\n"
                        << "# -----------------------------------\n";
                outFile.close();
            }
            std::system(("start notepad " + filePath).c_str());
            return;
        }

        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(line);
        }
        file.close();

        std::vector<int> nodeValues;
        std::vector<std::tuple<int,int,int>> rawEdges;
        std::string err;

        if (!parseAutoGraphLines(lines, nodeValues, rawEdges, err)) {
            std::cout << "[UI LOG] MST file parse failed: " << err << '\n';
            return;
        }

        if (nodeValues.empty()) {
            std::cout << "[UI LOG] MST file has no valid nodes.\n";
            return;
        }

        lastAlgorithm = "None";
        lastTotalWeight = 0;
        lastSelectedEdgeCount = 0;
        liveMessage = "Graph loaded from file";
        liveTotalWeight = 0;
        liveSelectedEdgeCount = 0;
        running = false;

        model.buildFromRaw(static_cast<int>(nodeValues.size()), nodeValues, rawEdges);
        rebuildGraphFromModel();
    }

    void MSTController::handleCreateManual(int nodeCount,
                                       const std::vector<int>& nodeValues,
                                       const std::vector<std::tuple<int,int,int>>& rawEdges) {
        if (codeViewer) codeViewer->hide();

        if (nodeCount <= 0) {
            model.clear();
            graph.clear();
            return;
        }

        if (static_cast<int>(nodeValues.size()) != nodeCount) return;

        for (const auto& [u, v, w] : rawEdges) {
            if (u < 0 || u >= nodeCount || v < 0 || v >= nodeCount || u == v) {
                std::cout << "[UI LOG] Invalid manual edge: " << u << ' ' << v << ' ' << w << '\n';
                return;
            }
        }
        lastAlgorithm = "None";
        lastTotalWeight = 0;
        lastSelectedEdgeCount = 0;

        model.buildFromRaw(nodeCount, nodeValues, rawEdges);
        rebuildGraphFromModel();
    }

    void MSTController::handleRunKruskal() {
        if (!ctx.animManager.empty()) {
            ctx.animManager.clearAll();
        }

        graph.resetVisuals();
        interruptRunning("Restarting with Kruskal");
        if (model.getNodeCount() <= 0 || model.getEdges().empty()) return;

        graph.resetVisuals();

        auto codeDef = Core::DSA::PseudoCode::MST::kruskal();
        if (codeViewer) codeViewer->setCode(codeDef);

        auto seq = std::make_unique<UI::Animations::SequenceAnimation>();

        auto addCode = [&](const std::string& label) {
            if (!codeViewer) return;
            int idx = codeDef.lineIndex(label);
            if (idx < 0) return;
            auto* viewer = codeViewer;
            seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                [viewer, idx]() { viewer->highlightLine(idx); }
            ));
        };

        addCode("sort_edges");
        seq->add(std::make_unique<UI::Animations::WaitAnimation>(DEFAULT_WAIT));

        auto steps = model.runKruskal();
        lastAlgorithm = "Kruskal";
        lastTotalWeight = 0;
        lastSelectedEdgeCount = 0;

        for (const auto& st : steps) {
            if (st.type == Core::DSA::MSTStep::Type::AcceptEdge) {
                lastTotalWeight += st.w;
                lastSelectedEdgeCount++;
            }
        }

        liveMessage = "Running Kruskal";
        liveTotalWeight = 0;
        liveSelectedEdgeCount = 0;
        running = true;

        for (const auto& st : steps) {
            if (st.type == Core::DSA::MSTStep::Type::ConsiderEdge) {
                addCode("consider");
                seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, st]() {
                        liveMessage = "Considering edge (" +
                                    std::to_string(st.u) + ", " +
                                    std::to_string(st.v) + ", " +
                                    std::to_string(st.w) + ")";
                    }
                ));

                if (auto* edge = getEdgeById(st.edgeId)) {
                    seq->add(std::make_unique<UI::Animations::EdgeColorAnimation>(
                        edge, edge->getColor(), Config::UI::Colors::EdgeHighlight, 0.18f
                    ));
                    seq->add(std::make_unique<UI::Animations::EdgeScaleAnimation>(
                        edge, edge->getThickness(), MST_EDGE_CONSIDER_THICK, 0.14f
                    ));
                }

                auto* nu = graph.getNode(st.u);
                auto* nv = graph.getNode(st.v);
                if (nu || nv) {
                    auto par = std::make_unique<UI::Animations::ParallelAnimation>();
                    if (nu) par->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(nu, 0.16f));
                    if (nv) par->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(nv, 0.16f));
                    seq->add(std::move(par));
                }

                addCode("check_cycle");
                seq->add(std::make_unique<UI::Animations::WaitAnimation>(DEFAULT_WAIT));
            }
            else if (st.type == Core::DSA::MSTStep::Type::AcceptEdge) {
                addCode("accept");
                seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, st]() {
                        liveSelectedEdgeCount++;
                        liveTotalWeight += st.w;
                        liveMessage = "Accepted edge (" +
                                    std::to_string(st.u) + ", " +
                                    std::to_string(st.v) + ", " +
                                    std::to_string(st.w) + ")";
                    }
                ));

                if (auto* edge = getEdgeById(st.edgeId)) {
                    seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                        [edge]() {
                            edge->setColor(MST_EDGE_ACCEPT);
                            edge->setThickness(MST_EDGE_THICK);
                        }
                    ));
                }

                addCode("union");
                seq->add(std::make_unique<UI::Animations::WaitAnimation>(DEFAULT_WAIT));

                auto* nu = graph.getNode(st.u);
                auto* nv = graph.getNode(st.v);
                if (nu || nv) {
                    auto par = std::make_unique<UI::Animations::ParallelAnimation>();
                    if (nu) par->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nu, 0.10f));
                    if (nv) par->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nv, 0.10f));
                    seq->add(std::move(par));
                }
            }
            else if (st.type == Core::DSA::MSTStep::Type::RejectEdge) {
                addCode("reject");
                seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, st]() {
                        liveMessage = "Rejected edge (" +
                                    std::to_string(st.u) + ", " +
                                    std::to_string(st.v) + ", " +
                                    std::to_string(st.w) + ")";
                    }
                ));

                if (auto* edge = getEdgeById(st.edgeId)) {
                    seq->add(std::make_unique<UI::Animations::EdgeColorAnimation>(
                        edge, edge->getColor(), MST_EDGE_REJECT, 0.16f
                    ));
                    seq->add(std::make_unique<UI::Animations::WaitAnimation>(0.10f));
                    seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                        [edge]() {
                            edge->setColor(Config::UI::Colors::EdgeFill);
                            edge->setThickness(Config::UI::EDGE_THICKNESS);
                        }
                    ));
                }

                auto* nu = graph.getNode(st.u);
                auto* nv = graph.getNode(st.v);
                if (nu || nv) {
                    auto par = std::make_unique<UI::Animations::ParallelAnimation>();
                    if (nu) par->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nu, 0.10f));
                    if (nv) par->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nv, 0.10f));
                    seq->add(std::move(par));
                }
            }
            else if (st.type == Core::DSA::MSTStep::Type::Finish) {
                addCode("finish");
                seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this]() {
                        running = false;
                        liveMessage = "Kruskal finished";
                    }
                ));
                seq->add(std::make_unique<UI::Animations::WaitAnimation>(0.45f));
            }
        }

        if (codeViewer) {
            auto* viewer = codeViewer;
            seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                [viewer]() { viewer->hide(); }
            ));
        }

        ctx.animManager.addAnimation(std::move(seq));
    }

    void MSTController::handleRunPrim(int startLabel) {
        if (!ctx.animManager.empty()) {
            ctx.animManager.clearAll();
        }

        graph.resetVisuals();
        interruptRunning("Restarting with Prim");
        if (model.getNodeCount() <= 0 || model.getEdges().empty()) return;

        const auto& values = model.getNodeValues();
        auto it = std::find(values.begin(), values.end(), startLabel);
        if (it == values.end()) {
            std::cout << "[UI LOG] Start node label not found: " << startLabel << '\n';
            return;
        }

        int startNode = static_cast<int>(std::distance(values.begin(), it));

        graph.resetVisuals();

        auto codeDef = Core::DSA::PseudoCode::MST::prim();
        if (codeViewer) codeViewer->setCode(codeDef);

        auto seq = std::make_unique<UI::Animations::SequenceAnimation>();

        auto addCode = [&](const std::string& label) {
            if (!codeViewer) return;
            int idx = codeDef.lineIndex(label);
            if (idx < 0) return;
            auto* viewer = codeViewer;
            seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                [viewer, idx]() { viewer->highlightLine(idx); }
            ));
        };

        auto steps = model.runPrim(startNode);
        std::set<int> visitedNodes;
        for (const auto& st : steps) {
            if (st.type == Core::DSA::MSTStep::Type::VisitNode && st.node >= 0) {
                visitedNodes.insert(st.node);
            }
        }

        lastAlgorithm = "Prim";
        lastTotalWeight = 0;
        lastSelectedEdgeCount = 0;

        for (const auto& st : steps) {
            if (st.type == Core::DSA::MSTStep::Type::AcceptEdge) {
                lastTotalWeight += st.w;
                lastSelectedEdgeCount++;
            }
        }

        liveMessage = "Running Prim";
        liveTotalWeight = 0;
        liveSelectedEdgeCount = 0;
        running = true;

        // [BƯỚC 1]: KHAI BÁO MẢNG THEO DÕI NGAY TRƯỚC VÒNG LẶP
        std::vector<bool> isVisited(graph.getNodes().size(), false);

        for (const auto& st : steps) {
            if (st.type == Core::DSA::MSTStep::Type::VisitNode) {
                // [BƯỚC 2]: CẬP NHẬT MẢNG THEO DÕI
                isVisited[st.node] = true; 
                
                addCode(st.codeLabel);

                seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, st]() {
                        liveMessage = "Visited node " + std::to_string(st.node);
                    }
                ));

                auto* node = graph.getNode(st.node);
                if (node) {
                    seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                        [node]() {
                            node->setFillColor(MST_NODE_VISITED);
                            node->setLabelColor(MST_NODE_VISITED_TEXT);
                        }
                    ));
                    seq->add(std::make_unique<UI::Animations::NodeScaleAnimation>(node, 1.0f, 1.15f, 0.12f));
                    seq->add(std::make_unique<UI::Animations::NodeScaleAnimation>(node, 1.15f, 1.0f, 0.12f));
                }

                seq->add(std::make_unique<UI::Animations::WaitAnimation>(DEFAULT_WAIT));
            }
            else if (st.type == Core::DSA::MSTStep::Type::RelaxEdge) {
                addCode("push_edge");

                seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, st]() {
                        liveMessage = "Push edge (" +
                                    std::to_string(st.u) + ", " +
                                    std::to_string(st.v) + ", " +
                                    std::to_string(st.w) + ")";
                    }
                ));

                if (auto* edge = getEdgeById(st.edgeId)) {
                    seq->add(std::make_unique<UI::Animations::EdgeColorAnimation>(
                        edge, edge->getColor(), sf::Color(120, 170, 255), 0.12f
                    ));
                    seq->add(std::make_unique<UI::Animations::WaitAnimation>(0.06f));
                    seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                        [edge]() {
                            if (edge->getColor() != MST_EDGE_ACCEPT) {
                                edge->setColor(Config::UI::Colors::EdgeFill);
                            }
                        }
                    ));
                }
            }
            else if (st.type == Core::DSA::MSTStep::Type::ConsiderEdge) {
                addCode("consider");

                seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, st]() {
                        liveMessage = "Considering edge (" +
                                    std::to_string(st.u) + ", " +
                                    std::to_string(st.v) + ", " +
                                    std::to_string(st.w) + ")";
                    }
                ));

                if (auto* edge = getEdgeById(st.edgeId)) {
                    seq->add(std::make_unique<UI::Animations::EdgeColorAnimation>(
                        edge, edge->getColor(), Config::UI::Colors::EdgeHighlight, 0.18f
                    ));
                    seq->add(std::make_unique<UI::Animations::EdgeScaleAnimation>(
                        edge, edge->getThickness(), MST_EDGE_CONSIDER_THICK, 0.14f
                    ));
                }

                auto* to = graph.getNode(st.v);
                if (to) {
                    seq->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(to, 0.14f));
                }

                seq->add(std::make_unique<UI::Animations::WaitAnimation>(DEFAULT_WAIT));
            }
            else if (st.type == Core::DSA::MSTStep::Type::AcceptEdge) {
                // [BƯỚC 3]: ĐÁNH DẤU CHẮC CHẮN NODE NÀY ĐÃ THUỘC MST
                isVisited[st.v] = true;
                
                addCode("accept");
                seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, st]() {
                        liveSelectedEdgeCount++;
                        liveTotalWeight += st.w;
                        liveMessage = "Accepted edge (" +
                                    std::to_string(st.u) + ", " +
                                    std::to_string(st.v) + ", " +
                                    std::to_string(st.w) + ")";
                    }
                ));

                if (auto* edge = getEdgeById(st.edgeId)) {
                    seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                        [edge]() {
                            edge->setColor(MST_EDGE_ACCEPT);
                            edge->setThickness(MST_EDGE_THICK);
                        }
                    ));
                }

                auto* to = graph.getNode(st.v);
                if (to) {
                    seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                        [to]() {
                            to->setFillColor(MST_NODE_VISITED);
                            to->setLabelColor(MST_NODE_VISITED_TEXT);
                        }
                    ));
                }
            }
            else if (st.type == Core::DSA::MSTStep::Type::RejectEdge) {
                addCode("reject");

                seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, st]() {
                        liveMessage = "Rejected edge (" +
                                    std::to_string(st.u) + ", " +
                                    std::to_string(st.v) + ", " +
                                    std::to_string(st.w) + ")";
                    }
                ));

                if (auto* edge = getEdgeById(st.edgeId)) {
                    seq->add(std::make_unique<UI::Animations::EdgeColorAnimation>(
                        edge, edge->getColor(), MST_EDGE_REJECT, 0.16f
                    ));
                    seq->add(std::make_unique<UI::Animations::WaitAnimation>(0.10f));
                    seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                        [edge]() {
                            if (edge->getColor() != MST_EDGE_ACCEPT) {
                                edge->setColor(Config::UI::Colors::EdgeFill);
                                edge->setThickness(Config::UI::EDGE_THICKNESS);
                            }
                        }
                    ));
                }

                auto* to = graph.getNode(st.v);
                if (to) {
                    if (isVisited[st.v]) {
                        seq->add(std::make_unique<UI::Animations::NodeColorAnimation>(
                            to, MST_NODE_VISITED, MST_NODE_VISITED_TEXT, 0.10f
                        ));
                    } else {
                        // Chưa thuộc MST mới xài NodeUnhighlightAnimation về màu mặc định
                        seq->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(to, 0.10f));
                    }
                }
            }
            else if (st.type == Core::DSA::MSTStep::Type::Finish) {
                addCode("finish");
                seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this]() {
                        running = false;
                        liveMessage = "Prim finished";
                    }
                ));
                seq->add(std::make_unique<UI::Animations::WaitAnimation>(0.45f));
            }
        }

        if (codeViewer) {
            auto* viewer = codeViewer;
            seq->add(std::make_unique<UI::Animations::CallbackAnimation>(
                [viewer]() { viewer->hide(); }
            ));
        }

        ctx.animManager.addAnimation(std::move(seq));
    }

    void MSTController::handleClearAll() {
        if (codeViewer) codeViewer->hide();
        lastAlgorithm = "None";
        lastTotalWeight = 0;
        lastSelectedEdgeCount = 0;
        graph.clearEdges();
        int currentSize = graph.getNodes().size();
        for (int i = 0; i < currentSize; ++i) {
            graph.removeNodeAt(0); 
        }
        model.clear();
    }

    void MSTController::interruptRunning(const std::string& reason) {
        running = false;
        liveMessage = reason;
        liveTotalWeight = 0;
        liveSelectedEdgeCount = 0;

        
        if (codeViewer) {
            codeViewer->hide();
        }
    }
} // namespace Controllers