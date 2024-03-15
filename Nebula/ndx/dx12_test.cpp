#include "dx12_Window.hpp"
#include <ndx/Context.hpp>

using namespace Nebula;
int main()
{
    auto wnd = std::make_shared<DX12_Test_Window>();

    ndx::ContextCreateInfo context_create_info = {
        .frame_count = 2,
        .window = wnd,
    };
    auto ctx = std::make_shared<ndx::Context>(context_create_info);

    while (!wnd->will_close())
    {
        ctx->on_render();
    }
    ctx->wait_idle();

    return 0;
}
