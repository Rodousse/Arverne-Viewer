#include "application/MainWindow.h"
#include "application/RendererWindow.h"
#include <plog/Log.h>
#include <QApplication>
#include <QLoggingCategory>
#include <QVulkanInstance>


int main(int argc, char* argv[])
{
    plog::init(plog::verbose, "./log.txt");
    QApplication a(argc, argv);

    const bool dbg = qEnvironmentVariableIntValue("QT_VK_DEBUG");

    QVulkanInstance instance;
    RendererWindow* vulkanWindow = new RendererWindow();

    if(dbg)
    {
        QLoggingCategory::setFilterRules(QStringLiteral("qt.vulkan=true"));

#ifndef Q_OS_ANDROID
        instance.setLayers(QByteArrayList() << "VK_LAYER_LUNARG_standard_validation");
#else
        instance.setLayers(QByteArrayList()
                           << "VK_LAYER_GOOGLE_threading"
                           << "VK_LAYER_LUNARG_parameter_validation"
                           << "VK_LAYER_LUNARG_object_tracker"
                           << "VK_LAYER_LUNARG_core_validation"
                           << "VK_LAYER_LUNARG_image"
                           << "VK_LAYER_LUNARG_swapchain"
                           << "VK_LAYER_GOOGLE_unique_objects");
#endif
    }

    vulkanWindow->initInstance(&instance);

    if(!instance.create())
    {
        PLOGE << "failed to create vulkan instance : " <<  instance.errorCode() << '\n';
    }

    vulkanWindow->setVulkanInstance(&instance);

    MainWindow mainWindow(vulkanWindow);
    mainWindow.resize(1280, 720);
    mainWindow.show();

    return a.exec();
}
