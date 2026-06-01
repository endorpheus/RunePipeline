#pragma once
#include <string>
#include <unordered_map>

struct Entry {
    std::string proto;
    std::string certainty;
    std::string note;     // optional scholarly note
};

class Lexicon {
public:
    bool Load(const std::string& filename);
    // Case-insensitive lookup; returns nullptr if not found.
    const Entry* Find(const std::string& english) const;

private:
    std::unordered_map<std::string, Entry> entries; // keys stored lowercase
};
