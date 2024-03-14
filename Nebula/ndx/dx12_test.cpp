#include "dx12_Window.hpp"
#include <ndx/Context.hpp>
#include <ndx/Swapchain.hpp>

using namespace Nebula;
int main()
{
    auto wnd = std::make_shared<DX12_Test_Window>();
    auto ctx = std::make_shared<ndx::Context>();
    auto swc = std::make_shared<ndx::Swapchain>(wnd, ctx);

    while (!wnd->will_close())
    {

    }

    return 0;
}