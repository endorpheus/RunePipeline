#include "Lexicon.h"
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

bool Lexicon::Load(const std::string& filename) {
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
        Entry e;
        e.proto     = value.value("proto",     std::string{});
        e.certainty = value.value("certainty", std::string{});
        e.note      = value.value("note",      std::string{});
        entries[toLower(word)] = std::move(e);
    }
    return true;
}

const Entry* Lexicon::Find(const std::string& english) const {
    auto it = entries.find(toLower(english));
    return it == entries.end() ? nullptr : &it->second;
}
