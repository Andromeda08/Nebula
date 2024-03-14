#pragma once

#include <concepts>
#include <memory>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vulkan/vulkan.hpp>
#include <nlog/nlog.hpp>
#include <nrg/common/NodeTraits.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/resource/Resource.hpp>
#include <nrg/resource/Requirement.hpp>

#if !defined(nrg_decl_resource_requirements)
#define nrg_decl_resource_requirements() \
public:                                                                                          \
    static const std::vector<std::shared_ptr<Requirement>> s_resource_requirements;              \
    const std::vector<std::shared_ptr<Requirement>>& get_resource_requirements() const override  \
    { return s_resource_requirements; }
#endif

#if !defined(nrg_def_resource_requirements)
#define nrg_def_resource_requirements(T, R) \
const std::vector<std::shared_ptr<Requirement>> T::s_resource_requirements = std::vector<std::shared_ptr<Requirement>> R;
#endif

#if !defined(nrg_def_get_resource_claims)
#define nrg_def_get_resource_claims()                       \
static std::vector<ResourceClaim> get_resource_claims() {   \
    std::vector<ResourceClaim> result;                      \
    std::transform(                                         \
        s_resource_requirements.cbegin(),                   \
        s_resource_requirements.cend(),                     \
        std::back_inserter(result),                         \
        [&](const auto& req) {                              \
            return ResourceClaim(req);                      \
        });                                                 \
    return result;                                          \
}
#endif

namespace Nebula::nrg
{
    template <typename T>
    concept HasResourceClaims = requires (T t) {
        { T::get_resource_claims() } -> std::same_as<std::vector<ResourceClaim>>;
    };

    class Node
    {
    public:
        struct Common
        {
            vk::Extent2D render_resolution {0, 0};
            uint32_t     frames_in_flight {0};
        };

        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        Node(std::string name, NodeType node_type);

        virtual ~Node() = default;

        #pragma region "Virtual methods"
        virtual void initialize() {}

        virtual void execute(const vk::CommandBuffer& command_buffer) {}

        virtual void update() {}

        virtual const std::vector<std::shared_ptr<Requirement>>& get_resource_requirements() const = 0;

        virtual bool set_resource(const std::string& key, const std::shared_ptr<Resource>& resource);
        #pragma endregion

        template<typename T>
        T& get_resource(const std::string& key);

        std::map<std::string, std::shared_ptr<Resource>>& resources();

        const std::string& name() const;

        void set_common(const Common& common);

    protected:
        Common m_common {};
        std::map<std::string, std::shared_ptr<Resource>> m_resources;

    private:
        const std::string m_name {"Unknown Node"};
        const NodeType    m_type {NodeType::eUnknown};
    };

    template<typename T>
    T& Node::get_resource(const std::string& key)
    {
        static_assert(std::is_base_of_v<Resource, T>, "Template parameter T must be a valid Resource type");
        if (m_resources.contains(key)) {
            return m_resources[key]->as<T>();
        }
        throw nlog::make_exception("No valid resource by ID {} was found for Node {}", key, m_name);
    }
}