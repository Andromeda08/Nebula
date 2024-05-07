#include <memory>
#include "RaytracerApp.hpp"

using namespace Nebula;

int main(int argc, char *argv[])
{
    auto application = std::make_shared<RaytracerApp>();
    application->run();
    return 0;
}