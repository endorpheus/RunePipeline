#pragma once
#include <string>
#include "Pipeline.h"
#include "RuneMapper.h"

class Server {
public:
    Server(Pipeline& pipeline, const RuneMapper& mapper,
           const std::string& uiPath, int port);
    // Blocks until the server is stopped (Ctrl-C / SIGINT).
    void Start();

private:
    Pipeline&         pipeline_;
    const RuneMapper& mapper_;
    std::string       uiPath_;
    int               port_;

    std::string serveUI() const;
    std::string translateJSON(const std::string& body) const;
    std::string runesJSON() const;
    std::string versionJSON() const;
};
