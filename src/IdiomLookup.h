#pragma once
#include <string>
#include <unordered_map>

struct IdiomEntry {
    std::string proto;      // PGmc form expressing the idiom's meaning
    std::string certainty;
    std::string gloss;      // English gloss of the semantic equivalent
};

class IdiomLookup {
public:
    bool Load(const std::string& filename);
    // Returns nullptr if phrase is not a known idiom.
    const IdiomEntry* Find(const std::string& phrase) const;

private:
    std::unordered_map<std::string, IdiomEntry> entries;
    // Lowercases alpha, collapses non-alpha runs to single space, strips ends.
    static std::string normalize(const std::string& s);
};
