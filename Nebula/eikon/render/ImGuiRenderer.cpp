#include "ImGuiRenderer.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <nvk/Instance.hpp>

#include "base/UIItem.hpp"

namespace Eikon
{
    ImGuiRenderer::ImGuiRenderer(const ImGuiRendererParams& params)
    : m_context(params.context)
    , m_swapchain(params.swapchain)
    {
        init_resources();
        init_imgui(params);
    }

    std::shared_ptr<ImGuiRenderer> ImGuiRenderer::createImGuiRenderer(const ImGuiRendererParams& params)
    {
        return std::make_shared<ImGuiRenderer>(params);
    }

    void ImGuiRenderer::render(const vk::CommandBuffer& command_buffer, const std::vector<std::shared_ptr<UIItem>>& ui_items)
    {
        BEGIN_DEUBG_LABEL(imgui, "ImGui", 0.8235f, 0.0588f, 0.2235f);

        m_active_render_pass->execute(command_buffer, m_framebuffer_ring->next(), [&](const auto& cmd) {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();
            for (const auto& item : ui_items)
            {
                item->draw();
            }
            ImGui::EndFrame();

            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();
            ImGui_ImplVulkan_RenderDrawData(draw_data, cmd);
        });

        END_DEBUG_LABEL;
    }

    void ImGuiRenderer::set_mode(const bool ui_only)
    {
        m_active_render_pass = ui_only ? m_rp_ui_only : m_rp_default;
    }

    void ImGuiRenderer::init_resources()
    {
        const vk::Device& vulkan_device = m_context->device()->handle();

        auto default_rp_ci = nvk::RenderPassCreateInfo()
            .add_color_attachment(m_swapchain->format(), vk::ImageLayout::ePresentSrcKHR,
                vk::SampleCountFlagBits::e1, { 0.0f, 0.0f, 0.0f, 0.0f }, vk::AttachmentLoadOp::eDontCare)
            .set_render_area({{0, 0}, m_swapchain->extent()})
            .set_name("ImGui Default");
        m_rp_default = nvk::RenderPass::create(default_rp_ci, m_context->device());

        auto ui_only_rp_ci = nvk::RenderPassCreateInfo()
            .add_color_attachment(m_swapchain->format(), vk::ImageLayout::ePresentSrcKHR,
                vk::SampleCountFlagBits::e1, { 0.0f, 0.0f, 0.0f, 0.0f }, vk::AttachmentLoadOp::eClear)
            .set_render_area({{0, 0}, m_swapchain->extent()})
            .set_name("ImGui UI Only");
        m_rp_ui_only = nvk::RenderPass::create(ui_only_rp_ci, m_context->device());

        auto framebuffer_create_info = nvk::FramebufferCreateInfo()
            .set_framebuffer_count(m_swapchain->image_count())
            .add_attachment(m_swapchain->image_view(0), 0, 0)
            .add_attachment(m_swapchain->image_view(1), 0, 1)
            .set_render_pass(m_rp_default->render_pass())
            .set_extent(m_swapchain->extent())
            .set_name("ImGui");
        m_framebuffers = nvk::Framebuffer::create(framebuffer_create_info, m_context->device());

        const auto& framebuffers = m_framebuffers->framebuffers();
        m_framebuffer_ring = std::make_unique<RingWrapper<vk::Framebuffer>>(framebuffers);

        const vk::DescriptorPoolSize pool_sizes[] {
        #pragma region PoolSize Array
            { vk::DescriptorType::eSampler, 1000 },
            { vk::DescriptorType::eCombinedImageSampler, 1000 },
            { vk::DescriptorType::eSampledImage, 1000 },
            { vk::DescriptorType::eStorageImage, 1000 },
            { vk::DescriptorType::eUniformTexelBuffer, 1000 },
            { vk::DescriptorType::eStorageTexelBuffer, 1000 },
            { vk::DescriptorType::eUniformBuffer, 1000 },
            { vk::DescriptorType::eStorageBuffer, 1000 },
            { vk::DescriptorType::eUniformBufferDynamic, 1000 },
            { vk::DescriptorType::eStorageBufferDynamic, 1000 },
            { vk::DescriptorType::eInputAttachment, 1000 }
        };
        #pragma endregion

        auto pool_info = vk::DescriptorPoolCreateInfo()
            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)
            .setMaxSets(1000 * IM_ARRAYSIZE(pool_sizes))
            .setPPoolSizes(pool_sizes)
            .setPoolSizeCount(IM_ARRAYSIZE(pool_sizes));

        if (const vk::Result result = vulkan_device.createDescriptorPool(&pool_info, nullptr, &m_descriptor_pool);
            result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create ImGui DescriptorPool");
        }
    }

    void ImGuiRenderer::init_imgui(const ImGuiRendererParams& params) const
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        const ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(params.font_file.c_str(), 16.0f);

        ImGui::StyleColorsDark();
        const ImGuiStyle& style = ImGui::GetStyle();

        const auto window = params.window.lock();
        if (!window) throw std::runtime_error("invalid window pointer");
        ImGui_ImplGlfw_InitForVulkan(window->handle(), true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_context->instance()->handle();
        init_info.PhysicalDevice = m_context->device()->physical_device();
        init_info.Device = m_context->device()->handle();
        init_info.QueueFamily = m_context->device()->q_general()->family_index;
        init_info.Queue = m_context->device()->q_general()->queue;
        init_info.PipelineCache = m_pipeline_cache;
        init_info.DescriptorPool = m_descriptor_pool;
        init_info.Subpass = 0;
        init_info.ImageCount = m_swapchain->image_count();
        init_info.MinImageCount = m_swapchain->image_count();
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = nullptr;
        init_info.RenderPass = m_rp_default->render_pass();

        ImGui_ImplVulkan_Init(&init_info);
        ImGui_ImplVulkan_CreateFontsTexture();
    }
}
