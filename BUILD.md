# Build

## Requirements

- C++17 compiler (GCC 9+ / Clang 9+)
- CMake 3.15+
- nlohmann/json (auto-fetched by CMake if not installed)

To install nlohmann/json manually (optional, speeds up first build):
```
pacman -S nlohmann-json          # Arch / Archcraft
apt install nlohmann-json3-dev   # Debian/Ubuntu
brew install nlohmann-json       # macOS
```

## CMake build (recommended)

```bash
cmake -B build
cmake --build build -j$(nproc)
./build/runepipe --help
```

## Quick one-liner (requires system nlohmann/json)

```bash
g++ src/*.cpp -std=c++17 -O2 -Isrc -o runepipe
./runepipe --help
```

## Usage

```bash
# Single phrase
echo "king of the sea" | ./build/runepipe

# Verbose pipeline trace
echo "wolf king" | ./build/runepipe -v

# Anachronistic words (semantic compounding fallback)
echo "computer warrior" | ./build/runepipe -v

# Pipe a word list
cat words.txt | ./build/runepipe

# Pass words directly
./build/runepipe shield maiden glory

# Override data files
./build/runepipe --lexicon /path/to/my-lexicon.json "battle cry"
```

## Environment variables

| Variable           | Purpose                              |
|--------------------|--------------------------------------|
| `RUNEPIPE_LEXICON`   | Override default data/lexicon.json   |
| `RUNEPIPE_COMPOUNDS` | Override default data/compounds.json |

## Install to ~/.local/bin

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=~/.local
cmake --build build
cmake --install build
```
