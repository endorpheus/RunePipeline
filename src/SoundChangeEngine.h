#pragma once
#include <string>
#include <vector>
#include <regex>

struct SoundRule {
    std::regex  pattern;
    std::string replacement;
    std::string description; // for debugging/verbose output
};

// Applies ordered Proto-Germanic → Proto-Norse phonological rules.
class SoundChangeEngine {
public:
    SoundChangeEngine();
    std::string Apply(const std::string& pgmc) const;

private:
    std::vector<SoundRule> rules;
};
