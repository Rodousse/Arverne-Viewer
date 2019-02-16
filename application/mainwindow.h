#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVulkanWindow>
#include <QSlider>

class MainWindow : public QWidget
{
    Q_OBJECT
private:


public:
    MainWindow(QWindow *vulkanWindow);
    ~MainWindow();
};

#endif // MAINWINDOW_H
