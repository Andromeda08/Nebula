#pragma once

#include <concepts>
#include <memory>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vulkan/vulkan.hpp>
#include <nlog/nlog.hpp>
#include <nrg/common/Resource.hpp>
#include <nrg/common/NodeType.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/common/ResourceSpecification.hpp>

#if !defined(DEF_RESOURCES)
#define DEF_RESOURCES()                                                                     \
public:                                                                                     \
    static const std::vector<ResourceSpecification> s_resource_specifications;              \
    const std::vector<ResourceSpecification>& get_resource_specifications() const override  \
    { return s_resource_specifications; }
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

        virtual const std::vector<ResourceSpecification>& get_resource_specifications() const = 0;

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