#include "Server.h"
#include "version.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <httplib.h>

using json = nlohmann::json;

Server::Server(Pipeline& pipeline, const RuneMapper& mapper,
               const std::string& uiPath, int port)
    : pipeline_(pipeline), mapper_(mapper), uiPath_(uiPath), port_(port) {}

std::string Server::serveUI() const {
    std::ifstream f(uiPath_);
    if (!f) return "<h1>ui/index.html not found</h1>";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static std::string sourceStr(TokenSource s) {
    switch (s) {
        case TokenSource::Lexicon:  return "lexicon";
        case TokenSource::Compound: return "compound";
        case TokenSource::Idiom:    return "idiom";
        case TokenSource::Unknown:  return "unknown";
    }
    return "unknown";
}

std::string Server::translateJSON(const std::string& body) const {
    json req, resp;
    try { req = json::parse(body); }
    catch (...) {
        return json{{"error","invalid JSON"}}.dump();
    }

    std::string text = req.value("text", std::string{});
    auto results = pipeline_.TranslateLine(text);

    json tokens = json::array();
    for (const auto& r : results) {
        json tok;
        tok["english"]     = r.english;
        tok["pgmc"]        = r.pgmc;
        tok["proto_norse"] = r.proto_norse;
        tok["runes"]       = r.runes;
        tok["certainty"]   = r.certainty;
        tok["source"]      = sourceStr(r.source);
        tok["gloss"]       = r.gloss;

        json rtArr = json::array();
        for (const auto& rt : r.runeTokens) {
            rtArr.push_back({
                {"rune",       rt.rune},
                {"name",       rt.name},
                {"phoneme",    rt.phoneme},
                {"sourceChar", rt.sourceChar},
            });
        }
        tok["rune_tokens"] = rtArr;
        tokens.push_back(tok);
    }
    resp["tokens"] = tokens;
    return resp.dump();
}

std::string Server::runesJSON() const {
    json arr = json::array();
    for (const auto& [glyph, info] : RuneMapper::AllRunes()) {
        arr.push_back({
            {"rune",    glyph},
            {"name",    info.name},
            {"phoneme", info.phoneme},
            {"meaning", info.meaning},
        });
    }
    return arr.dump();
}

std::string Server::versionJSON() const {
    return json{{"version", RUNEPIPE_VERSION}}.dump();
}

void Server::Start() {
    httplib::Server svr;

    svr.Get("/", [this](const httplib::Request&, httplib::Response& res) {
        res.set_content(serveUI(), "text/html; charset=utf-8");
    });

    svr.Post("/api/translate", [this](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(translateJSON(req.body), "application/json");
    });

    svr.Get("/api/runes", [this](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(runesJSON(), "application/json");
    });

    svr.Get("/api/version", [this](const httplib::Request&, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(versionJSON(), "application/json");
    });

    std::cout << "RunePipeline UI → http://localhost:" << port_ << "\n"
              << "Press Ctrl-C to quit.\n";

    svr.listen("localhost", port_);
}
