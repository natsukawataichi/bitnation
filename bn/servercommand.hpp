#pragma once

#include <bn/header.hpp>
#include <bn/command.hpp>

namespace bn {

class ServerCommand : public Command {
protected:
    int mArgc;
    char **mArgv;

public:
    ServerCommand(int argc, char *argv[]);
    virtual int run();
};

};
