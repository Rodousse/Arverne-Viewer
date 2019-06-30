#include "renderer/camera/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace renderer
{

const glm::vec3& Camera::getPosition() const
{
    return position_;
}

void Camera::setPosition(const glm::vec3& position)
{
    position_ = position;
    refresh();
}

const glm::vec3& Camera::getCenter() const
{
    return center_;
}

void Camera::setCenter(const glm::vec3& center)
{
    center_ = center;
    refresh();
}

void Camera::refresh()
{
    glm::vec3 direction = glm::normalize(center_ - position_);
    right_ = glm::normalize(-glm::cross(direction, upWorld_));
    up_ = glm::cross(direction, right_);
}

Camera::Camera():
    position_(-1.0f, 0.0f, 0.0f),
    center_(0.0f, 0.0f, 0.0f),
    upWorld_(0.0f, 0.0f, -1.0f),
    fov_(45.0f)
{
    refresh();
}

Camera::~Camera()
{

}

void Camera::moveRight(float amplitude)
{
    auto translation = right_ * amplitude;
    position_ += translation;
    center_ += translation;
}

void Camera::moveUp(float amplitude)
{
    auto translation = up_ * amplitude;
    position_ += translation;
    center_ += translation;
}

const glm::vec3& Camera::getUp() const
{
    return upWorld_;
}

void Camera::setUp(const glm::vec3& up)
{
    upWorld_ = glm::normalize(up);
    refresh();
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
