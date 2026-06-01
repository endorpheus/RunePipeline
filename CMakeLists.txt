cmake_minimum_required(VERSION 3.15)
project(RunePipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- nlohmann/json ---
find_package(nlohmann_json 3.2.0 QUIET)
if(NOT nlohmann_json_FOUND)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# --- cpp-httplib (HTTP-only, no OpenSSL needed) ---
FetchContent_Declare(
    cpp_httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.18.1
    GIT_SHALLOW    TRUE
)
# We only need the header; disable SSL and Zlib to keep it lightweight.
set(HTTPLIB_COMPILE    OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_OPENSSL_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_ZLIB_IF_AVAILABLE    OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cpp_httplib)

# --- Binary ---
add_executable(runepipe
    src/main.cpp
    src/Lexicon.cpp
    src/RuneMapper.cpp
    src/SoundChangeEngine.cpp
    src/SemanticDecomposer.cpp
    src/IdiomLookup.cpp
    src/Pipeline.cpp
    src/Server.cpp
)

target_include_directories(runepipe PRIVATE src)
target_link_libraries(runepipe PRIVATE
    nlohmann_json::nlohmann_json
    httplib::httplib
)
target_compile_options(runepipe PRIVATE -Wall -Wextra -Wpedantic)

# --- Copy data/ and ui/ next to the binary ---
add_custom_command(TARGET runepipe POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:runepipe>/data
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/ui
        $<TARGET_FILE_DIR:runepipe>/ui
    COMMENT "Copying data/ and ui/ to build directory"
)

# --- Install ---
include(GNUInstallDirs)
install(TARGETS runepipe RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY data DESTINATION ${CMAKE_INSTALL_DATADIR}/runepipe)
install(DIRECTORY ui   DESTINATION ${CMAKE_INSTALL_DATADIR}/runepipe)
