#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <QVector3D>

class Camera
{
protected:
    QVector3D position_;
    QVector3D center_; //Look at this point
    QVector3D up_; //Defines the Up vector in world coordinates
    float fov_;

public:




    Camera();
    virtual ~Camera();

    const QVector3D& getUp() const;
    void setUp(const QVector3D &up);
    float getFov() const;
    void setFov(float fov);
    const QVector3D& getPosition() const;
    virtual void setPosition(const QVector3D &position);
    const QVector3D& getCenter() const;
    virtual void setCenter(const QVector3D &center);
};

#endif // CAMERA_HPP
