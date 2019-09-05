#include "application/MainWindow.h"
#include <QLayout>
#include <QLabel>
#include <QFileDialog>
#include <QPushButton>

void MainWindow::refreshModelSelection()
{
    modelSelection_->clear();

    for(auto model : rendererWindow_->renderer().modelManager().getModels())
    {
        modelSelection_->addItem(QString::fromStdString(model.name()));
    }

    modelSelection_->setCurrentIndex(rendererWindow_->renderer().modelManager().getModels().size() - 1);
}

void MainWindow::loadObjFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select the obj file to load", "./",
                       "Obj File (*.obj)");

    if(filePath.size())
    {
        rendererWindow_->renderer().modelManager().loadNewMesh(filePath.toStdString());
        rendererWindow_->resetCamera();
    }

    refreshModelSelection();

}

void MainWindow::changeModelSelected(int index)
{
    if(index < 0) // no index selected
        return;

    rendererWindow_->renderer().modelManager().setSelectedModel(index);
    rendererWindow_->resetCamera();
}

MainWindow::MainWindow(QWindow* vulkanWindow):
    rendererWindow_(reinterpret_cast<RendererWindow*>(vulkanWindow))
{

    QGridLayout* layout = new QGridLayout(this);

    QWidget* wrapper = QWidget::createWindowContainer(vulkanWindow);
    wrapper->setFocusPolicy(Qt::StrongFocus);
    wrapper->setFocus();

    layout->addWidget(wrapper, 0, 0);
    layout->setColumnStretch(0, 3);
    layout->setRowStretch(0, 1);

    QGridLayout* optionLayout = new QGridLayout(this);

    QPushButton* fullScreen = new QPushButton("Activate Fullscreen", this);
    optionLayout->addWidget(fullScreen);

    QPushButton* browse = new QPushButton("Browse...", this);
    browse->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    optionLayout->addWidget(browse);

    modelSelection_ = new QComboBox(this);
    optionLayout->addWidget(modelSelection_);

    layout->addLayout(optionLayout, 0, 1, Qt::AlignCenter);
    layout->setColumnStretch(1, 1);

    connect(browse, SIGNAL(clicked()), this, SLOT(loadObjFile()));
    connect(fullScreen, SIGNAL(clicked()), rendererWindow_, SLOT(setFullscreen()));
    connect(modelSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(changeModelSelected(int)));

    setLayout(layout);
}



MainWindow::~MainWindow()
{

}
