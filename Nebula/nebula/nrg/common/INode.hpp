#pragma once

#include <concepts>
#include <memory>
#include <map>
#include <string>
#include <type_traits>
#include <vulkan/vulkan.hpp>
#include <nrg/common/IResource.hpp>
#include <nrg/common/NodeType.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nrg/common/ResourceSpecification.hpp>

#define DEF_RESOURCE_SPECIFICATIONS()                                           \
public:                                                                         \
    static const std::vector<ResourceSpecification> s_resource_specifications;

namespace Nebula::nrg
{
    template <typename T>
    concept HasResourceClaims = requires (T t) {
        { T::get_resource_claims() } -> std::same_as<std::vector<ResourceClaim>>;
    };

    class Node
    {
    public:
        Node(const Node&) = delete;

        Node& operator=(const Node&) = delete;

        Node() = default;

        Node(const std::string& name, const NodeType node_type) : m_name(name), m_type(node_type) {}

        virtual void initialize() {}

        virtual void execute(const vk::CommandBuffer& command_buffer) {}

        virtual const std::vector<ResourceSpecification>& get_resource_specifications() const = 0;

        virtual bool set_resource(const std::string& key, const std::shared_ptr<Resource>& resource)
        {
            if (!m_resources.contains(key)) {
                m_resources.insert({key, resource});
                return true;
            }

            m_resources[key] = resource;
            return true;
        }

        virtual ~Node() = default;

        template<typename T>
        T& get_resource(const std::string& key)
        {
            static_assert(std::is_base_of_v<Resource, T>, "Template parameter T must be a valid Resource type");
            if (m_resources.contains(key)) {
                return m_resources[key]->as<T>();
            }
            throw std::runtime_error(std::format("No valid resource by ID {} was found for Node {}", key, m_name));
        }

        std::map<std::string, std::shared_ptr<Resource>>& resources() { return m_resources; }

        const std::string& name() const { return m_name; }

    protected:
        std::map<std::string, std::shared_ptr<Resource>> m_resources;

    private:
        const std::string m_name{"Unknown Node"};
        const NodeType m_type{NodeType::eUnknown};
    };
}