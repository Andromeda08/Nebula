#include <napp/Application.hpp>

using namespace Nebula;

int main()
{
    auto application = std::make_shared<Application>();
    application->run();
    return 0;
}