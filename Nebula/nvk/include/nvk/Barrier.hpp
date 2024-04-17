#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include "Buffer.hpp"
#include "Image.hpp"
#include "Utility.hpp"

namespace Nebula::nvk
{
    struct Barrier
    {
        Barrier() = default;
        virtual void apply(const vk::CommandBuffer& command_buffer) = 0;
        virtual void revert(const vk::CommandBuffer& command_buffer) = 0;
        virtual ~Barrier() = default;

    protected:
        vk::DependencyInfo  m_dependency_info {};
        bool                m_can_revert {false};
    };

    class ImageBarrier : public Barrier
    {
    public:
        ImageBarrier(const std::shared_ptr<Image>& image, vk::ImageLayout old_layout, vk::ImageLayout new_layout);

        ~ImageBarrier() override = default;

        void apply(const vk::CommandBuffer& command_buffer) override;

        void revert(const vk::CommandBuffer& command_buffer) override;

        const vk::ImageMemoryBarrier2& barrier() const { return m_barrier; }

    private:
        std::shared_ptr<Image>      m_image;
        vk::ImageMemoryBarrier2     m_barrier {};
        vk::ImageMemoryBarrier2     m_reverse_barrier {};
    };

//    class BufferBarrier : public Barrier
//    {
//    public:
//        BufferBarrier(const std::shared_ptr<Buffer>& buffer);
//
//    private:
//        std::shared_ptr<Buffer>     m_buffer;
//        vk::BufferMemoryBarrier2    m_barrier {};
//        vk::BufferMemoryBarrier2    m_reverse_barrier {};
//    };
}