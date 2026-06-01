# RunePipeline

**Translate modern English into Proto-Germanic, Proto-Norse, and ultimately Elder Futhark runes.**

RunePipeline is a C++17 command-line tool and local web application that walks each word of an English phrase through a four-stage linguistic pipeline:

```
English  →  Proto-Germanic  →  Proto-Norse  →  Elder Futhark runes
 "wolf"       *wulfaz           wulfaR          ᚹᚢᛚᚠᚨᛉ
```

Every stage is shown: the reconstructed Proto-Germanic root, the Proto-Norse form after sound changes, and the Elder Futhark glyph sequence with per-rune names and phonemes. For words with no direct historical equivalent (modern concepts like "computer" or "internet"), a semantic decomposition engine builds a plausible PGmc compound from attested roots.

---

## How this project came about

The Elder Futhark runic alphabet was used across the Germanic world from roughly the 2nd to the 8th century CE, carved into weapons, jewellery, runestones, and burial goods. It encodes a stage of the language that preceded Old English, Old Norse, and the other Germanic branches: a world of shared myth, shared vocabulary, and shared sounds that later splintered into dozens of distinct tongues.

RunePipeline grew from a simple curiosity: *what would ordinary modern words have looked like to someone who spoke that ancestral language?* Rather than a static lookup table, the project attempts to reconstruct the actual linguistic path: starting from attested Proto-Germanic roots sourced from academic etymological dictionaries, applying the phonological changes that produced Proto-Norse, and then mapping the resulting sound sequence onto the Elder Futhark characters that speakers of that era would have used.

The result is useful for creative writers seeking period-appropriate names and inscriptions, game developers building authentic-feeling Norse or Germanic settings, students learning historical linguistics, and anyone who finds it compelling to hear the deep past echoed in familiar words.

---

## A note on historical accuracy

**Proto-Norse is old, and the historical record is thin. Please treat all output as a scholarly approximation, not ground truth.**

There are several reasons to hold the output with appropriate humility:

### Proto-Germanic was never written down

Every Proto-Germanic form marked with an asterisk (*wulfaz, *habjaną, etc.) is a *reconstruction*, inferred by working backwards from Old English, Old Norse, Gothic, Old High German, and other daughter languages using the systematic sound laws of comparative linguistics. No scribe ever wrote these words. Scholars sometimes disagree on the exact reconstruction, and RunePipeline's lexicon follows primarily Kroonen (2013) and Orel (2003), which themselves differ from earlier sources in places.

### The Elder Futhark corpus is tiny and fragmentary

Roughly 350 Elder Futhark inscriptions survive. Most are very short: a name, a word, a short formula. Many are damaged or ambiguous. The inscriptions span 600 years and a vast geographic range, so the "language" they represent was not uniform. RunePipeline models a generalised Proto-Norse, not any specific regional or temporal dialect.

### Sound changes had fuzzy edges

The phonological rules that turned Proto-Germanic into Proto-Norse (rhotacism: *z → R; vowel breaking; umlaut conditioning) were regular in the majority of cases but had conditioned exceptions, dialectal variation, and borrowed words that did not follow the expected pattern. RunePipeline applies a simplified rule set that captures the most common changes. Edge cases will not always be right.

### Lexical coverage is uneven

Concrete, everyday nouns (wolf, axe, sea, fire) are generally well-attested across multiple daughter languages and carry high certainty. Abstract concepts, specialist vocabulary, and culturally specific terms are thinner. Some entries carry `"certainty": "medium"` because the PGmc root is uncertain or the word entered Germanic as an early Latin or Celtic loan; these are indicated in the API output and the web GUI's pipeline trace. `"certainty": "low"` entries are genuine best-guesses.

### Modern words have no historical equivalent

"Computer," "satellite," and "democracy" did not exist. The semantic decomposition engine handles these by splitting them into their closest PGmc conceptual components (a computer becomes *taljō + tōlą*, "reckoning-tool"), which is creatively motivated but linguistically invented. The GUI labels these clearly as `[compound]` rather than `[lexicon]`.

### The rune-to-sound mapping is approximate

Elder Futhark runes encoded sounds, not spellings, and the exact phonetic value of several runes is debated. Vowel length was often not distinguished in writing. RunePipeline maps each character of the Proto-Norse form to the nearest rune, which is how ancient scribes would have approached the problem, though ancient scribes also varied considerably in their choices.

**In short:** use RunePipeline for inspiration, learning, and creative work. For academic citations, consult primary sources.

---

## Features

- **1,329-entry lexicon** sourced from Kroonen (2013) and Orel (2003), covering common vocabulary, numbers, adjectives, function words, kinship terms, fauna, flora, mythology, and more
- **Inflection stripping**: *wolves*, *running*, *brightest* resolve to their stem forms
- **Semantic compound decomposition**: modern or archaic words not in the lexicon are broken into PGmc root components
- **Idiom lookup**: 90 common English idioms mapped to their Proto-Germanic semantic equivalents
- **Four-stage pipeline trace**: see the PGmc root, certainty rating, source type, and per-rune breakdown for every token
- **Web GUI**: Norse-themed single-page app served locally, with no internet connection required after build
- **Elder Futhark reference panel**: all 24 runes with names, phonemes, and traditional meanings
- **REST API**: JSON endpoints for integration with other tools
- **Zero runtime dependencies**: one self-contained binary plus two JSON data files

---

## Prerequisites

| Requirement | Minimum version | Notes |
|---|---|---|
| C++ compiler | GCC 9+ or Clang 9+ | Must support C++17 |
| CMake | 3.15+ | Handles all dependency fetching |
| Git | Any recent version | Required for CMake FetchContent |
| Internet access (build only) | — | CMake downloads nlohmann/json and cpp-httplib on first build |

No other libraries need to be installed manually. CMake will automatically download:
- **nlohmann/json v3.11.3**: JSON parsing (uses system install if available)
- **cpp-httplib v0.18.1**: embedded HTTP server (header-only, HTTP-only, no OpenSSL)

### Arch Linux / Archcraft

```bash
sudo pacman -S base-devel cmake git
# Optional: pre-install nlohmann-json to skip the download step
sudo pacman -S nlohmann-json
```

### Debian / Ubuntu

```bash
sudo apt install build-essential cmake git
# Optional
sudo apt install nlohmann-json3-dev
```

### Fedora / RHEL

```bash
sudo dnf install gcc-c++ cmake git
```

### macOS (Homebrew)

```bash
brew install cmake git
# Optional
brew install nlohmann-json
```

---

## Installation

### Clone and build

```bash
git clone <repository-url> RunePipeline
cd RunePipeline
cmake -B build
cmake --build build -j$(nproc)
```

The binary is placed at `build/runepipe`. The `data/` and `ui/` directories are automatically copied next to it by the build system.

### Install to a local prefix

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=~/.local
cmake --build build -j$(nproc)
cmake --install build
```

This places `runepipe` in `~/.local/bin` and the data files in `~/.local/share/runepipe/`. Add `~/.local/bin` to your `PATH` if it is not already there.

### System-wide install

```bash
cmake -B build
cmake --build build -j$(nproc)
sudo cmake --install build
```

### Quick single-file build (requires system nlohmann/json)

```bash
g++ src/*.cpp -std=c++17 -O2 -Isrc -o runepipe
./runepipe --help
```

---

## Usage

### Command-line

```bash
# Translate a phrase
./build/runepipe wolf king

# Pipe from stdin
echo "shield maiden of the north" | ./build/runepipe

# Read a list of words
cat words.txt | ./build/runepipe

# Verbose pipeline trace — shows PGmc root, certainty, source, per-rune breakdown
./build/runepipe --verbose wolf king

# Disable semantic decomposition (unknown words stay unknown)
./build/runepipe --no-decompose computer
```

#### Example output

```
$ ./build/runepipe wolf king
ᚹᚢᛚᚠᚨᛉ ᚲᚢᚾᛁᚾᚷᚨᛉ

$ ./build/runepipe --verbose wolf
wolf ────────────────────────────────────────────────────────
  PGmc         wulfaz
  Proto-Norse  wulfaR
  Source       lexicon · high

  ᚹᚢᛚᚠᚨᛉ
  w·u·l·f·a·R
  Wunjo · Uruz · Laguz · Fehu · Ansuz · Algiz

$ ./build/runepipe --verbose computer
computer ───────────────────────────────────────────────────
  Gloss        reckoning-tool
  PGmc         taljōtōlą
  Proto-Norse  taliōtōla
  Source       compound · medium
```

### Web GUI

```bash
./build/runepipe --serve
# Opens http://localhost:3175 in your default browser automatically.
# Uses xdg-open on Linux, open on macOS, start on Windows.

# Custom port
./build/runepipe --serve 8080

# Skip auto-open (navigate to the URL yourself)
./build/runepipe --serve --no-open
```

The GUI provides:
- **Live translation** as you type (250 ms debounce)
- **Large rune output** with a one-click copy button
- **Pipeline trace cards**: expandable per-word panels showing PGmc root, Proto-Norse form, certainty badge, source type, and an interactive per-rune breakdown with hover tooltips (rune name, phoneme, source character)
- **Elder Futhark reference panel**: all 24 runes, toggleable
- Press `Ctrl-C` in the terminal to stop the server

### Override data files

```bash
./build/runepipe --lexicon /path/to/custom-lexicon.json "battle cry"
./build/runepipe --compounds /path/to/custom-compounds.json "spaceship"
./build/runepipe --idioms /path/to/custom-idioms.json "break a leg"
```

### Environment variables

| Variable | Purpose |
|---|---|
| `RUNEPIPE_LEXICON` | Override default `data/lexicon.json` |
| `RUNEPIPE_COMPOUNDS` | Override default `data/compounds.json` |
| `RUNEPIPE_IDIOMS` | Override default `data/idioms.json` |
| `RUNEPIPE_UI` | Override default `ui/index.html` |

---

## Command-line reference

```
Usage: runepipe [options] [words...]
  Reads from stdin when no words are given.

Options:
  -v, --verbose        Show full pipeline trace per word
  --no-decompose       Skip semantic decomposition
  --lexicon <path>     Override data/lexicon.json
  --compounds <path>   Override data/compounds.json
  --idioms <path>      Override data/idioms.json
  --serve [port]       Launch the web GUI (default port 3175)
  --no-open            Don't auto-open a browser with --serve
  -h, --help           Show this help
```

---

## REST API

When running with `--serve`, three JSON endpoints are available.

### `POST /api/translate`

Translate a phrase. Request body: `{"text": "wolf king"}`.

```json
{
  "tokens": [
    {
      "english":     "wolf",
      "pgmc":        "wulfaz",
      "proto_norse": "wulfaR",
      "runes":       "ᚹᚢᛚᚠᚨᛉ",
      "certainty":   "high",
      "source":      "lexicon",
      "gloss":       "",
      "rune_tokens": [
        { "rune": "ᚹ", "name": "Wunjo",  "phoneme": "w", "sourceChar": "w" },
        { "rune": "ᚢ", "name": "Uruz",   "phoneme": "u", "sourceChar": "u" },
        { "rune": "ᛚ", "name": "Laguz",  "phoneme": "l", "sourceChar": "l" },
        { "rune": "ᚠ", "name": "Fehu",   "phoneme": "f", "sourceChar": "f" },
        { "rune": "ᚨ", "name": "Ansuz",  "phoneme": "a", "sourceChar": "a" },
        { "rune": "ᛉ", "name": "Algiz",  "phoneme": "R", "sourceChar": "R" }
      ]
    }
  ]
}
```

**Token fields:**

| Field | Description |
|---|---|
| `english` | The original input word |
| `pgmc` | Reconstructed Proto-Germanic root (asterisk form without leading *) |
| `proto_norse` | PGmc with Proto-Norse sound changes applied |
| `runes` | Elder Futhark glyph string |
| `certainty` | `high`, `medium`, or `low`; indicates confidence in the PGmc reconstruction |
| `source` | `lexicon`, `compound`, `idiom`, or `unknown` |
| `gloss` | Human-readable decomposition (populated for compound and idiom tokens) |
| `rune_tokens` | Per-rune breakdown array for rendering interactive tooltips |

### `GET /api/runes`

Returns all 24 Elder Futhark runes with names, phonemes, and traditional meanings.

```json
[
  { "rune": "ᚠ", "name": "Fehu",    "phoneme": "f", "meaning": "cattle, wealth" },
  { "rune": "ᚢ", "name": "Uruz",    "phoneme": "u", "meaning": "aurochs, strength" },
  ...
]
```

### `GET /api/version`

```json
{ "version": "1.4.0" }
```

---

## Data files

All data files are plain JSON and can be edited without recompiling.

### `data/lexicon.json`

The primary vocabulary store. Each entry maps an English headword to a Proto-Germanic form and certainty rating:

```json
{
  "wolf":  { "proto": "wulfaz",    "certainty": "high" },
  "raven": { "proto": "hrabnaz",   "certainty": "high" },
  "amber": { "proto": "glaesą",    "certainty": "medium" },
  "plow":  { "proto": "plōgaz",    "certainty": "medium" }
}
```

The lexicon currently contains **1,329 entries** spanning everyday verbs, numbers, adjectives, function words, body parts, kinship, fauna, flora, tools, weapons, mythology, weather, geography, and abstract concepts.

After editing `lexicon.json`, copy it to `build/data/lexicon.json` and restart the server. No recompilation is needed:

```bash
cp data/lexicon.json build/data/lexicon.json
pkill -f "runepipe --serve"
./build/runepipe --serve &
```

### `data/compounds.json`

Maps modern or anachronistic English words to PGmc root components that are concatenated into a compound form. Used as a fallback when a word is not in the lexicon:

```json
{
  "computer":  { "components": ["taljō", "tōlą"], "gloss": "reckoning-tool" },
  "internet":  { "components": ["allą", "netją"], "gloss": "all-net" },
  "democracy": { "components": ["folkaz", "waldaną"], "gloss": "folk-rule" }
}
```

### `data/idioms.json`

Maps common English idioms to a Proto-Germanic semantic equivalent, expressing the meaning in PGmc rather than producing a word-for-word rendering:

```json
{
  "under the weather": { "proto": "sauhtiz", "certainty": "high", "gloss": "feeling ill" },
  "time flies":        { "proto": "hwīlō snagwaz", "certainty": "medium", "gloss": "time passes swiftly" }
}
```

---

## Pipeline architecture

```
Input phrase
    │
    ▼
Tokenizer          — splits on whitespace and punctuation
    │
    ▼
Inflection stripper — removes -s, -es, -ed, -ing, -er, -est endings
    │
    ├─── IdiomLookup  — checks whole-phrase idiom table first
    │
    ├─── Lexicon      — direct headword lookup (case-insensitive)
    │
    └─── SemanticDecomposer — compound fallback from components table
              │
              ▼
         SoundChangeEngine — applies PGmc → Proto-Norse phonological rules:
             1. Rhotacism: *z → *R
             2. eu-umlaut: eu → iu before a high-vowel syllable
             3. Final -ją → -ja (ja-stem nasal loss)
             4. Final -ōją → -ōja (ō-ja stem nasal loss)
              │
              ▼
         RuneMapper — maps each Proto-Norse character to an Elder Futhark glyph
              │
              ▼
         PipelineResult { english, pgmc, proto_norse, runes, certainty,
                          source, gloss, stem, runeTokens }
```

### Source types

| Source | Meaning |
|---|---|
| `lexicon` | Direct match in `lexicon.json`; highest confidence |
| `compound` | Built from PGmc root components in `compounds.json`; labelled with a gloss |
| `idiom` | Whole-phrase idiom match from `idioms.json` |
| `unknown` | No mapping found; word is passed through untranslated |

---

## Project structure

```
RunePipeline/
├── CMakeLists.txt
├── BUILD.md
├── README.md
├── data/
│   ├── lexicon.json        Primary vocabulary (1,329 entries)
│   ├── compounds.json      Semantic compound fallback (125 entries)
│   └── idioms.json         English idiom mappings (90 entries)
├── ui/
│   └── index.html          Self-contained web GUI (no build step)
└── src/
    ├── main.cpp            Entry point, CLI argument parsing
    ├── Pipeline.h/cpp      Orchestrates the four-stage pipeline
    ├── Lexicon.h/cpp       JSON lexicon loader and lookup
    ├── SoundChangeEngine.h/cpp  PGmc → Proto-Norse phonological rules
    ├── RuneMapper.h/cpp    Proto-Norse → Elder Futhark glyph mapping
    ├── SemanticDecomposer.h/cpp Compound fallback from components
    ├── IdiomLookup.h/cpp   Whole-phrase idiom table
    ├── Server.h/cpp        Embedded HTTP server (cpp-httplib)
    └── version.h           Version string
```

---

## Extending the lexicon

The lexicon is the easiest part of the project to extend. Adding an entry requires no C++ knowledge:

1. Open `data/lexicon.json` in any text editor.
2. Add your entry following the existing format:
   ```json
   "mead": { "proto": "meduz", "certainty": "high" }
   ```
3. Copy to the build directory and restart:
   ```bash
   cp data/lexicon.json build/data/lexicon.json
   ```

**Choosing a certainty level:**

| Level | Use when |
|---|---|
| `high` | The PGmc root is attested in at least two Germanic branches (OE, ON, Gothic, OHG, etc.) with a regular correspondence |
| `medium` | The root is reconstructible but uncertain: only one branch attests it, or it is an early loan from Latin/Celtic/other |
| `low` | The best available guess; the form is speculative or the word may not have a plausible PGmc equivalent |

**Primary sources for PGmc reconstruction (in priority order):**

1. Kroonen, G. (2013). *Etymological Dictionary of Proto-Germanic*. Brill.
2. Orel, V. (2003). *A Handbook of Germanic Etymology*. Brill.
3. OE/ON/Gothic cognate chains via OED, Bosworth-Toller, and Zoëga's *Old Icelandic Dictionary*
4. Ringe, D. (2006). *From Proto-Indo-European to Proto-Germanic*. Oxford.

---

## Academic references

- Kroonen, G. (2013). *Etymological Dictionary of Proto-Germanic*. Leiden: Brill.
- Orel, V. (2003). *A Handbook of Germanic Etymology*. Leiden: Brill.
- Ringe, D. (2006). *From Proto-Indo-European to Proto-Germanic*. Oxford: Oxford University Press.
- Fortson, B. W. (2010). *Indo-European Language and Culture: An Introduction* (2nd ed.). Wiley-Blackwell.
- Page, R. I. (2005). *Runes*. British Museum Press.
- Antonsen, E. H. (2002). *Runes and Germanic Linguistics*. Mouton de Gruyter.

---

## License

License to be determined. All rights reserved pending a decision.

---

## Version

Current version: **1.4.0**
