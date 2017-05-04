#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QObject>

class Figure;
class LineFigure;
class AxisFigure;
class SelectTool;
class RoboCamera;
class MapGraphicsScene;

extern QList <Figure *> figures;
extern AxisFigure *axis;
extern LineFigure *poolLine;
extern QList <QRectF> containerCenters;

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
    friend class SelectTool;
public:
    GraphicsScene();
    void addFigure(Figure *figure);
    void addScreen(QGraphicsPixmapItem *item);
    void clearScene();
    void setShowAxes(bool value);
    void setAxesAngle(double angle);
    void updateAxisCenter(QPointF point);
    void setShowContainersCenters(bool value);
protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
public slots:
    void updateScene();
private:
    QGraphicsPixmapItem *_screen;
    bool _showAxes;
    bool _showContainersCenters;
    double _axesAngle;
    QPointF _centerOfCoordSystem;
};

class VideoGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    VideoGraphicsScene();
    void addScreen(QGraphicsPixmapItem *item);
    void makeScreen();
    void setCamera(RoboCamera *camera);
    void addScreenScene(GraphicsScene *screenScene);
    void addMapScene(MapGraphicsScene *mapScene);
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
    MapGraphicsScene *_mapScene;
};

class MapGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    MapGraphicsScene();
    void updateScene();
    void clearScene();
};

#endif // GRAPHICSVIEW_H
