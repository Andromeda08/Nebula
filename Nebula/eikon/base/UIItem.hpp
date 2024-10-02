#pragma once

#include <uuid.h>

namespace Eikon
{
    struct UIItem
    {
        UIItem() = default;

        [[nodiscard]] uuids::uuid uuid() const { return m_id; }

        virtual void draw() = 0;
        virtual void update() = 0;
        virtual ~UIItem() = default;

    protected:
        const uuids::uuid m_id = uuids::uuid_system_generator{}();
    };
}