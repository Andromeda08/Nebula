#include "Swapchain.hpp"
#include "Utilities.hpp"

namespace Nebula::nvk
{
    Swapchain::Swapchain(const SwapchainCreateInfo& create_info)
    : m_window(create_info.window)
    , m_device(create_info.context->device())
    , m_surface(create_info.context->surface())
    {
        set_swapchain_parameters(create_info);
        create_swapchain();
        acquire_images();
        create_image_views();
        create_sync_objects();
        make_scissor_and_viewport();

        print_success("Created {} with {} Images\n\tSize: [{}x{}]\n\tFormat: {} | ColorSpace: {}\n\tPresent Mode: {}",
                      Format::cyan("vk::Swapchain"), m_image_count, m_size.width, m_size.height, to_string(m_format),
                      to_string(m_color_space), to_string(m_present_mode));
    }

    std::shared_ptr<Swapchain> Swapchain::create(const SwapchainCreateInfo& create_info)
    {
        return std::make_shared<Swapchain>(create_info);
    }

    Swapchain::~Swapchain()
    {
        destroy();
        print_verbose("Destroyed Swapchain");
    }

    uint32_t Swapchain::acquire_next_image(uint32_t current_frame) const
    {
        vk::Result result;
        vk::Fence fence = m_frame_in_flight[current_frame];
        result = m_device->handle().waitForFences(1, &fence, true, std::numeric_limits<uint64_t>::max());
        result = m_device->handle().resetFences(1, &fence);
        return m_device->handle().acquireNextImageKHR(m_swapchain, std::numeric_limits<uint64_t>::max(),
                                                      m_image_ready[current_frame], nullptr).value;
    }

    void Swapchain::submit_and_present(uint32_t current_frame, uint32_t acquired_image,
                                       const vk::CommandBuffer& command_buffer) const
    {
        vk::Semaphore wait_semaphores[] = { m_image_ready[current_frame] };
        vk::Semaphore signal_semaphores[] = { m_rendering_finished[current_frame] };
        vk::PipelineStageFlags wait_stages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };

        auto submit_info = vk::SubmitInfo()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(wait_semaphores)
            .setPWaitDstStageMask(wait_stages)
            .setCommandBufferCount(1)
            .setCommandBuffers(command_buffer)
            .setSignalSemaphoreCount(1)
            .setPSignalSemaphores(signal_semaphores);

        if (const vk::Result result = m_device->q_general()->queue.submit(1, &submit_info, m_frame_in_flight[current_frame]);
            result != vk::Result::eSuccess)
        {
            throw make_exception("Failed to submit command buffer ({})", to_string(result));
        }

        auto present_info = vk::PresentInfoKHR()
            .setWaitSemaphoreCount(1)
            .setPWaitSemaphores(signal_semaphores)
            .setSwapchainCount(1)
            .setPSwapchains(&m_swapchain)
            .setImageIndices(acquired_image)
            .setPResults(nullptr);
        if (const vk::Result result = m_device->q_general()->queue.presentKHR(&present_info);
            result != vk::Result::eSuccess)
        {
            throw make_exception("Failed to present frame ({})", to_string(result));
        }
    }

    void Swapchain::set_viewport_scissor(const vk::CommandBuffer& command_buffer) const
    {
        command_buffer.setScissor(0, 1, &m_cached_scissor);
        command_buffer.setViewport(0, 1, &m_cached_viewport);

    }

    const vk::Rect2D& Swapchain::get_scissor() const
    {
        return m_cached_scissor;
    }

    const vk::Viewport& Swapchain::get_viewport() const
    {
        return m_cached_viewport;
    }

    float Swapchain::aspect_ratio() const
    {
        return static_cast<float>(m_size.width) / static_cast<float>(m_size.height);
    }

    const vk::Image& Swapchain::image(uint32_t i) const
    {
        if (i > m_images.size())
        {
            throw make_exception<std::out_of_range>("Swapchain Image index {} out of bounds", i);
        }
        return m_images[i];
    }

    const vk::ImageView& Swapchain::image_view(uint32_t i) const
    {
        if (i > m_image_views.size())
        {
            throw make_exception<std::out_of_range>("Swapchain ImageView index {} out of bounds", i);
        }
        return m_image_views[i];
    }

    void Swapchain::destroy()
    {
        for (auto& s : m_image_ready) m_device->handle().destroy(s);
        for (auto& s : m_rendering_finished) m_device->handle().destroy(s);
        for (auto& f : m_frame_in_flight) m_device->handle().destroy(f);

        for (auto& image_view : m_image_views)
        {
            m_device->handle().destroy(image_view);
        }

        m_device->handle().destroy(m_swapchain);
    }

    void Swapchain::set_swapchain_parameters(const SwapchainCreateInfo& create_info)
    {
        const auto& pd = m_device->physical_device();
        vk::SurfaceCapabilitiesKHR        surface_caps    = pd.getSurfaceCapabilitiesKHR(m_surface);
        std::vector<vk::SurfaceFormatKHR> surface_formats = pd.getSurfaceFormatsKHR(m_surface);
        std::vector<vk::PresentModeKHR>   present_modes   = pd.getSurfacePresentModesKHR(m_surface);

        m_image_count = create_info.image_count;
        m_pre_transform = surface_caps.currentTransform;

        // Capability Checks
        if (surface_caps.minImageCount > m_image_count ||
            surface_caps.maxImageCount < m_image_count)
        {
            throw make_exception("Swapchain image count {} out of supported range", m_image_count);
        }

        if (surface_caps.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            m_size = vk::Extent2D { surface_caps.currentExtent.width, surface_caps.currentExtent.height };
        }

        vk::Extent2D min = surface_caps.minImageExtent;
        vk::Extent2D max = surface_caps.maxImageExtent;

        vk::Extent2D wsize = m_window->framebuffer_size();
        m_size.width = std::clamp(wsize.width, min.width, max.width);
        m_size.height = std::clamp(wsize.height, min.height, max.height);

        // Surface Format & ColorSpace
        if (surface_formats.empty())
        {
            throw make_exception("No surface formats found");
        }
        m_format      = surface_formats[0].format;
        m_color_space = surface_formats[0].colorSpace;
        for (const auto& format : surface_formats)
        {
            if (format.format == create_info.pref_format &&
                format.colorSpace == create_info.pref_color_space)
            {
                m_format = format.format;
                m_color_space = format.colorSpace;
                break;
            }
        }

        // Present Mode
        if (present_modes.empty())
        {
            throw make_exception("No present modes found");
        }
        m_present_mode = present_modes[0];
        auto it = std::ranges::find(present_modes, create_info.pref_present_mode);
        if (it != std::end(present_modes))
        {
            m_present_mode = create_info.pref_present_mode;
        }
    }

    void Swapchain::create_swapchain()
    {
        auto create_info = vk::SwapchainCreateInfoKHR()
            .setSurface(m_surface)
            .setMinImageCount(m_image_count)
            .setImageFormat(m_format)
            .setImageColorSpace(m_color_space)
            .setImageExtent(m_size)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
            .setPreTransform(m_pre_transform)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setPresentMode(m_present_mode)
            .setClipped(true)
            .setOldSwapchain(nullptr)
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setQueueFamilyIndexCount(0)
            .setPQueueFamilyIndices(nullptr);

        if (const vk::Result result = m_device->handle().createSwapchainKHR(&create_info, nullptr, &m_swapchain);
            result != vk::Result::eSuccess)
        {
            throw make_exception("Failed to create vk::SwapchainKHR ({})", to_string(result));
        }
    }

    void Swapchain::acquire_images()
    {
        m_images.resize(m_image_count);
        m_images = m_device->handle().getSwapchainImagesKHR(m_swapchain);
    }

    void Swapchain::create_image_views()
    {
        vk::ComponentMapping component_mapping = {
            vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
            vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity
        };

        auto create_info = vk::ImageViewCreateInfo()
            .setComponents(component_mapping)
            .setFormat(m_format)
            .setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 })
            .setViewType(vk::ImageViewType::e2D);

        m_image_views.resize(m_image_count);
        for (uint32_t i = 0; i < m_image_count; i++)
        {
            create_info.setImage(m_images[i]);
            if (const vk::Result result = m_device->handle().createImageView(&create_info, nullptr, &m_image_views[i]);
                result != vk::Result::eSuccess)
            {
                throw make_exception("Failed to create vk::ImageView #{} for Swapchain", i);
            }

            m_device->name_object(m_images[i], fmt::format("Swapchain #{}", i), vk::ObjectType::eImage);
            m_device->name_object(m_image_views[i], fmt::format("Swapchain ImageView #{}", i), vk::ObjectType::eImageView);
        }
    }

    void Swapchain::create_sync_objects()
    {
        vk::Result result;
        m_image_ready.resize(m_image_count);
        m_rendering_finished.resize(m_image_count);
        m_frame_in_flight.resize(m_image_count);

        for (uint32_t i = 0; i < m_image_count; i++)
        {
            auto sci = vk::SemaphoreCreateInfo();
            auto fci = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

            result = m_device->handle().createSemaphore(&sci, nullptr, &m_image_ready[i]);
            result = m_device->handle().createSemaphore(&sci, nullptr, &m_rendering_finished[i]);
            result = m_device->handle().createFence(&fci, nullptr, &m_frame_in_flight[i]);
        }
    }

    void Swapchain::make_scissor_and_viewport()
    {
        m_cached_scissor = vk::Rect2D {{ 0, 0 }, { m_size.width, m_size.height }};
        /**
         * Create a viewport object based on the current state of the Swapchain.
         * The viewport is flipped along the Y axis for GLM compatibility.
         * This requires Maintenance1, which is included by default since API version 1.1.
         * https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/
         */
        m_cached_viewport = vk::Viewport()
            .setX(0.0f)
            .setWidth(static_cast<float>(m_size.width))
            .setY(static_cast<float>(m_size.height))
            .setHeight(-1.0f * static_cast<float>(m_size.height))
            .setMaxDepth(1.0f)
            .setMinDepth(0.0f);
    }
}