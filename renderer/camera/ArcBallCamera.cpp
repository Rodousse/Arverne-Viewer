#include "ArcBallCamera.hpp"
#include <QtMath>


namespace renderer
{



ArcBallCamera::ArcBallCamera():
    Camera(),
    phi_(3*M_PI/4.0),
    theta_(M_PI),
    radius_(2.0f)
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
    phi_ = std::max(std::min(static_cast<float>(M_PI-0.01), phi), 0.01f);
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

void ArcBallCamera::setCenter(const QVector3D &center)
{
    Camera::setCenter(center);
    computePosition();
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::setPosition(const QVector3D &position)
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
    position_.setX(radius_ * sin(phi_) * cos(theta_));
    position_.setY(radius_ * sin(phi_) * sin(theta_));
    position_.setZ(radius_ * cos(phi_));

    position_ += center_;
}

//------------------------------------------------------------------------------------------------------

void ArcBallCamera::computePolar()
{
    radius_ = sqrt(pow(position_.x(), 2)+pow(position_.y(), 2)+pow(position_.z(), 2));
    theta_ = atan(position_.y()/position_.x());
    phi_ = atan((pow(position_.x(), 2)+pow(position_.y(), 2))/position_.z());
}


}


