#pragma once

#include <QMainWindow>
#include <QVulkanWindow>
#include <QSlider>
#include "application/RendererWindow.h"
#include <QComboBox>

class MainWindow : public QWidget
{
    Q_OBJECT
private:
    RendererWindow* rendererWindow_;
    QComboBox* modelSelection_;

    void refreshModelSelection();

protected slots:
    void loadObjFile();
    void changeModelSelected(int index);


public:
    MainWindow(QWindow* vulkanWindow);
    ~MainWindow();
};
