#pragma once

#include <bn/header.hpp>

namespace bn {

struct AppOption {
    bool isHelp;
};

class App {
protected:
    AppOption mOpt;
    int mArgc;
    char **mArgv;
    int mOptind;

public:
    ~App();
    App(int argc, char *argv[]);
    bool run();

protected:
    void showUsage();
    bool parseOpts();
    bool _run();
};

};  // nc
