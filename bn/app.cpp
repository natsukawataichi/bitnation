#include <bn/app.hpp>

namespace bn {

App::~App() {
}

App::App(int argc, char *argv[])
    : mOpt()
    , mArgc(argc)
    , mArgv(argv)
{}

void 
App::showUsage() {
    std::cout << "Bit Nation CLI.\n";
    std::cout << "\n";
    std::cout << "Usage:\n";
    std::cout << "\n";
    std::cout << "    bitnation [command] [arguments] [options]\n";
    std::cout << "\n";
    std::cout << "The options are:\n";
    std::cout << "\n";
    std::cout << "    -h, --help    Show usage.\n";
    std::cout << std::endl;
}

int
App::run() {
    return _run();
}

int 
App::_run() {
    bool needUsage = mOpt.isHelp || mArgc < 2;
    if (needUsage) {
        showUsage();
        return 0;
    }

    const char *cmdName = mArgv[1];
    return runCmd(cmdName);
}

int
App::runCmd(const std::string &cmdName) {
    mCmdName = cmdName;

    auto cmd = makeCmd(mCmdName);
    if (!cmd) {
        error("invalid command name \"%s\"", mCmdName.c_str());
        return 1;
    }

    return cmd->run();
}

CommandPtr 
App::makeCmd(const std::string &cmdName) {
    int cmdArgc = mArgc - 1;
    char **cmdArgv = mArgv + 1;

    if (cmdName == "server") {
        return std::make_shared<ServerCommand>(cmdArgc, cmdArgv);
    }

    return nullptr;
}

};  // bn

