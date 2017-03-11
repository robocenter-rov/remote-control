#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QObject>

class Figure;
class LineFigure;
class SelectTool;
class RoboCamera;

extern QList <Figure *> figures;

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
    friend class SelectTool;
public:
    GraphicsScene();
    void addFigure(Figure *figure);
    void addScreen(QGraphicsPixmapItem *item);
    void clearScene();
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
private slots:
    void updateScene();
private:
    QTimer *_timer;
    QGraphicsPixmapItem *_screen;
};

class VideoGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    VideoGraphicsScene();
    void addScreen(QGraphicsPixmapItem *item);
    void makeScreen();
    void setCamera(RoboCamera *camera);
    void addScreenView(GraphicsScene *screenScene);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
private slots:
    void updateAnimationMakeScreen();
private:
    void clearTimer();
    void clearScreenItem();
    void clearScreen();
    void startNewAnimation();
    RoboCamera *_mainCamera;
    QGraphicsPixmapItem *_screen;
    QTimer *_timer;
    QPixmap _pic;
    double _picOpacity;
    QGraphicsPixmapItem *_screenItem;
    GraphicsScene *_screenScene;
};

#endif // GRAPHICSVIEW_H
