#include "mainwindow.h"
#include <iostream>

QDataStream& operator<<(QDataStream &ds, const Cmd::AxesValue &msg)
{
    ds << msg.axis0 << " " << msg.axis1 << " " << msg.axis2 << " " << msg.axis3 << " " << msg.axis4;
    return ds;
}

void MainWindow::loadQSS()
{
    QFile styleF("../remote-control/qss/appstyles.css");
    if (!styleF.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Warning: application styles not found";
    }
    this->setStyleSheet(styleF.readAll());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _ui(new Ui::MainWindow),
      _communicator(new Communicator())
{
    _ui->setupUi(this);
    cameraInit();
    loadQSS();
}

MainWindow::~MainWindow()
{
    delete _ui;
    delete _communicator;
}

void MainWindow::cameraInit()
{
    _mainCamera = new RoboCamera(_ui->mainView, this, "mainCamera");
}

bool MainWindow::eventFilter(QObject *, QEvent *event)
{
    if(event->type() == QEvent::Resize ) {
        _ui->mainView->fitInView(_mainCamera->getScene()->sceneRect(), Qt::KeepAspectRatio);
        return true;
    }
    return false;
}
