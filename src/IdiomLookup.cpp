#include "IdiomLookup.h"
#include <fstream>
#include <cctype>
#include <nlohmann/json.hpp>

std::string IdiomLookup::normalize(const std::string& s) {
    std::string out;
    for (unsigned char c : s) {
        if (std::isalpha(c)) {
            out += static_cast<char>(std::tolower(c));
        } else if (!out.empty() && out.back() != ' ') {
            out += ' ';
        }
    }
    if (!out.empty() && out.back() == ' ')
        out.pop_back();
    return out;
}

bool IdiomLookup::Load(const std::string& filename) {
    std::ifstream f(filename);
    if (!f) return false;
    nlohmann::json j;
    try { f >> j; } catch (...) { return false; }
    for (auto& [key, val] : j.items()) {
        if (!val.is_object()) continue;
        IdiomEntry e;
        e.proto     = val.value("proto",     std::string{});
        e.certainty = val.value("certainty", std::string{"medium"});
        e.gloss     = val.value("gloss",     std::string{});
        entries[normalize(key)] = std::move(e);
    }
    return !entries.empty();
}

const IdiomEntry* IdiomLookup::Find(const std::string& phrase) const {
    auto it = entries.find(normalize(phrase));
    return it != entries.end() ? &it->second : nullptr;
}
