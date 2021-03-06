#pragma once

#include "renderer/camera/Camera.h"

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

    virtual void setPosition(const glm::vec3& position) override;
    virtual void setCenter(const glm::vec3& position) override;

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
