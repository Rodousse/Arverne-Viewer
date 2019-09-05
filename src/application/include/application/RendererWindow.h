#pragma once

#include "renderer/VulkanApplication.h"
#include <QVulkanInstance>
#include <QResizeEvent>
#include <QWindow>
#include "renderer/camera/ArcBallCamera.h"

#define WIDTH_WINDOW 800
#define HEIGHT_WINDOW 600


class RendererWindow: public QWindow
{
    Q_OBJECT
private:
    renderer::VulkanApplication renderer_;
    bool initialized_ = false;
    bool isFullscreen = false;

    static constexpr float MOVE_INCREMENT_STEP = 0.001f;
    static constexpr float ANGLE_INCREMENT_STEP = 0.01f;
    static constexpr float RADIUS_INCREMENT_STEP = 0.1f;
    Qt::MouseButton mouseButtonPressed_ = Qt::NoButton;
    QPoint mouseLastPosition_;
    renderer::ArcBallCamera camera_;

    // --------------------------- QWINDOW VIRTUAL
    void resizeEvent(QResizeEvent* e) override;
    void exposeEvent(QExposeEvent*) override;
    //--------------------------------------------

    void processWheelEvent(const QWheelEvent* e);
    void processLeftMouseButtonEvent(const QMouseEvent* e);
    void processRightMouseButtonEvent(const QMouseEvent* e);
    void processMiddleMouseButtonEvent(const QMouseEvent* e);

public:
    RendererWindow();
    virtual ~RendererWindow() = default;

    // --------------------------- QWINDOW VIRTUAL
    bool event(QEvent* e) override;
    //--------------------------------------------

    void resetCamera();
    void setAngle(uint16_t angle);

    renderer::VulkanApplication& renderer();

public slots:
    void setFullscreen();
};
