#include "SemanticDecomposer.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

bool SemanticDecomposer::Load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) return false;

    json j;
    try {
        file >> j;
    } catch (const json::exception&) {
        return false;
    }

    for (auto& [word, value] : j.items()) {
        if (!value.is_object()) continue;
        CompoundEntry e;
        for (auto& comp : value["components"])
            e.components.push_back(comp.get<std::string>());
        e.gloss = value.value("gloss", std::string{});
        entries[toLower(word)] = std::move(e);
    }
    return true;
}

const CompoundEntry* SemanticDecomposer::Find(const std::string& english) const {
    auto it = entries.find(toLower(english));
    return it == entries.end() ? nullptr : &it->second;
}

std::string SemanticDecomposer::Compound(const CompoundEntry& entry) {
    std::string result;
    for (const auto& component : entry.components)
        result += component;
    return result;
}
