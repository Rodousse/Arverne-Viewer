#include "application/RendererWindow.h"
#include <time.h>
#include <defines.h>



RendererWindow::RendererWindow():
    modelManager_(&vkCore_)
{
    setSurfaceType(VulkanSurface);
    vkCore_.setCamera(camera_);
}


RendererWindow::~RendererWindow()
{
    setName("Arverne Viewer");
}

void RendererWindow::initCore()
{
    //Adding required extensions for the application
    std::vector<const char*> extensions;
    //Strictly needed by qt
    extensions.push_back("VK_KHR_surface");
#ifdef UNIX_
    extensions.push_back("VK_KHR_xcb_surface");
#elif WIN32_
    extensions.push_back("VK_KHR_win32_surface");
#endif
    extensions.push_back("VK_EXT_debug_report"); // enable debug as qt log
    vkCore_.addRequiredExtensions(extensions.data(), static_cast<uint32_t>(extensions.size()));


    //Set which device features are needed
    VkPhysicalDeviceFeatures neededFeatures = {};
    neededFeatures.samplerAnisotropy = VK_TRUE;
    neededFeatures.geometryShader = VK_TRUE;
    neededFeatures.sampleRateShading = VK_TRUE;

    vkCore_.setPhysicalDeviceFeaturesRequired(neededFeatures);

    vkCore_.createInstance();
}

void RendererWindow::initWindow()
{
    createSurface();
    vkCore_.resizeExtent(size().width(), size().height());
    vkCore_.initVulkan();
    drawFrame();
}


void RendererWindow::initInstance(QVulkanInstance* instance)
{
    initCore();
    instance->setVkInstance(vkCore_.getInstance());
}


void RendererWindow::createSurface()
{
    VkSurfaceKHR surface;
    surface = vulkanInstance()->surfaceForWindow(this);
    vkCore_.setSurface(surface);
}

void RendererWindow::resizeWindow(int width, int height)
{
    windowResized_ = true;

    if(width != 0 && height != 0)
    {
        vkCore_.resizeExtent(width, height);
    }
}

void RendererWindow::drawFrame()
{
    //If the surface on which we are drawing on isn't the same as the one provided by the window, we stop drawing
    if(vulkanInstance()->surfaceForWindow(this))
    {
        vkCore_.drawFrame();
        requestUpdate();
    }
}

void RendererWindow::resizeEvent(QResizeEvent* e)
{
    QWindow::resizeEvent(e);

    if(e->size() != e->oldSize())
    {
        resizeWindow(e->size().width(), e->size().height());
    }
}

void RendererWindow::processWheelEvent(const QWheelEvent* e)
{
    camera_.incrementRadius(-e->angleDelta().y() * RADIUS_INCREMENT_STEP * 1.0f /
                            120.0f); //We divide by 120 because of the wheel step of the average mouse beeing 120 units
}

void RendererWindow::processLeftMouseButtonEvent(const QMouseEvent* e)
{
    QPointF delta = QPointF(e->pos() - mouseLastPosition_) * ANGLE_INCREMENT_STEP;
    camera_.incrementTheta(delta.x());
    camera_.incrementPhi(delta.y());
    mouseLastPosition_ = e->pos();
}

void RendererWindow::processRightMouseButtonEvent(const QMouseEvent* e)
{

}

void RendererWindow::processMiddleMouseButtonEvent(const QMouseEvent* e)
{

}


void RendererWindow::exposeEvent(QExposeEvent*)
{
    if(isExposed())
    {
        if(!initialized_)
        {
            initialized_ = true;
            initWindow();
            drawFrame();
        }
    }
}

bool RendererWindow::event(QEvent* e)
{
    bool cameraUpdate = false; //Camera need to be updated

    if(e->type() == QEvent::UpdateRequest)
    {
        drawFrame();
    }

    switch(e->type())
    {
        case QEvent::MouseButtonPress:
            if(mouseButtonPressed_ == Qt::NoButton)
            {
                mouseButtonPressed_ = static_cast<QMouseEvent*>(e)->button();
                mouseLastPosition_ =  static_cast<QMouseEvent*>(e)->pos();
                cameraUpdate = true;
            }

            break;

        case QEvent::MouseMove:
            switch(mouseButtonPressed_)
            {
                case Qt::LeftButton:
                    processLeftMouseButtonEvent(static_cast<QMouseEvent*>(e));
                    break;

                case Qt::RightButton:
                    processRightMouseButtonEvent(static_cast<QMouseEvent*>(e));
                    break;

                case Qt::MiddleButton:
                    processMiddleMouseButtonEvent(static_cast<QMouseEvent*>(e));
                    break;

                default:
                    break;
            }

            cameraUpdate = true;
            break;

        case QEvent::MouseButtonRelease:
            cameraUpdate = true;

            if(mouseButtonPressed_ == static_cast<QMouseEvent*>(e)->button())
            {
                mouseButtonPressed_ = Qt::NoButton;
            }

            break;

        case QEvent::KeyRelease:
            if(static_cast<QKeyEvent*>(e)->key() == Qt::Key_Escape)
                if(this->visibility() == QWindow::FullScreen)
                {
                    this->showNormal();
                }

            break;
    }

    if(e->type() == QEvent::Wheel)
    {
        cameraUpdate = true;
        processWheelEvent(static_cast<const QWheelEvent*>(e));
    }

    if(cameraUpdate)
    {
        vkCore_.setCamera(camera_);
    }

    return QWindow::event(e);
}

void RendererWindow::loadNewMesh(const std::string& path)
{
    modelManager_.loadNewMesh(path);
}

ModelManager& RendererWindow::getModelManager()
{
    return modelManager_;
}

const ModelManager& RendererWindow::getModelManager() const
{
    return modelManager_;
}



void RendererWindow::setFullscreen()
{
    this->showFullScreen();
}


