#include "Core/DSA/Trie.hpp"

namespace Core::DSA {

    Trie::Trie() {
        rootIndex = allocateNode(); 
    }

    int Trie::allocateNode() {
        int index;
        if (!freeIndices.empty()) {
            index = freeIndices.back();
            freeIndices.pop_back();
        } else {
            index = pool.size();
            pool.push_back(TrieNode());
        }

        pool[index].isActive = true;
        pool[index].isEndOfWord = false;
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            pool[index].children[i] = -1;
        }
        return index;
    }

    void Trie::freeNode(int index) {
        if (index >= 0 && index < pool.size()) {
            pool[index].isActive = false;
            freeIndices.push_back(index);
        }
    }

    void Trie::clear() {
        pool.clear();
        freeIndices.clear();
        rootIndex = allocateNode(); 
    }

    void Trie::insert(const std::string& word) {
        int curr = rootIndex;
        for (char c : word) {
            int charIndex = c - 'a';
            
            if (pool[curr].children[charIndex] == -1) {
                int newNodeIdx = allocateNode(); 
                pool[curr].children[charIndex] = newNodeIdx;
            }
            
            curr = pool[curr].children[charIndex];
        }
        pool[curr].isEndOfWord = true;
    }

    bool Trie::search(const std::string& word) const {
        int curr = rootIndex;
        for (char c : word) {
            int charIndex = c - 'a';
            if (pool[curr].children[charIndex] == -1) {
                return false; 
            }
            curr = pool[curr].children[charIndex];
        }
        return pool[curr].isEndOfWord; 
    }

    bool Trie::startsWith(const std::string& prefix) const {
        int curr = rootIndex;
        for (char c : prefix) {
            int charIndex = c - 'a';
            if (pool[curr].children[charIndex] == -1) {
                return false;
            }
            curr = pool[curr].children[charIndex];
        }
        return true; 
    }

    bool Trie::deleteWord(const std::string& word) {
        return deleteHelper(rootIndex, word, 0);
    }

    bool Trie::deleteHelper(int currentIndex, const std::string& word, int depth) {
        if (currentIndex == -1) return false;

        if (depth == word.length()) {
            if (!pool[currentIndex].isEndOfWord) return false; 
            
            pool[currentIndex].isEndOfWord = false; 
            
            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                if (pool[currentIndex].children[i] != -1) return false; 
            }
            return true; 
        }

        int charIndex = word[depth] - 'a';
        int childIndex = pool[currentIndex].children[charIndex];
        
        if (deleteHelper(childIndex, word, depth + 1)) {
            freeNode(childIndex);
            pool[currentIndex].children[charIndex] = -1; 

            if (pool[currentIndex].isEndOfWord) return false;
            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                if (pool[currentIndex].children[i] != -1) return false;
            }
            return true; 
        }

        return false;
    }

} // namespace Core::DSA