#include "Barrier.hpp"

namespace Nebula::nvk
{
    ImageBarrier::ImageBarrier(const std::shared_ptr<Image>& image, vk::ImageLayout old_layout,
                               vk::ImageLayout new_layout): m_image(image)
    {
        auto& props = image->properties();

        m_barrier = vk::ImageMemoryBarrier2()
            .setOldLayout(old_layout)
            .setNewLayout(new_layout)
            .setSrcAccessMask(vk::AccessFlagBits2::eNone)
            .setDstAccessMask(vk::AccessFlagBits2::eNone)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eNone)
            .setDstStageMask(vk::PipelineStageFlagBits2::eNone)
            .setSubresourceRange(props.subresource_range)
            .setImage(m_image->image());

        m_reverse_barrier = vk::ImageMemoryBarrier2()
            .setNewLayout(old_layout)
            .setOldLayout(new_layout)
            .setSrcAccessMask(vk::AccessFlagBits2::eNone)
            .setDstAccessMask(vk::AccessFlagBits2::eNone)
            .setSrcStageMask(vk::PipelineStageFlagBits2::eNone)
            .setDstStageMask(vk::PipelineStageFlagBits2::eNone)
            .setSubresourceRange(props.subresource_range)
            .setImage(m_image->image());

        m_dependency_info.setImageMemoryBarrierCount(1);
    }

    void ImageBarrier::apply(const vk::CommandBuffer& command_buffer)
    {
        m_image->update_state({ vk::AccessFlagBits2::eNone, m_barrier.newLayout });

        m_dependency_info.setPImageMemoryBarriers(&m_barrier);
        command_buffer.pipelineBarrier2(&m_dependency_info);

        m_can_revert = true;
    }

    void ImageBarrier::revert(const vk::CommandBuffer& command_buffer)
    {
        if (!m_can_revert) return;

        m_image->update_state({ vk::AccessFlagBits2::eNone, m_barrier.oldLayout });

        m_dependency_info.setPImageMemoryBarriers(&m_reverse_barrier);
        command_buffer.pipelineBarrier2(&m_dependency_info);

        m_can_revert = false;
    }
}