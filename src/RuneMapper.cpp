#include "RuneMapper.h"
#include <cctype>

// Fold a UTF-8 key to its lowercase equivalent for rune-table lookup.
static std::string foldKey(const std::string& key) {
    if (key.size() == 1) {
        if (key[0] == 'R') return key; // Proto-Norse R phoneme — keep uppercase for Algiz
        return std::string(1, static_cast<char>(
            std::tolower(static_cast<unsigned char>(key[0]))));
    }
    if (key == "\xc3\x9e") return "\xc3\xbe"; // Þ → þ
    if (key == "\xc3\x90") return "\xc3\xb0"; // Ð → ð
    if (key == "\xc5\x8a") return "\xc5\x8b"; // Ŋ → ŋ
    return key;
}

std::string RuneMapper::ToRunes(const std::string& proto) const {
    std::string out;
    size_t i = 0;
    while (i < proto.size()) {
        unsigned char c = static_cast<unsigned char>(proto[i]);
        int len = 1;
        if      (c >= 0xF0) len = 4;
        else if (c >= 0xE0) len = 3;
        else if (c >= 0xC0) len = 2;
        if (i + len > proto.size()) break;

        std::string key = foldKey(proto.substr(i, len));
        auto it = runes.find(key);
        if (it != runes.end())
            out += it->second;
        i += len;
    }
    return out;
}

std::vector<RuneToken> RuneMapper::ToRuneTokens(const std::string& proto) const {
    std::vector<RuneToken> out;
    size_t i = 0;
    while (i < proto.size()) {
        unsigned char c = static_cast<unsigned char>(proto[i]);
        int len = 1;
        if      (c >= 0xF0) len = 4;
        else if (c >= 0xE0) len = 3;
        else if (c >= 0xC0) len = 2;
        if (i + len > proto.size()) break;

        std::string rawKey = proto.substr(i, len);
        std::string key    = foldKey(rawKey);

        auto ri = runes.find(key);
        auto ii = info.find(key);
        if (ri != runes.end()) {
            RuneToken tok;
            tok.rune       = ri->second;
            tok.sourceChar = rawKey;
            if (ii != info.end()) {
                tok.name    = ii->second.name;
                tok.phoneme = ii->second.phoneme;
            }
            out.push_back(std::move(tok));
        }
        i += len;
    }
    return out;
}

const std::vector<std::pair<std::string, RuneInfo>>& RuneMapper::AllRunes() {
    static const std::vector<std::pair<std::string, RuneInfo>> all = {
        {"ᚠ", {"Fehu",    "f",  "cattle, wealth"}},
        {"ᚢ", {"Uruz",    "u",  "aurochs, strength"}},
        {"ᚦ", {"Thurisaz","þ",  "giant, thorn"}},
        {"ᚨ", {"Ansuz",   "a",  "god, mouth"}},
        {"ᚱ", {"Raidho",  "r",  "ride, journey"}},
        {"ᚲ", {"Kaunan",  "k",  "torch, fire"}},
        {"ᚷ", {"Gebo",    "g",  "gift"}},
        {"ᚹ", {"Wunjo",   "w",  "joy, hope"}},
        {"ᚺ", {"Hagalaz", "h",  "hail, disruption"}},
        {"ᚾ", {"Naudiz",  "n",  "need, constraint"}},
        {"ᛁ", {"Isaz",    "i",  "ice, stillness"}},
        {"ᛃ", {"Jera",    "j",  "year, harvest"}},
        {"ᛇ", {"Ihwaz",   "ï",  "yew tree, endurance"}},
        {"ᛈ", {"Perthro", "p",  "lot cup, fate"}},
        {"ᛉ", {"Algiz",   "z/R","elk, protection"}},
        {"ᛊ", {"Sowilo",  "s",  "sun, victory"}},
        {"ᛏ", {"Tiwaz",   "t",  "Tyr, justice"}},
        {"ᛒ", {"Berkanan","b",  "birch, growth"}},
        {"ᛖ", {"Ehwaz",   "e",  "horse, partnership"}},
        {"ᛗ", {"Mannaz",  "m",  "man, humanity"}},
        {"ᛚ", {"Laguz",   "l",  "water, flow"}},
        {"ᛜ", {"Ingwaz",  "ŋ",  "Ing/Freyr, fertility"}},
        {"ᛞ", {"Dagaz",   "d",  "day, breakthrough"}},
        {"ᛟ", {"Othala",  "o",  "heritage, estate"}},
    };
    return all;
}
