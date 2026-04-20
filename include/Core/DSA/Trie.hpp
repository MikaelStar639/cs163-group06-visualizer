#pragma once
#include <vector>
#include <string>

namespace Core::DSA {

    const int ALPHABET_SIZE = 26;

    struct TrieNode {
        int children[ALPHABET_SIZE];
        bool isEndOfWord;           
        bool isActive;              
    };

    class Trie {
    private:
        std::vector<TrieNode> pool;   
        std::vector<int> freeIndices; 
        int rootIndex;                

        int allocateNode(); 
        void freeNode(int index);

        bool deleteHelper(int currentIndex, const std::string& word, int depth);

    public:
        Trie();
        
        void clear();

        void insert(const std::string& word);
        bool search(const std::string& word) const;
        bool startsWith(const std::string& prefix) const;
        bool deleteWord(const std::string& word);

        int getRootIndex() const { return rootIndex; }
        const std::vector<TrieNode>& getPool() const { return pool; }
    };

} // namespace Core::DSA