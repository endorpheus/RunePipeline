#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct RuneInfo {
    std::string name;
    std::string phoneme;
    std::string meaning;
};

struct RuneToken {
    std::string rune;
    std::string name;
    std::string phoneme;
    std::string sourceChar; // the proto-form character(s) that produced this rune
};

class RuneMapper {
public:
    std::string              ToRunes(const std::string& proto) const;
    std::vector<RuneToken>   ToRuneTokens(const std::string& proto) const;

    // All 24 Elder Futhark runes for the reference panel.
    static const std::vector<std::pair<std::string, RuneInfo>>& AllRunes();

private:
    // Maps UTF-8 grapheme → rune glyph (for ToRunes)
    const std::unordered_map<std::string, std::string> runes = {
        {"f","ᚠ"}, {"u","ᚢ"}, {"þ","ᚦ"}, {"a","ᚨ"}, {"r","ᚱ"},
        {"k","ᚲ"}, {"g","ᚷ"}, {"w","ᚹ"}, {"h","ᚺ"}, {"n","ᚾ"},
        {"i","ᛁ"}, {"j","ᛃ"}, {"ï","ᛇ"}, {"p","ᛈ"}, {"z","ᛉ"},
        {"s","ᛊ"}, {"t","ᛏ"}, {"b","ᛒ"}, {"e","ᛖ"}, {"m","ᛗ"},
        {"l","ᛚ"}, {"ŋ","ᛜ"}, {"d","ᛞ"}, {"o","ᛟ"},
        {"R","ᛉ"},
        {"ð","ᚦ"},
        {"ā","ᚨ"}, {"ē","ᛖ"}, {"ī","ᛁ"}, {"ō","ᛟ"}, {"ū","ᚢ"},
        {"ą","ᚨ"}, {"ǫ","ᛟ"},
        {"ô","ᛟ"}, {"ê","ᛖ"}, {"î","ᛁ"}, {"û","ᚢ"},
        {"c","ᚲ"}, {"v","ᚠ"},
    };

    // Maps UTF-8 grapheme → {name, phoneme, meaning} for rune tooltips
    const std::unordered_map<std::string, RuneInfo> info = {
        {"f", {"Fehu",    "f",  "cattle, wealth"}},
        {"u", {"Uruz",    "u",  "aurochs, strength"}},
        {"þ", {"Thurisaz","þ",  "giant, thorn"}},
        {"ð", {"Thurisaz","þ",  "giant, thorn"}},
        {"a", {"Ansuz",   "a",  "god, mouth"}},
        {"r", {"Raidho",  "r",  "ride, journey"}},
        {"k", {"Kaunan",  "k",  "torch, fire"}},
        {"c", {"Kaunan",  "k",  "torch, fire"}},
        {"g", {"Gebo",    "g",  "gift"}},
        {"w", {"Wunjo",   "w",  "joy"}},
        {"h", {"Hagalaz", "h",  "hail"}},
        {"n", {"Naudiz",  "n",  "need, constraint"}},
        {"i", {"Isaz",    "i",  "ice"}},
        {"j", {"Jera",    "j",  "year, harvest"}},
        {"ï", {"Ihwaz",   "ï",  "yew tree, endurance"}},
        {"p", {"Perthro", "p",  "lot cup, fate"}},
        {"z", {"Algiz",   "z/R","elk, protection"}},
        {"R", {"Algiz",   "R",  "elk, protection (rhotacised *z)"}},
        {"s", {"Sowilo",  "s",  "sun"}},
        {"t", {"Tiwaz",   "t",  "Tyr, justice"}},
        {"b", {"Berkanan","b",  "birch, growth"}},
        {"e", {"Ehwaz",   "e",  "horse"}},
        {"m", {"Mannaz",  "m",  "man, humanity"}},
        {"l", {"Laguz",   "l",  "water, lake"}},
        {"ŋ", {"Ingwaz",  "ŋ",  "Ing/Freyr, fertility"}},
        {"d", {"Dagaz",   "d",  "day, breakthrough"}},
        {"o", {"Othala",  "o",  "heritage, estate"}},
        // Long vowels — same rune info as their short equivalents
        {"ā", {"Ansuz",   "ā",  "god, mouth (long)"}},
        {"ē", {"Ehwaz",   "ē",  "horse (long)"}},
        {"ī", {"Isaz",    "ī",  "ice (long)"}},
        {"ō", {"Othala",  "ō",  "heritage, estate (long)"}},
        {"ū", {"Uruz",    "ū",  "aurochs, strength (long)"}},
        {"ą", {"Ansuz",   "ą",  "god, mouth (nasal)"}},
        {"ǫ", {"Othala",  "ǫ",  "heritage (nasal)"}},
        {"ô", {"Othala",  "ô",  "heritage, estate"}},
        {"ê", {"Ehwaz",   "ê",  "horse"}},
        {"î", {"Isaz",    "î",  "ice"}},
        {"û", {"Uruz",    "û",  "aurochs, strength"}},
        {"v", {"Fehu",    "v",  "cattle, wealth"}},
    };
};
