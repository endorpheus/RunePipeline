#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct CompoundEntry {
    std::vector<std::string> components; // PGmc root forms
    std::string gloss;                   // human-readable decomposition
};

// Falls back to compound PGmc forms for words not in the primary lexicon.
class SemanticDecomposer {
public:
    bool Load(const std::string& filename);
    // Returns nullptr if word has no compound mapping.
    const CompoundEntry* Find(const std::string& english) const;
    // Joins components into a single PGmc compound string.
    static std::string Compound(const CompoundEntry& entry);

private:
    std::unordered_map<std::string, CompoundEntry> entries; // keys lowercase
};
