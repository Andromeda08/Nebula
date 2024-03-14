#include "Context.hpp"

#include <format>
#include <iostream>
#include <stdexcept>
#include "Utility.hpp"


namespace Nebula::ndx
{

    Context::Context()
    {
        std::cout << std::format("{} Initializing {} context", p_info, green("DirectX 12")) << std::endl;
        
        if (auto result = CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory));
            FAILED(result))
        {
            throw std::runtime_error(std::format("{} Failed to create {}", p_error, green("DXGIFactory")));
        }

        m_device = std::make_shared<Device>(m_factory.Get());
    }
}