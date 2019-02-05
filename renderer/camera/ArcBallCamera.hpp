#ifndef ARCBALLCAMERA_HPP
#define ARCBALLCAMERA_HPP


#include "Camera.hpp"

namespace renderer
{

class ArcBallCamera : public Camera
{
private:
    float radius_;
    float phi_;
    float theta_;

    void computePosition();
    void computePolar();

public:
    ArcBallCamera();
    virtual ~ArcBallCamera() override;

    virtual void setPosition(const QVector3D &position) override;
    virtual void setCenter(const QVector3D &position) override;

    void incrementPhi(float phiStep);
    void incrementTheta(float thetaStep);
    void incrementRadius(float radiusStep);

    float getRadius() const;
    void setRadius(float radius);
    float getPhi() const;
    void setPhi(float phi);
    float getTheta() const;
    void setTheta(float theta);
};

}
#endif // ARCBALLCAMERA_HPP
