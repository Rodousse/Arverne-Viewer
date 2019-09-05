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

    instance.setVkInstance(vulkanWindow->renderer().vkInstance());

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
