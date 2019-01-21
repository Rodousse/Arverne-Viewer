#include "Camera.hpp"




const QVector3D& Camera::getPosition() const
{
    return position_;
}

//------------------------------------------------------------------------------------------------------

void Camera::setPosition(const QVector3D &position)
{
    position_ = position;
}

//------------------------------------------------------------------------------------------------------

const QVector3D &Camera::getCenter() const
{
    return center_;
}

//------------------------------------------------------------------------------------------------------

void Camera::setCenter(const QVector3D &center)
{
    center_ = center;
}

//------------------------------------------------------------------------------------------------------

Camera::Camera():
    position_(-1.0f,0.0f,0.0f),
    center_(0.0f, 0.0f, 0.0f),
    up_(0.0f,0.0f,1.0f),
    fov_(45.0f)
{

}

//------------------------------------------------------------------------------------------------------

Camera::~Camera()
{

}

//------------------------------------------------------------------------------------------------------

const QVector3D& Camera::getUp() const
{
    return up_;
}

//------------------------------------------------------------------------------------------------------

void Camera::setUp(const QVector3D &up)
{
    up_ = up;
}

//------------------------------------------------------------------------------------------------------

float Camera::getFov() const
{
    return fov_;
}

//------------------------------------------------------------------------------------------------------

void Camera::setFov(float fov)
{
    fov_ = fov;
}