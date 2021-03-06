#include "calcwindow.h"
#include "ui_calcwindow.h"
#include <QFile>

#include "graphicsscene.h"
#include "calc-tools/basetool.h"

CalcWindow::CalcWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CalcWindow)
{
    _ui->setupUi(this);
    _videoScene = new VideoGraphicsScene();
    _videoScene->addScreen(&_screen);

    _mainCamera = new RoboCamera(0, _ui->videoView, this, _videoScene);
    _videoScene->setCamera(_mainCamera);

    _ui->videoView->setScene(_videoScene);
    _ui->videoView->viewport()->installEventFilter(this);
    _ui->videoView->show();

    _screenScene = new GraphicsScene();
    _screenScene->addScreen(&_screen);
    _ui->screensView->setScene(_screenScene);
    _ui->screensView->viewport()->installEventFilter(this);
    _ui->screensView->show();

    _videoScene->addScreenScene(_screenScene);

    _mapScene = new MapGraphicsScene();
    _ui->mapView->setScene(_mapScene);
    _ui->mapView->viewport()->installEventFilter(this);
    _ui->mapView->show();

    _videoScene->addMapScene(_mapScene);

    loadQSS();
    generateTools();
    connect(_ui->screensTab, SIGNAL(currentChanged(int)), this, SLOT(changedTabIndex()));
    connect(_ui->axesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showAxes(int)));
    _clearButton = new QPushButton(_ui->toolsWidget);
    initClearButton();
}

CalcWindow::~CalcWindow()
{
    delete _ui;
}

void CalcWindow::loadQSS()
{
    QFile styleF("../remote-control/qss/appstyles.css");
    if (!styleF.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Warning: application styles not found";
    }
    this->setStyleSheet(styleF.readAll());
}

bool CalcWindow::eventFilter(QObject *, QEvent *event)
{
    if(event->type() == QEvent::Resize) {
        _ui->videoView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        _ui->screensView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        _ui->mapView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        return true;
    }
    return false;
}

void CalcWindow::generateTools()
{
    currentTool = new LineTool(_ui->toolsWidget);
    _tools.append(currentTool);
    _tools.append(new SelectTool(_ui->toolsWidget));
    _tools.append(new PoolLineTool(_ui->toolsWidget));
    _tools.append(new ReplaceAxisTool(_ui->toolsWidget));
    _tools.append(new ContainersCenterTool(_ui->toolsWidget));
}

void CalcWindow::changedTabIndex()
{
    int idx = _ui->screensTab->currentIndex();
    if (idx == 2) {
        _mapScene->setSceneRect(_ui->videoView->sceneRect());
        _mapScene->updateScene();
    }
}

void CalcWindow::initClearButton() {
    _clearButton = new QPushButton(_ui->toolsWidget);
    _clearButton->setText("Clear");
    _clearButton->setIconSize(QSize(30, 30));
    connect(_clearButton, SIGNAL(clicked(bool)), this, SLOT(clearScreenView()));
}

void CalcWindow::clearScreenView()
{
    _mapScene->clearScene();
    _screenScene->updateScene();
}

void CalcWindow::showAxes(int value)
{
    _screenScene->setShowAxes(value);
}
