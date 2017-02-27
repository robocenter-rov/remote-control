#include "mainwindow.h"
#include <iostream>

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
    updateDepth();
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

void MainWindow::updateDepth()
{
    QList<QGraphicsItem *> t = _ui->mainView->scene()->items();
    for (auto it = t.begin(); it != t.end(); it++) {
        if (it == t.begin()) continue;
        _ui->mainView->scene()->removeItem(dynamic_cast<QGraphicsItem *>(*it));
    }
    qreal h = _ui->mainView->scene()->height()/15;
    qreal middle_y = _ui->mainView->scene()->height()/2;

    QString s;
    qreal height = _ui->mainView->scene()->height();

    for (qreal i = middle_y; i < height; i += h){
        _ui->mainView->scene()->addLine(0, i, 10, i);
        s.clear(); s.setNum(i);
        _ui->mainView->scene()->addText(s)->setPos(QPointF(15.0, i - 12));
    }

    for (qreal i = middle_y - h; i > 0; i -= h){
        _ui->mainView->scene()->addLine(0, i, 10, i);
        s.clear(); s.setNum(i);
        _ui->mainView->scene()->addText(s)->setPos(QPointF(15.0, i - 12));
    }
}
