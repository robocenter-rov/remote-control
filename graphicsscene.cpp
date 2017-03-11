#include "graphicsscene.h"
#include "basetool.h"
#include "figure.h"
#include <QGraphicsItem>
#include "robocamera.h"

QList <BaseTool *> tools;
BaseTool *currentTool;

GraphicsScene::GraphicsScene() :
    QGraphicsScene()
{

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
    for (auto it = _figures.begin(); it != _figures.end(); it++)
        (dynamic_cast<Figure *>(*it))->draw(this);
}

void GraphicsScene::addFigure(Figure *figure)
{
    _figures.append(figure);
}

void GraphicsScene::addScreen(QGraphicsPixmapItem *item)
{
    addItem(item);
    _screen = item;
}

VideoGraphicsScene::VideoGraphicsScene() : QGraphicsScene()
{

}

void VideoGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    makeScreen();
}

void VideoGraphicsScene::makeScreen()
{
    _mainCamera->imageCapture();
    if (!_mainCamera->getLastSavedImage().isNull()) {
        _screen->setPixmap(QPixmap::fromImage(_mainCamera->getLastSavedImage()));
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
