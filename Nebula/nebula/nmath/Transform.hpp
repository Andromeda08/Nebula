#pragma once

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <vulkan/vulkan.hpp>

namespace Nebula::nmath
{
    struct Transform
    {
        glm::vec3 translate = glm::vec3(0.0f);
        glm::vec3 scale     = glm::vec3(1.0f);
        glm::vec3 euler     = glm::vec3(0.0f);

        glm::mat4 model() const
        {
            glm::mat4 T = glm::translate(glm::mat4(1.0f), translate);
            glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
            glm::mat4 R = glm::yawPitchRoll(euler.y, euler.x, euler.z);
            return T * R * S;
        }

        vk::TransformMatrixKHR model3x4() const
        {
            glm::mat4 m = model();
            return vk::TransformMatrixKHR({
                std::array { m[0].x, m[1].x, m[2].x, m[3].x },
                std::array { m[0].y, m[1].y, m[2].y, m[3].y },
                std::array { m[0].z, m[1].z, m[2].z, m[3].z }
            });
        }
    };
}