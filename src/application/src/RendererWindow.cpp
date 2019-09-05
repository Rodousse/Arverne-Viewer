#include "application/RendererWindow.h"
#include <qcoreapplication.h>
#include <time.h>
#include <defines.h>



RendererWindow::RendererWindow():
    renderer_()
{
    QWindow::setSurfaceType(VulkanSurface);
    renderer_.setCamera(camera_);
}


renderer::VulkanApplication& RendererWindow::renderer()
{
    return renderer_;
}

void RendererWindow::resizeEvent(QResizeEvent* e)
{
    QWindow::resizeEvent(e);

    if(e->size() != e->oldSize())
    {
        renderer_.resizeWindow(e->size().width(), e->size().height());
    }
}

void RendererWindow::processWheelEvent(const QWheelEvent* e)
{
    //We divide by 120 because of the wheel step of the average mouse beeing 120 units
    float radiusIncrement = -e->angleDelta().y() / 120.0f;
    radiusIncrement *= RADIUS_INCREMENT_STEP;
    radiusIncrement *= camera_.getRadius();
    camera_.incrementRadius(radiusIncrement);
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
    QPointF delta = QPointF(e->pos() - mouseLastPosition_) * MOVE_INCREMENT_STEP;
    delta *= camera_.getRadius();
    camera_.moveRight(delta.x());
    camera_.moveUp(-delta.y());
    mouseLastPosition_ = e->pos();
}


void RendererWindow::exposeEvent(QExposeEvent*)
{
    if(isExposed())
    {
        if(!initialized_)
        {
            initialized_ = true;
            renderer_.create(QVulkanInstance::surfaceForWindow(this));
            renderer_.resizeWindow(width(), height());
            requestUpdate();
        }
    }
}

bool RendererWindow::event(QEvent* e)
{
    bool cameraUpdate = false; //Camera need to be updated

    if(e->type() == QEvent::UpdateRequest)
    {
        renderer_.drawFrame();
        requestUpdate();
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

    if(e->type() == QEvent::PlatformSurface
       && static_cast<const QPlatformSurfaceEvent*>(e)->surfaceEventType() ==
       QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)// Surface to be destroyed
    {
        QCoreApplication::processEvents();// We assume this is the end of the application and process it
        renderer_.cleanup(); // Cleanup the renderer
    }

    if(cameraUpdate)
    {
        renderer_.setCamera(camera_);
    }

    return QWindow::event(e);
}



void RendererWindow::resetCamera()
{
    camera_.setCenter(glm::vec3(0.0));
    camera_.setRadius(5.0f);
    renderer_.setCamera(camera_);
}



void RendererWindow::setFullscreen()
{
    this->showFullScreen();
}


