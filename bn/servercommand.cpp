#include <bn/servercommand.hpp>

namespace bn {

ServerCommand::ServerCommand(int argc, char *argv[])
    : mArgc(argc)
    , mArgv(argv)
{}

int 
ServerCommand::run() {
    std::cout << "runserver" << std::endl;
    return 0;
}

};
