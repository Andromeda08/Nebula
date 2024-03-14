#include "Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace Nebula::ns
{
    Camera::Camera(glm::ivec2 size, glm::vec3 eye, float h_fov, float near, float far)
    : m_size(size), m_eye(eye), m_fov(h_fov), m_near(near), m_far(far) {}

    glm::mat4 Camera::view() const
    {
        return glm::lookAt(m_eye, m_eye + m_orientation, m_up);
    }

    glm::mat4 Camera::projection() const
    {
        return glm::perspective(glm::radians(m_fov), (float) m_size.x / (float) m_size.y, m_near, m_far);
    }

    void Camera::register_keys(GLFWwindow* p_window)
    {
        // WASD movement
        if (glfwGetKey(p_window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_eye += m_speed * m_orientation;
        }
        if (glfwGetKey(p_window, GLFW_KEY_A) == GLFW_PRESS)
        {
            m_eye += m_speed * -glm::normalize(glm::cross(m_orientation, m_up));
        }
        if (glfwGetKey(p_window, GLFW_KEY_S) == GLFW_PRESS)
        {
            m_eye += m_speed * -m_orientation;
        }
        if (glfwGetKey(p_window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_eye += m_speed * glm::normalize(glm::cross(m_orientation, m_up));
        }

        // Move up & down
        if (glfwGetKey(p_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            m_eye += (m_speed / 2.0f) * m_up;
        }
        if (glfwGetKey(p_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            m_eye -= (m_speed / 2.0f) * m_up;
        }

        // Exit on ESC
        if (glfwGetKey(p_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(p_window, true);
        }
    }

    void Camera::register_mouse(GLFWwindow* p_window)
    {
        if (glfwGetMouseButton(p_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            if (m_click)
            {
                glfwSetCursorPos(p_window, (m_size.x / 2), (m_size.y / 2));
                m_click = false;
            }

            double mouseX, mouseY;
            glfwGetCursorPos(p_window, &mouseX, &mouseY);

            float rotX = m_sensitivity * (float)(mouseY - (m_size.y / 2)) / m_size.y;
            float rotY = m_sensitivity * (float)(mouseX - (m_size.x / 2)) / m_size.x;

            glm::vec3 newOrientation = glm::rotate(m_orientation, glm::radians(-rotX), glm::normalize(glm::cross(m_orientation, m_up)));

            if (abs(glm::angle(newOrientation, m_up) - glm::radians(90.0f)) <= glm::radians(85.0f))
            {
                m_orientation = newOrientation;
            }

            m_orientation = glm::rotate(m_orientation, glm::radians(-rotY), m_up);
            glfwSetCursorPos(p_window, (m_size.x / 2), (m_size.y / 2));
        }
        else if (glfwGetMouseButton(p_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            glfwSetInputMode(p_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_click = true;
        }
    }

    CameraData Camera::uniform_data() const
    {
        auto v = view();
        auto p = projection();
        auto e = eye();

        return {
            .view = v,
            .proj = p,
            .view_inverse = glm::inverse(v),
            .proj_inverse = glm::inverse(p),
            .eye = { e.x, e.y, e.z, 1.0f },
            .near_plane = m_near,
            .far_plane = m_far,
        };
    }
}