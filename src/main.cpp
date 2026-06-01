#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <linux/limits.h>

#include "Lexicon.h"
#include "IdiomLookup.h"
#include "SemanticDecomposer.h"
#include "SoundChangeEngine.h"
#include "RuneMapper.h"
#include "Pipeline.h"
#include "Server.h"

static std::string exeDir() {
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len < 0) return ".";
    buf[len] = '\0';
    std::string path(buf);
    auto slash = path.rfind('/');
    return slash == std::string::npos ? "." : path.substr(0, slash);
}

static std::string findData(const std::string& flagPath,
                             const std::string& envVar,
                             const std::string& relName) {
    if (!flagPath.empty()) return flagPath;
    const char* env = std::getenv(envVar.c_str());
    if (env && *env) return env;
    std::string next = exeDir() + "/" + relName;
    { std::ifstream f(next); if (f) return next; }
    std::string dev = exeDir() + "/../" + relName;
    { std::ifstream f(dev); if (f) return dev; }
    { std::ifstream f(relName); if (f) return relName; }
    return relName;
}

static void printUsage(const char* prog) {
    std::cerr
        << "Usage: " << prog << " [options] [words...]\n"
        << "  Reads from stdin when no words are given.\n\n"
        << "Options:\n"
        << "  -v, --verbose        Show full pipeline trace per word\n"
        << "  --no-decompose       Skip semantic decomposition\n"
        << "  --lexicon <path>     Override data/lexicon.json\n"
        << "  --compounds <path>   Override data/compounds.json\n"
        << "  --serve [port]       Launch the web GUI (default port 3175)\n"
        << "  -h, --help           Show this help\n";
}

static std::string sourceLabel(TokenSource s) {
    switch (s) {
        case TokenSource::Lexicon:  return "lexicon";
        case TokenSource::Compound: return "compound";
        case TokenSource::Idiom:    return "idiom";
        case TokenSource::Unknown:  return "unknown";
    }
    return "";
}

static void printVerbose(const PipelineResult& r) {
    // Unknown tokens: brief one-liner, no block.
    if (r.source == TokenSource::Unknown) {
        std::cout << r.english << "  [unknown]\n";
        return;
    }

    // ── Header line ──────────────────────────────────────────────
    // "ravens → raven" or just "wolf", then dashes to column 56.
    std::string label = r.english;
    if (!r.stem.empty())
        label += " \xe2\x86\x92 " + r.stem;   // UTF-8 →
    std::cout << label << ' ';
    int fill = 56 - static_cast<int>(label.size());
    for (int i = 0; i < fill; ++i) std::cout << "\xe2\x94\x80"; // UTF-8 ─
    std::cout << '\n';

    // ── Linguistic chain ─────────────────────────────────────────
    if (!r.gloss.empty())
        std::cout << "  Gloss        " << r.gloss << '\n';
    std::cout << "  PGmc         " << r.pgmc << '\n';
    std::cout << "  Proto-Norse  " << r.proto_norse << '\n';
    std::cout << "  Source       " << sourceLabel(r.source)
              << " \xc2\xb7 " << r.certainty << '\n'; // UTF-8 ·

    // ── Per-rune breakdown ───────────────────────────────────────
    if (!r.runeTokens.empty()) {
        std::string phonemes, names;
        for (size_t i = 0; i < r.runeTokens.size(); ++i) {
            if (i) { phonemes += "\xc2\xb7"; names += " \xc2\xb7 "; }
            phonemes += r.runeTokens[i].phoneme;
            names    += r.runeTokens[i].name;
        }
        std::cout << '\n';
        std::cout << "  " << r.runes    << '\n';
        std::cout << "  " << phonemes   << '\n';
        std::cout << "  " << names      << '\n';
    }

    std::cout << '\n';
}

int main(int argc, char* argv[]) {
    bool verbose      = false;
    bool decompose    = true;
    bool serveMode    = false;
    int  servePort    = 3175;
    std::string lexiconPath;
    std::string compoundsPath;
    std::string idiomsPath;
    std::vector<std::string> words;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "--no-decompose") {
            decompose = false;
        } else if (arg == "--lexicon" && i + 1 < argc) {
            lexiconPath = argv[++i];
        } else if (arg == "--compounds" && i + 1 < argc) {
            compoundsPath = argv[++i];
        } else if (arg == "--idioms" && i + 1 < argc) {
            idiomsPath = argv[++i];
        } else if (arg == "--serve") {
            serveMode = true;
            // Optional port number following --serve
            if (i + 1 < argc && std::isdigit(static_cast<unsigned char>(argv[i+1][0])))
                servePort = std::stoi(argv[++i]);
        } else if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (!arg.empty() && arg[0] != '-') {
            words.push_back(arg);
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            printUsage(argv[0]);
            return 1;
        }
    }

    std::string lxPath = findData(lexiconPath,   "RUNEPIPE_LEXICON",   "data/lexicon.json");
    std::string cpPath = findData(compoundsPath, "RUNEPIPE_COMPOUNDS", "data/compounds.json");
    std::string idPath = findData(idiomsPath,    "RUNEPIPE_IDIOMS",    "data/idioms.json");

    Lexicon lexicon;
    if (!lexicon.Load(lxPath)) {
        std::cerr << "Error: could not load lexicon from '" << lxPath << "'\n"
                  << "Set RUNEPIPE_LEXICON or use --lexicon <path>.\n";
        return 1;
    }

    SemanticDecomposer decomposer;
    decomposer.Load(cpPath);

    IdiomLookup idioms;
    idioms.Load(idPath);  // failure is silent — idioms are optional

    SoundChangeEngine sce;
    RuneMapper mapper;
    Pipeline pipeline(lexicon, decomposer, sce, mapper, idioms, decompose);

    if (serveMode) {
        std::string uiPath = findData("", "RUNEPIPE_UI", "ui/index.html");
        // Auto-open Firefox in background; ignore failure.
        std::string url = "http://localhost:" + std::to_string(servePort);
        std::string openCmd = "firefox " + url + " &";
        std::system(openCmd.c_str());  // NOLINT — intentional browser launch
        Server server(pipeline, mapper, uiPath, servePort);
        server.Start();
        return 0;
    }

    auto processLine = [&](const std::string& line) {
        auto results = pipeline.TranslateLine(line);
        if (verbose) {
            for (const auto& r : results)
                printVerbose(r);
        } else {
            bool first = true;
            for (const auto& r : results) {
                if (r.source == TokenSource::Unknown) continue;
                if (!first) std::cout << ' ';
                std::cout << r.runes;
                first = false;
            }
            if (!results.empty()) std::cout << '\n';
        }
    };

    if (!words.empty()) {
        std::string line;
        for (size_t i = 0; i < words.size(); ++i) {
            if (i) line += ' ';
            line += words[i];
        }
        processLine(line);
    } else {
        std::string line;
        while (std::getline(std::cin, line))
            processLine(line);
    }

    return 0;
}
