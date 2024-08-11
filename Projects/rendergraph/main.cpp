#include <memory>
#include "RenderGraphApp.hpp"

using namespace Nebula;

int main(int argc, char *argv[])
{
    auto application = std::make_shared<RenderGraphApp>();
    application->run();
    return 0;
}