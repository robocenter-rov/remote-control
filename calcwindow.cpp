#include "calcwindow.h"
#include "ui_calcwindow.h"
#include <QFile>

#include "graphicsscene.h"
#include "basetool.h"

CalcWindow::CalcWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CalcWindow)
{
    _ui->setupUi(this);
    _videoScene = new VideoGraphicsScene();
    _videoScene->addScreen(&_screen);

    _mainCamera = new RoboCamera(_ui->videoView, this, "mainCamera", _videoScene);
    _videoScene->setCamera(_mainCamera);

    _ui->videoView->setScene(_videoScene);
    _ui->videoView->viewport()->installEventFilter(this);
    _ui->videoView->show();

    _screenScene = new GraphicsScene();
    _screenScene->addScreen(&_screen);
    _ui->screensView->setScene(_screenScene);
    _ui->screensView->viewport()->installEventFilter(this);
    _ui->screensView->show();

    loadQSS();
    generateTools();
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
        return true;
    }
    return false;
}

void CalcWindow::generateTools()
{
    currentTool = new LineTool(_ui->toolsWidget);
    tools.append(new OptionTool(_ui->toolsWidget));
    tools.append(currentTool);
    tools.append(new RectTool(_ui->toolsWidget));
    tools.append(new SelectTool(_ui->toolsWidget));
    int i = 0;
    for (auto it = tools.begin(); it != tools.end(); it++)
        (*it)->setLevel(i++);
}
