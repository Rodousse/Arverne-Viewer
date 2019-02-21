#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVulkanWindow>
#include <QSlider>
#include "application/RendererWindow.hpp"
#include <QComboBox>

class MainWindow : public QWidget
{
    Q_OBJECT
private:
    RendererWindow *renderer_;
    QComboBox *modelSelection_;

    void refreshModelSelection();

protected slots:
    void loadObjFile();
    void changeModelSelected(int index);


public:
    MainWindow(QWindow *vulkanWindow);
    ~MainWindow();
};

#endif // MAINWINDOW_H
