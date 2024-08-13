#include "RenderPath.hpp"

#include <vulkan/vulkan.hpp>
#include "nrg/common/Node.hpp"
#include "nrg/resource/Resources.hpp"
#include "nvk/Barrier.hpp"

#ifdef NBL_DEBUG
#include <fmt/printf.h>
#include "ncommon/Measure.hpp"
#endif

namespace Nebula::nrg
{
    void push_debug_label(const std::array<float, 4>& color, const std::string& name, const vk::CommandBuffer& command_buffer)
    {
        auto label = vk::DebugUtilsLabelEXT().setColor(color).setPLabelName(name.c_str());
        command_buffer.beginDebugUtilsLabelEXT(label);
    }

    void pop_debug_label(const vk::CommandBuffer& command_buffer)
    {
        command_buffer.endDebugUtilsLabelEXT();
    }

    void RenderPath::execute(const vk::CommandBuffer& command_buffer)
    {
        if (!m_initialized)
        {
            #ifdef NBL_DEBUG
                auto time = measure([&](){initialize(command_buffer);});
                fmt::println("RenderPath initialized in {} ms", time.count());
            #else
                initialize(command_buffer);
            #endif
        }

        for (const auto& node : m_nodes)
        {
            if (node->type() == NodeType::eSceneDataProvider)
            {
                continue;
            }

            #ifdef NBL_DEBUG
            push_debug_label(node->marker_color(), node->name(), command_buffer);
            #endif

            node->update();

            #pragma region "evaluate barriers"
            std::vector<vk::ImageMemoryBarrier2> barriers;
            auto res_reqs = node->get_resource_requirements();
            for (const auto& [ id, resource ] : node->resources())
            {
                if (resource->type() != ResourceType::eImage) continue;
                auto r_image = resource->as<ImageResource>();
                if (r_image.get_image()->state().layout == vk::ImageLayout::eDepthAttachmentOptimal) continue;

                auto fnd = std::ranges::find_if(res_reqs, [&, id](const auto& rr){ return rr->name == id; });
                if (fnd == std::end(res_reqs))
                {
                    continue;
                }
                ImageRequirement req = (*fnd)->as<ImageRequirement>();

                const auto& image = r_image.get_image();

                auto barrier = nvk::ImageBarrier(image, image->state().layout, req.expected_layout);
                barriers.push_back(barrier.barrier());

                // Interesting assumption but let's go with this for now
                image->update_state({vk::AccessFlagBits2::eNone, req.expected_layout});
            }

            auto barrier_dependency_info = vk::DependencyInfo()
                .setPImageMemoryBarriers(barriers.data())
                .setImageMemoryBarrierCount(barriers.size());

            command_buffer.pipelineBarrier2(barrier_dependency_info);
            #pragma endregion

            node->execute(command_buffer);

            #ifdef NBL_DEBUG
            pop_debug_label(command_buffer);
            #endif
        }
    }

    void RenderPath::initialize(const vk::CommandBuffer& command_buffer)
    {
        for (const auto& [id, resource] : m_resources)
        {
            if (resource->type() == ResourceType::eImage)
            {
                auto image = resource->as<ImageResource>().get_image();
                nvk::ImageBarrier(image, image->state().layout, vk::ImageLayout::eGeneral).apply(command_buffer);
            }
        }

        for (const auto& node : m_nodes)
        {
            node->initialize();
        }

        m_initialized = true;
    }
}
