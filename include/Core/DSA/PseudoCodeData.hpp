#pragma once
#include <vector>
#include <string>

namespace Core::DSA::PseudoCode {

    // ===== LINKED LIST =====

    inline std::vector<std::string> insertHead() {
        return {
            "newNode = new Node(val)",
            "newNode.next = head",
            "head = newNode"
        };
    }

    inline std::vector<std::string> insertTail() {
        return {
            "curr = head",
            "while curr.next != null:",
            "  curr = curr.next",
            "newNode = new Node(val)",
            "curr.next = newNode"
        };
    }

    inline std::vector<std::string> insertAt() {
        return {
            "curr = head",
            "for i = 0 to pos - 1:",
            "  curr = curr.next",
            "newNode = new Node(val)",
            "newNode.next = curr.next",
            "curr.next = newNode"
        };
    }

    inline std::vector<std::string> deleteHead() {
        return {
            "temp = head",
            "head = head.next",
            "delete temp"
        };
    }

    inline std::vector<std::string> deleteTail() {
        return {
            "curr = head",
            "while curr.next.next != null:",
            "  curr = curr.next",
            "delete curr.next",
            "curr.next = null"
        };
    }

    inline std::vector<std::string> deleteAt() {
        return {
            "curr = head",
            "for i = 0 to pos - 1:",
            "  curr = curr.next",
            "temp = curr.next",
            "curr.next = temp.next",
            "delete temp"
        };
    }

    inline std::vector<std::string> search() {
        return {
            "curr = head",
            "while curr != null:",
            "  if curr.val == target:",
            "    return FOUND",
            "  curr = curr.next",
            "return NOT FOUND"
        };
    }

    inline std::vector<std::string> updateAt() {
        return {
            "curr = head",
            "for i = 0 to pos:",
            "  curr = curr.next",
            "curr.val = newVal"
        };
    }

    inline std::vector<std::string> updateByValue() {
        return {
            "curr = head",
            "while curr != null:",
            "  if curr.val == oldVal:",
            "    curr.val = newVal",
            "    return",
            "  curr = curr.next"
        };
    }

} // namespace Core::DSA::PseudoCode
