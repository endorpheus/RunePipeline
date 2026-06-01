#include "Pipeline.h"
#include <cctype>
#include <vector>
#include <string>

// Returns candidate base forms to try when exact lookup fails.
// Covers the most common English inflectional suffixes.
static std::vector<std::string> stemCandidates(const std::string& w) {
    std::vector<std::string> out;
    int n = static_cast<int>(w.size());

    // -ves → -f / -fe  (wolves→wolf, knives→knife, leaves→leaf)
    if (n > 3 && w.compare(n - 3, 3, "ves") == 0) {
        out.push_back(w.substr(0, n - 3) + "f");
        out.push_back(w.substr(0, n - 3) + "fe");
    }
    // -ies → -y  (armies→army, bodies→body, stories→story)
    if (n > 3 && w.compare(n - 3, 3, "ies") == 0)
        out.push_back(w.substr(0, n - 3) + "y");

    // -es → strip -es or just -s  (foxes→fox, churches→church)
    if (n > 2 && w.compare(n - 2, 2, "es") == 0) {
        out.push_back(w.substr(0, n - 2));
        out.push_back(w.substr(0, n - 1));
    }
    // -s → strip -s  (ravens→raven, kings→king)  but not -ss
    if (n > 2 && w.back() == 's' && w[n - 2] != 's')
        out.push_back(w.substr(0, n - 1));

    // -ing → strip  (singing→sing, walking→walk)
    // also -ing → -e  (riding→ride, making→make)
    if (n > 4 && w.compare(n - 3, 3, "ing") == 0) {
        out.push_back(w.substr(0, n - 3));
        out.push_back(w.substr(0, n - 3) + "e");
    }
    // -ed → strip -ed or -d  (walked→walk, loved→love)
    if (n > 3 && w.compare(n - 2, 2, "ed") == 0) {
        out.push_back(w.substr(0, n - 2));
        out.push_back(w.substr(0, n - 1));
    }
    // -er → strip  (singer→sing, hunter→hunt)
    if (n > 4 && w.compare(n - 2, 2, "er") == 0)
        out.push_back(w.substr(0, n - 2));

    // -est → strip  (strongest→strong, boldest→bold)
    if (n > 5 && w.compare(n - 3, 3, "est") == 0)
        out.push_back(w.substr(0, n - 3));

    // possessive: -'s is already stripped by Tokenize (apostrophe is non-alpha)
    // so "wolf's" tokenizes as "wolfs"; handle like -s above (already covered)

    return out;
}

Pipeline::Pipeline(const Lexicon& lex, const SemanticDecomposer& decomp,
                   const SoundChangeEngine& sce, const RuneMapper& mapper,
                   const IdiomLookup& idioms, bool allowDecompose)
    : lexicon(lex), decomposer(decomp), soundEngine(sce),
      runeMapper(mapper), idiomLookup(idioms), allowDecompose(allowDecompose) {}

PipelineResult Pipeline::Translate(const std::string& word) const {
    PipelineResult r;
    r.english = word;

    // Direct lookup, then try stemmed candidates in priority order.
    std::string foundStem;
    const Entry* e = lexicon.Find(word);
    if (!e) {
        for (const auto& stem : stemCandidates(word)) {
            e = lexicon.Find(stem);
            if (e) { foundStem = stem; break; }
        }
    }
    if (e) {
        r.stem = foundStem;
        r.pgmc        = e->proto;
        r.certainty   = e->certainty;
        r.proto_norse = soundEngine.Apply(r.pgmc);
        r.runes       = runeMapper.ToRunes(r.proto_norse);
        r.runeTokens  = runeMapper.ToRuneTokens(r.proto_norse);
        r.source      = TokenSource::Lexicon;
        return r;
    }

    if (allowDecompose) {
        const CompoundEntry* ce = decomposer.Find(word);
        if (ce) {
            r.pgmc       = SemanticDecomposer::Compound(*ce);
            r.certainty  = "compound";
            r.gloss      = ce->gloss;
            r.proto_norse = soundEngine.Apply(r.pgmc);
            r.runes      = runeMapper.ToRunes(r.proto_norse);
            r.runeTokens = runeMapper.ToRuneTokens(r.proto_norse);
            r.source     = TokenSource::Compound;
            return r;
        }
    }

    r.source    = TokenSource::Unknown;
    r.certainty = "unknown";
    return r;
}

std::vector<PipelineResult> Pipeline::TranslateLine(const std::string& line) const {
    auto tokens = Tokenize(line);
    std::vector<PipelineResult> results;

    size_t i = 0;
    while (i < tokens.size()) {
        // Try longest idiom window first (maximal munch).
        bool matched = false;
        for (size_t len = tokens.size() - i; len >= 2; --len) {
            std::string phrase;
            for (size_t k = i; k < i + len; ++k) {
                if (k > i) phrase += ' ';
                phrase += tokens[k];
            }
            const IdiomEntry* ie = idiomLookup.Find(phrase);
            if (ie) {
                PipelineResult r;
                r.english     = phrase;
                r.pgmc        = ie->proto;
                r.certainty   = ie->certainty;
                r.gloss       = ie->gloss;
                r.proto_norse = soundEngine.Apply(ie->proto);
                r.runes       = runeMapper.ToRunes(r.proto_norse);
                r.runeTokens  = runeMapper.ToRuneTokens(r.proto_norse);
                r.source      = TokenSource::Idiom;
                results.push_back(std::move(r));
                i += len;
                matched = true;
                break;
            }
        }
        if (!matched) {
            results.push_back(Translate(tokens[i]));
            ++i;
        }
    }
    return results;
}

std::vector<std::string> Pipeline::Tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string tok;
    for (char c : line) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!tok.empty()) { tokens.push_back(tok); tok.clear(); }
        } else if (std::isalpha(static_cast<unsigned char>(c)) || c == '-') {
            tok += c;
        }
        // Punctuation is stripped; hyphens kept for compound words.
    }
    if (!tok.empty()) tokens.push_back(tok);
    return tokens;
}
