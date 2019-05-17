#pragma once

#include "renderer/VulkanApplication.h"
#include <QVulkanWindowRenderer>
#include <QResizeEvent>
#include <QWindow>
#include "renderer/camera/ArcBallCamera.h"
#include "application/ModelManager.h"

#define WIDTH_WINDOW 800
#define HEIGHT_WINDOW 600


class RendererWindow: public QWindow, public renderer::VulkanApplication
{
    Q_OBJECT
private:
    QVulkanWindow* window_;
    bool windowResized_ = false;
    bool initialized_ = false;
    bool isFullscreen = false;

    const float ANGLE_INCREMENT_STEP = 0.01f;
    const float RADIUS_INCREMENT_STEP = 0.5f;
    Qt::MouseButton mouseButtonPressed_ = Qt::NoButton;
    QPoint mouseLastPosition_;
    renderer::ArcBallCamera camera_;
    ModelManager modelManager_;

    // --------------------------- VULKANAPPLICATION VIRTUAL
    void initCore() override;
    void initWindow()override;
    void createSurface()override;
    void resizeWindow(int width, int height)override;
    void drawFrame()override;
    void mainLoop()override {}
    void run()override {}
    //--------------------------------------------

    // --------------------------- QWINDOW VIRTUAL
    void resizeEvent(QResizeEvent* e) override;
    //--------------------------------------------

    void processWheelEvent(const QWheelEvent* e);
    void processLeftMouseButtonEvent(const QMouseEvent* e);
    void processRightMouseButtonEvent(const QMouseEvent* e);
    void processMiddleMouseButtonEvent(const QMouseEvent* e);

public:
    RendererWindow();
    ~RendererWindow() override;

    // --------------------------- QWINDOW VIRTUAL
    void exposeEvent(QExposeEvent*) override;
    bool event(QEvent* e) override;
    //--------------------------------------------

    void loadNewMesh(const std::string& path);
    ModelManager& getModelManager();
    const ModelManager& getModelManager()const;
    void initInstance(QVulkanInstance* instance);
    void setAngle(uint16_t angle);

public slots:
    void setFullscreen();
};
