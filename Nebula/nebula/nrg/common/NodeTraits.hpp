#pragma once

#include <fmt/format.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <glm/vec4.hpp>
#include <imgui.h>
#include <imnodes.h>

#define TO_IM_COL32(color) IM_COL32(color.r, color.g, color.b, 255)

namespace Nebula::nrg
{
    enum class NodeType
    {
        eAmbientOcclusion,
        eAntiAliasing,
        eBloom,
        eDeferredLighting,
        eDenoise,
        eGaussianBlur,
        eGBuffer,
        eHairRender,
        eHairSimulation,
        eMeshShaderGBuffer,
        eRayTracing,
        eShadowMapGeneration,
        eToneMapping,

        // Unique Types
        ePresent,
        eSceneDataProvider,

        eUnknown
    };

    inline NodeType to_node_type(const std::string& str)
    {
        using enum NodeType;

        if (str == "AmbientOcclusion")      return eAmbientOcclusion;
        if (str == "AntiAliasing")          return eAntiAliasing;
        if (str == "Bloom")                 return eBloom;
        if (str == "DeferredLighting")      return eDeferredLighting;
        if (str == "Denoise")               return eDenoise;
        if (str == "GaussianBlur")          return eGaussianBlur;
        if (str == "GBuffer")               return eGBuffer;
        if (str == "HairRender")            return eHairRender;
        if (str == "HairSimulation")        return eHairSimulation;
        if (str == "MeshShaderGBuffer")     return eMeshShaderGBuffer;
        if (str == "RayTracing")            return eRayTracing;
        if (str == "ShadowMapGeneration")   return eShadowMapGeneration;
        if (str == "ToneMapping")           return eToneMapping;
        if (str == "Present")               return ePresent;
        if (str == "SceneDataProvider")     return eSceneDataProvider;
        if (str == "Unknown")               return eUnknown;

        throw std::runtime_error(fmt::format(R"(Unknown NodeType "{}")", str));
    }

    inline std::string to_string(const NodeType node_type)
    {
        using enum NodeType;
        switch (node_type)
        {
            case eAmbientOcclusion:     return "Ambient Occlusion";
            case eAntiAliasing:         return "Anti-Aliasing";
            case eBloom:                return "Bloom";
            case eDeferredLighting:     return "Deferred Lighting Pass";
            case eDenoise:              return "Denoise";
            case eGaussianBlur:         return "Gaussian Blur";
            case eGBuffer:              return "G-Buffer Pass";
            case eHairRender:           return "Hair Render";
            case eHairSimulation:       return "Hair Simulation";
            case eMeshShaderGBuffer:    return "Mesh G-Buffer Pass";
            case eRayTracing:           return "Raytracing";
            case eShadowMapGeneration:  return "Shadow Map Generation";
            case eToneMapping:          return "Tone Mapping";

            case ePresent:              return "Present";
            case eSceneDataProvider:    return "Scene Data Provider";

            default:                    return "Unknown";
        }
    }

    inline std::vector<NodeType> get_all_node_types()
    {
        using enum NodeType;
        return {
            eAmbientOcclusion, eAntiAliasing, eBloom, eDeferredLighting, eDenoise, eGaussianBlur,
            eGBuffer, eHairRender, eHairSimulation, eMeshShaderGBuffer, eRayTracing, eShadowMapGeneration,
            eToneMapping, ePresent, eSceneDataProvider
        };
    }

    struct NodeColors
    {
        glm::ivec4 title_bar            { 128, 128, 128, 255 };
        glm::ivec4 title_bar_hovered    { 255, 255, 255, 255 };
        glm::ivec4 title_bar_selected   { 255, 255, 255, 255 };

        inline void push_color_styles() const
        {
            ImNodes::PushColorStyle(ImNodesCol_TitleBar, TO_IM_COL32(title_bar));
            ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, TO_IM_COL32(title_bar_hovered));
            ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, TO_IM_COL32(title_bar_selected));
        }

        inline static void pop_color_styles()
        {
            for (int32_t i = 0; i < 3; i++) ImNodes::PopColorStyle();
        }
    };
}