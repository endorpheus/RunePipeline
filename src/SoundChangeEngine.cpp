#include "SoundChangeEngine.h"

// Proto-Germanic → Proto-Norse phonological rules, applied in order.
// Sources: Ringe (2006), Kroonen (2013), Fortson (2010).
SoundChangeEngine::SoundChangeEngine() {
    // 1. Rhotacism: *z → *R everywhere.
    //    This is the defining PGmc→Proto-Norse change; the new phoneme *R
    //    (distinct from *r) is written with the Algiz rune ᛉ in Elder Futhark.
    rules.push_back({std::regex("z"), "R", "Rhotacism: z → R"});

    // 2. eu → iu before a syllable containing a high vowel (i, j, u).
    //    Simplified: treat any eu followed within 4 chars by i, j, or u.
    //    We use a lookahead-style substitution: eu([^aeo]*[iju]) → iu\1
    rules.push_back({
        std::regex("eu([^aeo]{0,4}[iju])"),
        "iu$1",
        "eu-umlaut: eu → iu before high vowel syllable"
    });

    // 3. Word-final *-ją → *-ja (loss of final nasal in ja-stems).
    //    Only in word-final position.
    rules.push_back({std::regex("ją$"), "ja", "Final -ją → -ja"});

    // 4. *-ōją → *-ōja (same, ō-ja stems).
    rules.push_back({std::regex("ōją$"), "ōja", "Final -ōją → -ōja"});
}

std::string SoundChangeEngine::Apply(const std::string& pgmc) const {
    std::string form = pgmc;
    for (const auto& rule : rules)
        form = std::regex_replace(form, rule.pattern, rule.replacement);
    return form;
}
