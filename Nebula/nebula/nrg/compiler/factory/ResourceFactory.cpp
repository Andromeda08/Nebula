#include "ResourceFactory.hpp"
#include <nlog/nlog.hpp>
#include <nrg/resource/Resources.hpp>

namespace Nebula::nrg
{

    ResourceFactory::ResourceFactory(const std::shared_ptr<Context>& context)
    : m_context(context)
    {
    }

    std::shared_ptr<Resource> ResourceFactory::create(const ResourceCreateInfo& create_info)
    {
        switch (create_info.type)
        {
            case ResourceType::eSceneData: {
                return std::make_shared<SceneResource>(m_context->get_selected_scene(),
                                                       create_info.name);
            }
            case ResourceType::eImage: {
                bool is_depth_image = (create_info.format == vk::Format::eD32Sfloat);
                vk::Extent2D extent = create_info.claim.req->as<ImageRequirement>().extent;

                if (extent.width == 0 || extent.height == 0)
                {
                    extent = m_context->m_swapchain->extent();
                }

                using enum vk::ImageAspectFlagBits;
                using enum vk::ImageUsageFlagBits;
                auto image_info = nvk::ImageCreateInfo()
                    .set_aspect_flags((is_depth_image ? eDepth : eColor))
                    .set_extent(extent)
                    .set_format(create_info.format)
                    .set_memory_property_flags(vk::MemoryPropertyFlagBits::eDeviceLocal)
                    .set_name(create_info.name)
                    .set_sample_count(vk::SampleCountFlagBits::e1)
                    .set_usage_flags((is_depth_image ? eSampled | eDepthStencilAttachment : create_info.usage_flags | eColorAttachment))
                    .set_tiling(vk::ImageTiling::eOptimal)
                    .set_with_sampler(true);
                return std::make_shared<ImageResource>(nvk::Image::create(image_info, m_context->m_device),
                                                       create_info.name);
            }
            case ResourceType::eImageArray: {
                throw nlog::make_exception("ImageArray Resources are not supported yet.");
            }
            case ResourceType::eStorageBuffer: {
                throw nlog::make_exception("StorageBuffer Resources are not supported yet.");
            }
            default:
                return nullptr;
        }
    }
}