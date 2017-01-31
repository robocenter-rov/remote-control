#include "graphicsscene.h"
#include "basetools.h"

QList <BaseTool *> tools;
BaseTool *currentTool;

void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    currentTool->drawOnMouseDoubleClick(this, event->scenePos());
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
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
