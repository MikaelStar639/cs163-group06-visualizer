#pragma once
#include <vector>
#include <string>
#include <stdexcept>

namespace Core::DSA {

    // A single line of pseudocode with metadata
    struct PseudoCodeLine {
        std::string text;        // Display text
        std::string label;       // Unique ID for lookup (avoids magic line numbers)
        int indentLevel = 0;     // Indent depth for rendering
    };

    // A complete pseudocode definition for one operation
    struct PseudoCodeDef {
        std::string name;                     // Operation name (e.g. "Insert Head")
        std::vector<PseudoCodeLine> lines;    // All lines

        // Lookup line index by label — returns -1 if not found
        int lineIndex(const std::string& label) const {
            for (int i = 0; i < static_cast<int>(lines.size()); ++i) {
                if (lines[i].label == label) return i;
            }
            return -1;
        }

        // Convert to raw strings for backward compatibility with PseudoCodeViewer
        std::vector<std::string> toStringVector() const {
            std::vector<std::string> result;
            result.reserve(lines.size());
            for (auto& line : lines) {
                std::string prefix(line.indentLevel * 2, ' ');
                result.push_back(prefix + line.text);
            }
            return result;
        }
    };

} // namespace Core::DSA

namespace Core::DSA::PseudoCode {

    // ===== LINKED LIST =====
    namespace LinkedList {

        inline PseudoCodeDef insertHead() {
            return { "Insert Head", {
                { "newNode = new Node(val)",  "create_node",  0 },
                { "newNode.next = head",      "link_next",    0 },
                { "head = newNode",           "update_head",  0 },
            }};
        }

        inline PseudoCodeDef insertTail() {
            return { "Insert Tail", {
                { "if head == null:",            "check_null",    0 },
                { "head = new Node(val)",        "insert_empty",  1 },
                { "else:",                       "else",          0 },
                { "curr = head",                 "init_curr",     1 },
                { "while curr.next != null:",    "loop_cond",     1 },
                { "curr = curr.next",            "advance",       2 },
                { "curr.next = new Node(val)",   "insert_tail",   1 },
            }};
        }

        inline PseudoCodeDef insertAt() {
            return { "Insert At Position", {
                { "if pos < 0 or pos > size: return",  "bounds_check",  0 },
                { "if pos == 0: insertHead(val)",       "check_head",    0 },
                { "else:",                              "else",          0 },
                { "pre = head",                         "init_pre",      1 },
                { "for i = 0 to pos - 2:",              "loop_cond",     1 },
                { "pre = pre.next",                     "advance",       2 },
                { "newNode = new Node(val)",             "create_node",   1 },
                { "newNode.next = pre.next",             "link_next",     1 },
                { "pre.next = newNode",                  "link_pre",      1 },
            }};
        }

        inline PseudoCodeDef deleteHead() {
            return { "Delete Head", {
                { "if head == null: return",   "check_null",  0 },
                { "temp = head",               "save_temp",   0 },
                { "head = head.next",          "advance",     0 },
                { "delete temp",               "delete",      0 },
            }};
        }

        inline PseudoCodeDef deleteTail() {
            return { "Delete Tail", {
                { "if head == null: return",           "check_null",     0 },
                { "if head.next == null: head = null", "single_node",    0 },
                { "else:",                             "else",           0 },
                { "pre = head",                        "init_pre",       1 },
                { "while pre.next.next != null:",      "loop_cond",      1 },
                { "pre = pre.next",                    "advance",        2 },
                { "del = pre.next",                    "save_del",       1 },
                { "pre.next = null",                   "unlink",         1 },
                { "delete del",                        "delete",         1 },
            }};
        }

        inline PseudoCodeDef deleteAt() {
            return { "Delete At Position", {
                { "if pos < 0 or pos >= size: return",  "bounds_check",  0 },
                { "if pos == 0: deleteHead()",           "check_head",    0 },
                { "else:",                               "else",          0 },
                { "pre = head",                          "init_pre",      1 },
                { "for i = 0 to pos - 2:",               "loop_cond",     1 },
                { "pre = pre.next",                      "advance",       2 },
                { "del = pre.next",                      "save_del",      1 },
                { "pre.next = del.next",                 "unlink",        1 },
                { "delete del",                          "delete",        1 },
            }};
        }

        inline PseudoCodeDef search() {
            return { "Search", {
                { "curr = head",                "init_curr",     0 },
                { "while curr != null:",        "loop_cond",     0 },
                { "if curr.val == target:",     "check_val",     1 },
                { "return FOUND",              "found",         2 },
                { "curr = curr.next",           "advance",       1 },
                { "return NOT FOUND",           "not_found",     0 },
            }};
        }

        inline PseudoCodeDef updateAt() {
            return { "Update At Position", {
                { "if pos < 0 or pos >= size: return",  "bounds_check",  0 },
                { "curr = head",                         "init_curr",     0 },
                { "for i = 0 to pos - 1:",               "loop_cond",     0 },
                { "curr = curr.next",                    "advance",       1 },
                { "curr.val = newVal",                   "update_val",    0 },
            }};
        }

        inline PseudoCodeDef updateByValue() {
            return { "Update By Value", {
                { "curr = head",                "init_curr",     0 },
                { "while curr != null:",        "loop_cond",     0 },
                { "if curr.val == oldVal:",     "check_val",     1 },
                { "curr.val = newVal",          "update_val",    2 },
                { "return FOUND",              "found",         2 },
                { "curr = curr.next",           "advance",       1 },
                { "return NOT FOUND",           "not_found",     0 },
            }};
        }

    } // namespace LinkedList

} // namespace Core::DSA::PseudoCode
