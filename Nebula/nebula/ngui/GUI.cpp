#include "GUI.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imnodes.h>
#include <nvk/Instance.hpp>
#include <nvk/render/RenderPass.hpp>

namespace Nebula::ngui
{
    GUI::GUI(const std::string& font_path,
             const std::shared_ptr<wsi::Window>& window,
             const std::shared_ptr<nvk::Context>& context,
             const std::shared_ptr<nvk::Swapchain>& swapchain)
    : m_window(window) ,m_context(context), m_swapchain(swapchain)
    {
        init_imgui(font_path);
    }

    void GUI::render(const vk::CommandBuffer& command_buffer, const std::function<void()>& lambda)
    {
        nvk::RenderPass::Execute()
            .with_clear_values<1>(m_clear_value)
            .with_framebuffer(m_framebuffers[m_next_framebuffer])
            .with_render_area({{0, 0}, m_swapchain->extent()})
            .with_render_pass(m_render_pass)
            .execute(command_buffer, [&](const vk::CommandBuffer& cmd){
                nvk::MemoryUsage mem_usage = m_context->device()->get_memory_usage();
                const ImGuiIO& io = ImGui::GetIO();
                ImGui_ImplVulkan_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();
                {
                    ImGui::Begin("Metrics");
                    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);
                    ImGui::Text("Total Memory Usage: %.2f %s", mem_usage.usage, mem_usage.usage_coeff.c_str());
                    ImGui::Text("Available Memory Budget: %.2f %s", mem_usage.budget, mem_usage.budget_coeff.c_str());
                    ImGui::End();

                    lambda();
                }
                ImGui::EndFrame();

                ImGui::Render();
                ImDrawData* main_draw_data = ImGui::GetDrawData();
                ImGui_ImplVulkan_RenderDrawData(main_draw_data, cmd);
            });

        m_next_framebuffer = (m_next_framebuffer + 1) % 2;
    }

    void GUI::init_imgui(const std::string& font_path)
    {
        m_render_pass = nvk::RenderPass::Builder()
            .add_color_attachment(m_swapchain->format(), vk::ImageLayout::ePresentSrcKHR, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eLoad)
            .make_subpass()
            .with_name("ImGui RenderPass")
            .create(m_context->device());

        m_clear_value[0].color = std::array<float, 4>({ 0.f, 0.f, 0.f, 0.f });

        const auto& device = m_context->device()->handle();
        std::vector attachments = { m_swapchain->image_view(0) };
        auto fb_create_info = vk::FramebufferCreateInfo()
            .setAttachmentCount(attachments.size())
            .setPAttachments(attachments.data())
            .setRenderPass(m_render_pass)
            .setHeight(m_swapchain->extent().height)
            .setWidth(m_swapchain->extent().width)
            .setLayers(1);

        for (int32_t i = 0; i < m_swapchain->image_count(); i++)
        {
            attachments[0] = m_swapchain->image_view(i);
            if (const vk::Result result = device.createFramebuffer(&fb_create_info, nullptr, &m_framebuffers[i]);
                result != vk::Result::eSuccess)
            {
                throw std::runtime_error(std::format("[Error] Failed to create ImGui Framebuffer #{}", i));
            }
        }

        #pragma region PoolSize Array
        const vk::DescriptorPoolSize pool_sizes[] {
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

        if (const vk::Result result = device.createDescriptorPool(&pool_info, nullptr, &m_descriptor_pool);
            result != vk::Result::eSuccess)
        {
            throw std::runtime_error("[Error] Failed to create ImGui DescriptorPool");
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.FontGlobalScale = get_ui_scale();
        io.Fonts->AddFontFromFileTTF(font_path.c_str(), get_font_size());

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(get_ui_scale());

        ImGui_ImplGlfw_InitForVulkan(m_window->handle(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_context->instance()->handle();
        init_info.PhysicalDevice = m_context->device()->physical_device();
        init_info.Device = device;
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
        init_info.RenderPass = m_render_pass;
        ImGui_ImplVulkan_Init(&init_info);

        ImGui_ImplVulkan_CreateFontsTexture();
        //ImGui_ImplVulkan_DestroyFontUploadObjects();

        ImNodes::CreateContext();
        ImNodes::StyleColorsDark();
    }

    float GUI::get_ui_scale() const
    {
        if (m_swapchain->extent().width <= 1920 && m_swapchain->extent().height <= 1080)
        {
            return 1.0f;
        }

        return 1.5f;
    }

    float GUI::get_font_size() const
    {
        if (m_swapchain->extent().width <= 1920 && m_swapchain->extent().height <= 1080)
        {
            return 16.0f;
        }

        return 18.0f;
    }

    bool GUI::want_capture_mouse() const
    {
        const ImGuiIO& io = ImGui::GetIO();
        return io.WantCaptureMouse;
    }

    bool GUI::want_capture_keyboard() const
    {
        const ImGuiIO& io = ImGui::GetIO();
        return io.WantCaptureKeyboard;
    }
}