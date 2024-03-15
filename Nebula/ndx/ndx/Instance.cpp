#include "Instance.hpp"
#include "Utility.hpp"

namespace Nebula::ndx
{
    Instance::Instance(bool debug)
    {
        uint32_t factory_flags = 0;

        if (debug)
        {
            if (auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug));
                FAILED(result))
            {
                throw make_exception("Failed to get {}", p_error, red("ID3D12Debug"));
            }

            m_debug->EnableDebugLayer();
            factory_flags = DXGI_CREATE_FACTORY_DEBUG;
        }

        if (auto result = CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&m_factory));
            FAILED(result))
        {
            throw make_exception("Failed to create {}", p_error, red("DXGIFactory"));
        }

        std::string extra;
        if (debug)
        {
            extra = std::format(" with debugging features");
        }
        pInfo("Created {}{}", green("DXGIFactory"), extra);
    }
}