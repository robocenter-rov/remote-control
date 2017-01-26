#include "calcwindow.h"
#include "ui_calcwindow.h"
#include <QFile>

CalcWindow::CalcWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(new Ui::CalcWindow)
{
    _ui->setupUi(this);
    _mainCamera = new RoboCamera(_ui->videoView, this, "mainCamera");
    loadQSS();
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
    if(event->type() == QEvent::Resize ) {
        _ui->videoView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        return true;
    }
    return false;
}
