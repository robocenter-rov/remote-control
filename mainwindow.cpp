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
    clearScene();
    QGraphicsScene *scene = _ui->mainView->scene();
    qreal h = scene->height()/15;
    qreal middle_y = scene->height()/2;
    qreal height = scene->height();

    qreal currentDepth = 63.0;

    qreal nearestUp = int(currentDepth) - (int(currentDepth) == currentDepth),
          nearestDown = int(currentDepth) + 1;
    qreal p = currentDepth - int(currentDepth), q = 1 - p;

    if (p == 0) p = 1;
    QString s;

    for (qreal i = middle_y + q*h; i < height; i += h){
        scene->addLine(1, i, 8, i);
        scene->addLine(1, i - h/2, 4, i - h/2);
        s.clear(); s.setNum(nearestDown++);
        scene->addText(s)->setPos(QPointF(20.0, i - 12));
    }

    for (qreal i = middle_y - p*h; i > 10; i -= h){
        scene->addLine(1, i, 8, i);
        scene->addLine(1, i + h/2, 4, i + h/2);
        s.clear(); s.setNum(nearestUp--);
        scene->addText(s)->setPos(QPointF(20.0, i - 12));
    }

    QVector<QPointF> points;
    points << QPointF(1, middle_y - 4) << QPointF(20, middle_y) << QPointF(1, middle_y + 4);
    QPolygonF pointer(points);
    scene->addPolygon(pointer, QPen(), QBrush(QColor(127, 0, 0)));
    s.clear(); s.setNum(currentDepth); s += " cm";
    scene->addText(s, QFont("Times", 10, QFont::Bold))->setPos(QPointF(36.0, middle_y - 12));
}

void MainWindow::clearScene()
{
    QList<QGraphicsItem *> t = _ui->mainView->scene()->items();
    for (auto it = t.begin(); it != t.end(); it++) {
        if (it == t.begin()) continue;
        _ui->mainView->scene()->removeItem(dynamic_cast<QGraphicsItem *>(*it));
    }
}
