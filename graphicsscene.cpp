#include "graphicsscene.h"
#include "calc-tools/basetool.h"
#include "calc-tools/figure.h"
#include <QGraphicsItem>
#include "robocamera.h"

QList <BaseTool *> tools;
BaseTool *currentTool;
LineFigure *poolLine;

GraphicsScene::GraphicsScene() :
    QGraphicsScene()
{
    axis = new AxisFigure();
    _showAxes = false;
}

void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    currentTool->drawOnMouseDoubleClick(this, event->scenePos());
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    updateScene();
    currentTool->drawOnMouseMove(this, event->scenePos());
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    currentTool->drawOnMousePress(this, event->scenePos());
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    currentTool->drawOnMouseRelease(this, event->scenePos());
}

void GraphicsScene::updateScene()
{
    QList<QGraphicsItem *> t = items();
    for (auto it = t.begin(); it != t.end(); it++) {
        QGraphicsScene::removeItem(dynamic_cast<QGraphicsItem *>(*it));
    }
    addItem(_screen);
    for (auto it = figures.begin(); it != figures.end(); it++)
        (dynamic_cast<Figure *>(*it))->draw(this);
    if (_showAxes) {
        axis->draw(this);
    }
    QString s("\n\n\n");
    for (auto it = containerCenters.begin(); it != containerCenters.end(); it++) {
        this->addRect(*it, (it == containerCenters.begin()) ? QColor(255, 0, 0) : QColor(0, 0, 255));

        //this->addLine(axis->getCenterPoint().x(), axis->getCenterPoint().y(), (*it).x(), (*it).y());
        //LineFigure line(QPointF(axis->getCenterPoint().x(), axis->getCenterPoint().y()), QPointF((*it).x(), (*it).y()));
        //s += line.getInfo() + "\n";
    }
    this->addText(s);
    if (poolLine != nullptr) {
        poolLine->draw(this);
    }
}

void GraphicsScene::clearScene()
{
    QList<QGraphicsItem *> t = items();
    for (auto it = t.begin(); it != t.end(); it++) {
        QGraphicsScene::removeItem(dynamic_cast<QGraphicsItem *>(*it));
    }
    containerCenters.clear();
    figures.clear();
}

void GraphicsScene::addFigure(Figure *figure)
{
    figures.append(figure);
}

void GraphicsScene::addScreen(QGraphicsPixmapItem *item)
{
    addItem(item);
    _screen = item;
}

void GraphicsScene::setShowAxes(bool value)
{
    _showAxes = value;
    updateScene();
}

void GraphicsScene::setAxesAngle(double angle)
{
    _axesAngle = angle;
}

void GraphicsScene::updateAxisCenter(QPointF point)
{
    axis->resetCenterPoint(point);
}

void GraphicsScene::setShowContainersCenters(bool value)
{
    _showContainersCenters = value;
}

VideoGraphicsScene::VideoGraphicsScene() : QGraphicsScene()
{
    _timer = nullptr;
    _screenItem = nullptr;
    _screenScene = nullptr;
    _mapScene = nullptr;
}

void VideoGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _mapScene->clearScene();
    _screenScene->updateScene();
    makeScreen();
}

void VideoGraphicsScene::makeScreen()
{
    _mainCamera->imageCapture();
    if (!_mainCamera->getLastSavedImage().isNull()) {
        _picOpacity = 1.0;
        scaleCoef = 0;
        currentTool->destroyProperties();
        if (_screenScene != nullptr) {
            _screenScene->clearScene();
            _screenScene->addScreen(_screen);
        }
        if (_mapScene != nullptr) {
            _mapScene->clearScene();
        }
        _pic = QPixmap::fromImage(_mainCamera->getLastSavedImage());
        _screen->setPixmap(_pic);
        startNewAnimation();
    } else {
        qDebug() << "Screen not found\n";
    }
}

void VideoGraphicsScene::setCamera(RoboCamera *camera)
{
    _mainCamera = camera;
}

void VideoGraphicsScene::addScreen(QGraphicsPixmapItem *item)
{
    _screen = item;
}

void VideoGraphicsScene::startNewAnimation()
{
    clearTimer();
    _timer = new QTimer();
    _timer->setInterval(100);
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateAnimationMakeScreen()));
    _timer->start();
}

void VideoGraphicsScene::updateAnimationMakeScreen()
{
    clearScreenItem();
    if (_picOpacity < 0.1) {
        clearTimer();
        return;
    }
    _picOpacity -= 0.1;
    QImage img(_pic.width(), _pic.height(), QImage::Format_ARGB32_Premultiplied);
    img.fill(QColor(255, 255, 255, _picOpacity*255));
    _screenItem = new QGraphicsPixmapItem(QPixmap::fromImage(img));
    addItem(_screenItem);
}

void VideoGraphicsScene::clearTimer()
{
    if (_timer != nullptr) {
        _timer->stop();
        delete _timer;
        _timer = nullptr;
    }
}

void VideoGraphicsScene::clearScreenItem()
{
    if (_screenItem != nullptr) {
        delete _screenItem;
        _screenItem = nullptr;
    }
}

void VideoGraphicsScene::addScreenScene(GraphicsScene *screenScene)
{
    _screenScene = screenScene;
}

void VideoGraphicsScene::addMapScene(MapGraphicsScene *mapScene)
{
    _mapScene = mapScene;
}

MapGraphicsScene::MapGraphicsScene() : QGraphicsScene()
{

}

void MapGraphicsScene::updateScene()
{
    QList<QGraphicsItem *> t = items();
    for (auto it = t.begin(); it != t.end(); it++) {
        QGraphicsScene::removeItem(dynamic_cast<QGraphicsItem *>(*it));
    }
    for (auto it = figures.begin(); it != figures.end(); it++)
        (dynamic_cast<Figure *>(*it))->draw(this);
    axis->draw(this);
    QString s("\n\n\n");
    double offset = 1.5;
    for (auto it = containerCenters.begin(); it != containerCenters.end(); it++) {
        this->addRect(*it, (it == containerCenters.begin()) ? QColor(255, 0, 0) : QColor(0, 0, 255));
        this->addLine(axis->getCenterPoint().x(), axis->getCenterPoint().y(), (*it).x() + offset, (*it).y() + offset);
        LineFigure line(QPointF(axis->getCenterPoint().x(), axis->getCenterPoint().y()), QPointF((*it).x() + offset, (*it).y() + offset));
        s += line.getInfo() + "\n";
    }
    this->addText(s);
}

void MapGraphicsScene::clearScene()
{
    QList<QGraphicsItem *> t = items();
    for (auto it = t.begin(); it != t.end(); it++) {
        QGraphicsScene::removeItem(dynamic_cast<QGraphicsItem *>(*it));
    }

    for (auto it = figures.begin(); it != figures.end(); it++)
        (dynamic_cast<Figure *>(*it))->~Figure();
}
