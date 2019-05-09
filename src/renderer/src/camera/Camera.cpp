#include "renderer/camera/Camera.h"


namespace renderer
{

const glm::vec3& Camera::getPosition() const
{
    return position_;
}

void Camera::setPosition(const glm::vec3& position)
{
    position_ = position;
}

const glm::vec3& Camera::getCenter() const
{
    return center_;
}

void Camera::setCenter(const glm::vec3& center)
{
    center_ = center;
}

Camera::Camera():
    position_(-1.0f, 0.0f, 0.0f),
    center_(0.0f, 0.0f, 0.0f),
    up_(0.0f, 0.0f, -1.0f),
    fov_(45.0f)
{

}

Camera::~Camera()
{

}

const glm::vec3& Camera::getUp() const
{
    return up_;
}

void Camera::setUp(const glm::vec3& up)
{
    up_ = up;
}

float Camera::getFov() const
{
    return fov_;
}

void Camera::setFov(float fov)
{
    fov_ = fov;
}

}
