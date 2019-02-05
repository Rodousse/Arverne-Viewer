#-------------------------------------------------
#
# Project created by QtCreator 2018-11-18T16:43:15
#
#-------------------------------------------------

QT       += widgets#core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PBRViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        ui/mainwindow.cpp \
    renderer/DebugMessenger.cpp \
    renderer/PhysicalDeviceProperties.cpp \
    renderer/PhysicalDeviceProvider.cpp \
    renderer/Swapchain.cpp \
    renderer/Texture2D.cpp \
    renderer/Vertex.cpp \
    renderer/VkElement.cpp \
    renderer/VulkanApplication.cpp \
    renderer/VulkanCore.cpp \
    renderer/VulkanUtils.cpp \
    renderer/RendererWindow.cpp \
    renderer/camera/Camera.cpp \
    renderer/camera/ArcBallCamera.cpp \
    loader/Loader.cpp \
    loader/ObjLoader.cpp \
    renderer/Mesh.cpp


HEADERS += \
        ui/mainwindow.h \
    renderer/DebugMessenger.hpp \
    renderer/PhysicalDeviceProperties.hpp \
    renderer/PhysicalDeviceProvider.hpp \
    renderer/Swapchain.hpp \
    renderer/Texture2D.hpp \
    renderer/VkElement.hpp \
    renderer/VulkanApplication.hpp \
    renderer/VulkanCore.hpp \
    renderer/VulkanUtils.hpp \
    renderer/Vertex.hpp \
    renderer/RendererWindow.hpp \
    renderer/camera/Camera.hpp \
    renderer/camera/ArcBallCamera.hpp \
    loader/Loader.hpp \
    loader/ObjLoader.hpp \
    renderer/Mesh.hpp \
    third_party/tinyobjloader/tiny_obj_loader.h


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    shaders/fragment.frag \
    shaders/vertex.vert \
    Todo

RESOURCES += \
    pbrviewer.qrc


PATH_TO_VULKAN = $$(VULKAN_SDK)

win32: LIBS += -L$${PATH_TO_VULKAN}/Lib/ -lvulkan-1

INCLUDEPATH += $${PATH_TO_VULKAN}/Include
DEPENDPATH += $${PATH_TO_VULKAN}/Include

win32:!win32-g++: PRE_TARGETDEPS += $${PATH_TO_VULKAN}/Lib/vulkan-1.lib
else:win32-g++: PRE_TARGETDEPS += $${PATH_TO_VULKAN}/Lib/libvulkan-1.a
