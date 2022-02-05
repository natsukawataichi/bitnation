#include <bn/main.hpp>

int main(int argc, char *argv[]) {
    bn::App app(argc, argv);
    
    if (!app.run()) {
        return 1;
    }

    return 0;
}
