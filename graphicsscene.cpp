#include "graphicsscene.h"
#include "basetool.h"
#include "figure.h"
#include <QGraphicsItem>

QList <BaseTool *> tools;
BaseTool *currentTool;

GraphicsScene::GraphicsScene() :
    QGraphicsScene()
{
    _text = nullptr;
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
    if (_text != nullptr)
        addText(_text);
}

void GraphicsScene::addFigure(LineFigure *line)
{
    _figures.append(line);
}

void GraphicsScene::addScreen(QGraphicsPixmapItem *item)
{
    addItem(item);
    _screen = item;
}

void GraphicsScene::addTextFigure(QString text)
{
    //addText(text);
    _text = text;
}
