#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/constants.hpp>

class Camera
{
    glm::vec3 m_pos{0, 0, 0};

    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    float m_fovY, m_aspect, m_nearZ, m_farZ;

    mutable glm::mat4 m_view, m_projection;
    mutable glm::vec3 m_forward, m_right, m_up;
    mutable bool m_dirtyView = true, m_dirtyProjection = true;

    // using simple yaw/ptch euler camera model for now
    // view matrix rebuilt 
    void RecalcView() const
    {
        m_forward = glm::normalize(glm::vec3{
            cos(m_pitch) * sin(m_yaw),
            sin(m_pitch),
            cos(m_pitch) * cos(m_yaw)});
        glm::vec3 worldUp = glm::vec3(0, 1, 0);
        m_right = glm::normalize(glm::cross(m_forward, worldUp));
        m_up = glm::cross(m_right, m_forward);

        m_view = glm::lookAt(m_pos, m_pos + m_forward, m_up);
        m_dirtyView = false;
    }
    void RecalcProjection() const
    {
        m_projection = glm::perspective(m_fovY, m_aspect, m_nearZ, m_farZ);
        m_dirtyProjection = false;
    }

public:
    void SetPerspective(float fovY, float aspect, float nearZ, float farZ)
    {
        m_fovY = fovY;
        m_aspect = aspect;
        m_nearZ = nearZ;
        m_farZ = farZ;
        m_dirtyProjection = true;
    }
    void LookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
    {
        m_pos = eye;
        glm::vec3 dir = glm::normalize(center - eye);
        m_pitch = glm::asin(dir.y);
        m_yaw = glm::atan(dir.x, dir.z);
        m_dirtyView = true;
    }
    void Translate(glm::vec3 delta)
    {
        m_pos += delta;
        m_dirtyView = true;
    }
    void Rotate(float yawDelta, float pitchDelta)
    {
        // accum yaw and then wrap
        m_yaw += yawDelta;
        {
            float fullTurn = glm::two_pi<float>();
            m_yaw = std::fmod(m_yaw, fullTurn);
            if (m_yaw < 0.0f)
                m_yaw += fullTurn;
        }

        // accum and clamp pitch
        m_pitch += pitchDelta;
        float limit = glm::half_pi<float>() * 0.99f;
        m_pitch = glm::clamp(m_pitch, -limit, limit);

        m_dirtyView = true;
    }

    const glm::mat4 &GetView() const
    {
        if (m_dirtyView)
            RecalcView();
        return m_view;
    }
    const glm::mat4 &GetProjection() const
    {
        if (m_dirtyProjection)
            RecalcProjection();
        return m_projection;
    }

    glm::vec3 GetForward() const
    {
        if (m_dirtyView)
            RecalcView();
        return m_forward;
    }
    glm::vec3 GetRight() const
    {
        if (m_dirtyView)
            RecalcView();
        return m_right;
    }
    glm::vec3 GetUp() const
    {
        if (m_dirtyView)
            RecalcView();
        return m_up;
    }

    float GetFovY() const { return m_fovY; }
    float GetNearZ() const { return m_nearZ; }
    float GetFarZ() const { return m_farZ; }

    glm::vec3 GetPosition() const { return m_pos; }
    glm::vec2 GetYawPitch() const { return {m_yaw, m_pitch}; }
};