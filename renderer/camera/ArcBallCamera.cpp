#include "ArcBallCamera.hpp"
#include <glm/gtc/constants.hpp>
#include <algorithm>


namespace renderer
{

const float M_PI = glm::pi<float>();


ArcBallCamera::ArcBallCamera():
    Camera(),
    radius_(2.0f),
    phi_(3.0f*M_PI/4.0f),
    theta_(M_PI)
{
    computePosition();
}

//------------------------------------------------------------------------------------------------------

ArcBallCamera::~ArcBallCamera()
{

}

//------------------------------------------------------------------------------------------------------

float ArcBallCamera::getRadius() const
{
    return radius_;
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::setRadius(float radius)
{
    radius_ = radius;
    computePosition();
}

//------------------------------------------------------------------------------------------------------

float ArcBallCamera::getPhi() const
{
    return phi_;
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::setPhi(float phi)
{
    phi_ = std::max(std::min(static_cast<float>(M_PI-0.01f), phi), 0.01f);
    computePosition();
}

//------------------------------------------------------------------------------------------------------

float ArcBallCamera::getTheta() const
{
    return theta_;
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::setTheta(float theta)
{
    if(theta > 2*M_PI)
    {
        theta_ = theta - 2*M_PI;
    }
    theta_ = theta;
    computePosition();
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::setCenter(const glm::vec3 &center)
{
    Camera::setCenter(center);
    computePosition();
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::setPosition(const glm::vec3 &position)
{
    Camera::setPosition(position);
    computePolar();
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::incrementPhi(float phiStep)
{
    setPhi(phi_ - phiStep);
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::incrementTheta(float thetaStep)
{
    setTheta(theta_ + thetaStep);
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::incrementRadius(float radiusStep)
{
    setRadius(radius_ + radiusStep);
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::computePosition()
{
    position_.x = radius_ * sin(phi_) * cos(theta_);
    position_.y =radius_ * sin(phi_) * sin(theta_);
    position_.z =radius_ * cos(phi_);

    position_ += center_;
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::computePolar()
{
    radius_ = sqrt(pow(position_.x, 2)+pow(position_.y, 2)+pow(position_.z, 2));
    theta_ = atan(position_.y/position_.x);
    phi_ = atan((pow(position_.x, 2)+pow(position_.y, 2))/position_.z);
}


}


