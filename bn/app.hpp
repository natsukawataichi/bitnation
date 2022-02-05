#pragma once

#include <bn/header.hpp>
#include <bn/type.hpp>
#include <bn/error.hpp>
#include <bn/command.hpp>
#include <bn/servercommand.hpp>

namespace bn {

struct AppOption {
    bool isHelp;
};

class App {
protected:
    AppOption mOpt;
    int mArgc;
    char **mArgv;
    std::string mCmdName;

public:
    ~App();
    App(int argc, char *argv[]);
    int run();

protected:
    void showUsage();
    int _run();
    int runCmd(const std::string &cmdName);
    CommandPtr makeCmd(const std::string &cmdName);
};

};  // nc
