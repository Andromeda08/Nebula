### Node Template

- Don't forget to add a `NodeType`, and the appropriate node color definitions in `nrg_editor.json` and case for `EditorNodeFactory::create()`

#### Header
```c++
#pragma once

#include <memory>
#include <vulkan/vulkan.hpp>
#include <nrg/common/Node.hpp>
#include <nrg/common/NodeConfiguration.hpp>
#include <nrg/common/NodeTraits.hpp>
#include <nrg/common/ResourceClaim.hpp>
#include <nvk/Device.hpp>

namespace Nebula::nrg
{
    // Class definition
    class T : public Node
    {
    public:
        // Optional
        struct Configuration : public NodeConfiguration
        {
            void render() override {}
            bool validate() override {}
            ~Configuration() override = default;
        };

        T(const std::shared_ptr<Configuration>& configuration, const std::shared_ptr<nvk::Device>& device)
        : Node("Unknown Node", NodeType::eUnknown)
        , m_configuration(*configuration), mDevice(device) {}

        ~T() override = default;
        
        void initialize() override {}
        
        void execute(const vk::CommandBuffer& command_buffer) override {}
        
        void update() override {}

    private:
        const Configuration          m_configuration;   // Optional
        std::shared_ptr<nvk::Device> mDevice;

        nrg_decl_resource_requirements();
        nrg_def_get_resource_claims();
    };
}
```

#### Implementation
```c++
#include "T.hpp"
namespace Nebula::nrg
{
    nrg_def_resource_requirements(T, ({
        std::make_shared<Requirement>("Resource Name", ResourceUsage::eUnknown, ResourceType::eUnknown)
    }));
}
```

#### Velocity File Template
```
#pragma once

#[[#include]]# <memory>
#[[#include]]# <vector>
#[[#include]]# <vulkan/vulkan.hpp>
#[[#include]]# <nrg/common/Node.hpp>
#[[#include]]# <nrg/common/NodeConfiguration.hpp>
#[[#include]]# <nrg/common/NodeTraits.hpp>
#[[#include]]# <nrg/common/ResourceClaim.hpp>
#[[#include]]# <nrg/resource/Requirement.hpp>
#[[#include]]# <nvk/Device.hpp>

namespace Nebula::nrg
{
    class ${NAME} : public Node
    {
    public:
        explicit ${NAME}(const std::shared_ptr<nvk::Device>& device)
        : Node("${NAME}", NodeType::eUnknown)
        , mDevice(device) {}

        ~${NAME}() override = default;

        void initialize() override {}

        void execute(const vk::CommandBuffer& command_buffer) override {}

        void update() override {}

    private:
        std::shared_ptr<nvk::Device> mDevice;

        nrg_decl_resource_requirements();
        nrg_def_get_resource_claims();
    };
}
```

```
#[[#include]]# "${NAME}.hpp"
#[[#include]]# <nrg/common/ResourceTraits.hpp>

namespace Nebula::nrg
{
    nrg_def_resource_requirements(${NAME}, ({}));
}
```