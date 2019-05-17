#include "application/MainWindow.h"
#include <QLayout>
#include <QLabel>
#include <QFileDialog>
#include <QPushButton>

void MainWindow::refreshModelSelection()
{
    modelSelection_->clear();

    for(auto model : renderer_->getModelManager().getModels())
    {
        modelSelection_->addItem(QString::fromStdString(model.getName()));
    }

    modelSelection_->setCurrentIndex(renderer_->getModelManager().getModels().size() - 1);
}

void MainWindow::loadObjFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select the obj file to load", "./",
                       "Obj File (*.obj)");

    if(filePath.size())
    {
        renderer_->getModelManager().loadNewMesh(filePath.toStdString());
    }

    refreshModelSelection();

}

void MainWindow::changeModelSelected(int index)
{
    if(index < 0) // no index selected
        return;

    renderer_->getModelManager().setSelectedModel(index);
}

MainWindow::MainWindow(QWindow* vulkanWindow):
    renderer_(reinterpret_cast<RendererWindow*>(vulkanWindow))
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
    connect(fullScreen, SIGNAL(clicked()), renderer_, SLOT(setFullscreen()));
    connect(modelSelection_, SIGNAL(currentIndexChanged(int)), this, SLOT(changeModelSelected(int)));

    setLayout(layout);
}



MainWindow::~MainWindow()
{

}
