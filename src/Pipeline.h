#pragma once
#include <string>
#include <vector>
#include "Lexicon.h"
#include "IdiomLookup.h"
#include "SemanticDecomposer.h"
#include "SoundChangeEngine.h"
#include "RuneMapper.h"

enum class TokenSource {
    Lexicon,   // direct match in lexicon.json
    Compound,  // semantic decomposition from compounds.json
    Idiom,     // whole-phrase idiom match from idioms.json
    Unknown,   // no mapping found
};

struct PipelineResult {
    std::string english;
    std::string pgmc;
    std::string proto_norse;
    std::string runes;
    std::string certainty;
    std::string gloss;             // populated for Compound/Idiom sources
    std::string stem;              // non-empty when found via inflection stripping (e.g. "raven" for "ravens")
    TokenSource source;
    std::vector<RuneToken> runeTokens; // per-rune breakdown for the GUI
};

class Pipeline {
public:
    Pipeline(const Lexicon& lex, const SemanticDecomposer& decomp,
             const SoundChangeEngine& sce, const RuneMapper& mapper,
             const IdiomLookup& idioms,
             bool allowDecompose = true);

    PipelineResult Translate(const std::string& word) const;
    std::vector<PipelineResult> TranslateLine(const std::string& line) const;

private:
    const Lexicon&            lexicon;
    const SemanticDecomposer& decomposer;
    const SoundChangeEngine&  soundEngine;
    const RuneMapper&         runeMapper;
    const IdiomLookup&        idiomLookup;
    bool                      allowDecompose;

    static std::vector<std::string> Tokenize(const std::string& line);
};
