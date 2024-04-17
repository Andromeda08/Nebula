#pragma once

#include <string>
#include <type_traits>
#include <nrg/common/ResourceTraits.hpp>

#if !defined(nrg_decl_resource)
/* RenderGraph: Basic Resource Declaration Macro
 * T     - Type of Resource to declare
 * RTYPE - ResourceType enum value
 * U     - Type of the Underlying Resource
 * Name  - Name of the Underlying Resource
 */
#pragma region "nrg_decl_resource definition"
#define nrg_decl_resource(T, RTYPE, U, NAME)                                        \
class T final : public Resource {                                                   \
public:                                                                             \
    explicit T(const std::shared_ptr<U>& p_##NAME, const std::string& name = #T)    \
    : Resource(name, RTYPE), m_resource(p_##NAME) {}                                \
    ~T() override = default;                                                        \
    bool is_valid() override { return m_resource != nullptr; }                      \
    const std::shared_ptr<U>& get_##NAME() const noexcept { return m_resource; }    \
    const U& ref_##NAME() const noexcept { return *m_resource; }                    \
private:                                                                            \
    std::shared_ptr<U> m_resource;                                                  \
};
#pragma endregion
#endif

namespace Nebula::nrg
{
    class Resource
    {
    public:
        Resource(std::string name, const ResourceType resource_type)
        : m_name(std::move(name)), m_type(resource_type) {}

        template <typename T>
        T& as()
        {
            static_assert(std::is_base_of_v<Resource, T>, "Template parameter T must be a valid Resource type");
            return dynamic_cast<T&>(*this);
        }

        virtual bool is_valid() { return true; }

        virtual ~Resource() = default;

        const std::string& name() const noexcept { return m_name; }

        ResourceType type() const noexcept { return m_type; }

    private:
        std::string  m_name {"Unknown Resource"};
        ResourceType m_type {ResourceType::eUnknown};
    };
}