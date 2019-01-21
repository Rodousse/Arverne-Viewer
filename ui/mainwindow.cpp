#include "mainwindow.h"
#include <QLayout>
#include <QLabel>

MainWindow::MainWindow(QWindow *vulkanWindow)
{
    QGridLayout *layout = new QGridLayout(this);

    QWidget *wrapper = QWidget::createWindowContainer(vulkanWindow);
    wrapper->setFocusPolicy(Qt::StrongFocus);
    wrapper->setFocus();

    layout->addWidget(wrapper, 0, 0);
    layout->setColumnStretch(0, 3);
    layout->setRowStretch(0, 1);


    QGridLayout *optionLayout = new QGridLayout(this);


    QSlider *angleSlider = new QSlider(Qt::Orientation::Horizontal, this);
    angleSlider->setMinimum(0);
    angleSlider->setMaximum(360);
    optionLayout->addWidget(angleSlider);


    layout->addLayout(optionLayout, 0, 1, Qt::AlignCenter);
    layout->setColumnStretch(1, 1);


    setLayout(layout);
}

MainWindow::~MainWindow()
{

}
