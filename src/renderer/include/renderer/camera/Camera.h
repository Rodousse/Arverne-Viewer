#pragma once

#include <glm/vec3.hpp>

namespace renderer
{

class Camera
{
protected:
    glm::vec3 position_;
    glm::vec3 center_; //Look at this point
    glm::vec3 upWorld_; //Defines the Up vector in world coordinates
    glm::vec3 right_;
    glm::vec3 up_;
    float fov_;

    virtual void refresh();

public:
    Camera();
    virtual ~Camera();

    void moveRight(float amplitude);
    void moveUp(float amplitude);

    const glm::vec3& getUp() const;
    void setUp(const glm::vec3& up);
    float getFov() const;
    void setFov(float fov);
    const glm::vec3& getPosition() const;
    virtual void setPosition(const glm::vec3& position);
    const glm::vec3& getCenter() const;
    virtual void setCenter(const glm::vec3& center);
};

}
