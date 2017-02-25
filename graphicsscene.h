#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QObject>

class Figure;
class LineFigure;
class SelectTool;

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
    friend class SelectTool;
public:
    GraphicsScene();
    void addFigure(Figure *figure);
    void addScreen(QGraphicsPixmapItem *item);
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private slots:
    void updateScene();
private:
    QTimer *_timer;
    QList <Figure *> _figures;
    QGraphicsPixmapItem *_screen;
};

#endif // GRAPHICSVIEW_H
