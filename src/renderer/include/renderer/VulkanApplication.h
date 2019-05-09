#pragma once

#include "VulkanCore.h"
#include <string>
#include <memory>

namespace renderer
{

class VulkanApplication
{
protected:
    VulkanCore vkCore_;
    std::string name_;

    virtual void initCore() = 0;
    virtual void initWindow() = 0;
    virtual void createSurface() = 0;
    virtual void resizeWindow(int width, int height) = 0; //TODO
    virtual void drawFrame() = 0;
    virtual void mainLoop() = 0;

public:
    VulkanApplication();
    virtual ~VulkanApplication();

    void setName(const std::string& name);
    const std::string& getName()const;

    virtual void run() = 0;
};

}
