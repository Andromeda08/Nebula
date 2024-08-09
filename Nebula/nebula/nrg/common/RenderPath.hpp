#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <nrg/common/Node.hpp>
#include <nrg/resource/Resource.hpp>
#include <nrg/resource/Resources.hpp>
#include <nvk/Barrier.hpp>

namespace vk
{
    class CommandBuffer;
}

namespace Nebula::nrg
{
    struct RenderPath
    {
        std::vector<std::shared_ptr<Node>>               nodes;
        std::map<std::string, std::shared_ptr<Resource>> resources;

        void execute(const vk::CommandBuffer& command_buffer)
        {
            if (!m_initialized)
            {
                for (const auto& [id, resource] : resources)
                {
                    if (resource->type() == ResourceType::eImage)
                    {
                        auto image = resource->as<ImageResource>().get_image();
                        nvk::ImageBarrier(image, image->state().layout, vk::ImageLayout::eGeneral).apply(command_buffer);
                    }
                }

                for (const auto& node : nodes)
                {
                    node->initialize();
                }

                m_initialized = true;
            }

            for (const auto& node : nodes)
            {
                if (node->type() == NodeType::eSceneDataProvider)
                {
                    continue;
                }

                auto node_marker = vk::DebugUtilsLabelEXT()
                    .setColor(node->marker_color())
                    .setPLabelName(node->name().c_str());
                command_buffer.beginDebugUtilsLabelEXT(&node_marker);
                {
                    auto update_label = std::format("{} - Update", node->name());
                    auto update_marker = vk::DebugUtilsLabelEXT()
                        .setColor(std::array<float, 4>{ 0.345f, 0.949f, 0.898f, 1.0f})
                        .setPLabelName(update_label.c_str());
                    command_buffer.beginDebugUtilsLabelEXT(&update_marker);
                    {
                        node->update();
                    }
                    command_buffer.endDebugUtilsLabelEXT();

                    std::vector<vk::ImageMemoryBarrier2> barriers;
                    auto res_reqs = node->get_resource_requirements();
                    for (const auto& [ id, resource ] : node->resources())
                    {
                        if (resource->type() != ResourceType::eImage) continue;
                        auto r_image = resource->as<ImageResource>();
                        if (r_image.get_image()->state().layout == vk::ImageLayout::eDepthAttachmentOptimal) continue;

                        auto fnd = std::ranges::find_if(res_reqs, [&](const auto& rr){ return rr->name == id; });
                        if (fnd == std::end(res_reqs))
                        {
                            continue;
                        }
                        ImageRequirement req = (*fnd)->as<ImageRequirement>();

                        const auto& image = r_image.get_image();

                        auto barrier = nvk::ImageBarrier(image, image->state().layout, req.expected_layout);
                        barriers.push_back(barrier.barrier());

                        // Interesting assumption but let's go with this for now
                        image->update_state({ vk::AccessFlagBits2::eNone, req.expected_layout });
                    }

                    auto barrier_dependency_info = vk::DependencyInfo()
                        .setPImageMemoryBarriers(barriers.data())
                        .setImageMemoryBarrierCount(barriers.size());

                    command_buffer.pipelineBarrier2(barrier_dependency_info);

                    auto execute_label = std::format("{} - Execute", node->name());
                    auto execute_marker = vk::DebugUtilsLabelEXT()
                        .setColor(std::array<float, 4>{ 0.345f, 0.396f, 0.949f, 1.0f })
                        .setPLabelName(execute_label.c_str());
                    command_buffer.beginDebugUtilsLabelEXT(&execute_marker);
                    {
                        node->execute(command_buffer);
                    }
                    command_buffer.endDebugUtilsLabelEXT();
                }
                command_buffer.endDebugUtilsLabelEXT();
            }
        }

    private:
        bool m_initialized {false};
    };
}