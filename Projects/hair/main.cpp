#include <string>
#include <memory>
#include "HairRendererApp.hpp"

using namespace Nebula;

int main(int argc, char *argv[])
{
    std::string hair {};
    if (argc > 1) {
        hair = std::string(argv[1]);
    }

    auto application = std::make_shared<HairRendererApp>(hair);
    application->run();
    return 0;
}