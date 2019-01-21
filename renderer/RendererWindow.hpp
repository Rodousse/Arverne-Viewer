#ifndef RENDERER_WINDOW_HPP
#define RENDERER_WINDOW_HPP


#include "VulkanApplication.hpp"
#include <QVulkanWindowRenderer>
#include <QResizeEvent>
#include "renderer/camera/ArcBallCamera.hpp"

#define WIDTH_WINDOW 800
#define HEIGHT_WINDOW 600


class RendererWindow: public QWindow, public VulkanApplication
{
private:
    QVulkanWindow* window_;
	bool windowResized_ = false;
    bool initialized_ = false;

    const float ANGLE_INCREMENT_STEP = 0.03f;
    const float RADIUS_INCREMENT_STEP = 0.5f;
    Qt::MouseButton mouseButtonPressed_ = Qt::NoButton;
    QPoint mouseLastPosition_;
    ArcBallCamera camera_;


    // --------------------------- VULKANAPPLICATION VIRTUAL
    void initCore() override;
    void initWindow()override;
    void createSurface()override;
    void resizeWindow(int width, int height)override;
    void drawFrame()override;
    void mainLoop()override{}
    void run()override{}
    //--------------------------------------------



    // --------------------------- QWINDOW VIRTUAL
    void resizeEvent(QResizeEvent *e) override;

    //--------------------------------------------


    void processWheelEvent(const QWheelEvent *e);
    void processLeftMouseButtonEvent(const QMouseEvent *e);
    void processRightMouseButtonEvent(const QMouseEvent *e);
    void processMiddleMouseButtonEvent(const QMouseEvent *e);


public:
    RendererWindow();
    ~RendererWindow() override;

    // --------------------------- QWINDOW VIRTUAL
    void exposeEvent(QExposeEvent *) override;
    bool event(QEvent *e) override;
    //--------------------------------------------

    void initInstance(QVulkanInstance* instance);
    void setAngle(uint16_t angle);

};

#endif
