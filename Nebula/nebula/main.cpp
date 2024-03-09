#include <optional>
#include <napp/Application.hpp>

using namespace Nebula;

int main(int argc, char *argv[])
{
    std::optional<std::string> hair = std::nullopt;
    if (argc > 1) {
        hair = std::make_optional<std::string>(std::string(argv[1]));
    }

    auto application = std::make_shared<Application>(hair);
    application->run();
    return 0;
}