#include <bn/app.hpp>

namespace bn {

App::~App() {
}

App::App(int argc, char *argv[])
    : mArgc(argc)
    , mArgv(argv)
{}

bool
App::run() {
    if (!parseOpts()) {
        return false;
    }

    return _run();
}

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

bool 
App::_run() {
    if (mOpt.isHelp) {
        showUsage();
        return true;
    }

    return true;
}

bool 
App::parseOpts() {
    // parse options
    static struct option longopts[] = {
        {"help", no_argument, 0, 'h'},
        {"fname", required_argument, 0, 'f'},
        {0},
    };

    opterr = 0; // ignore error messages
    optind = 0; // init index of parse

    for (;;) {
        int optsindex;
        int cur = getopt_long(
            mArgc, mArgv, "hf:", longopts, &optsindex
        );
        if (cur == -1) {
            break;
        }

        switch (cur) {
        case 0: /* long option only */ break;
        case 'h': mOpt.isHelp = true; break;
        case 'f': printf("%s\n", optarg); break;
        case '?':
        default: perror("Unknown option"); break;
        }
    }

    mOptind = optind;
    
    if (mArgc < optind) {
        perror("Failed to parse option");
        return false;
    }

    return true;
}

};  // bn

