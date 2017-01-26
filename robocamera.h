#ifndef ROBOCAMERA_H
#define ROBOCAMERA_H

#include <QCamera>
#include <QVideoWidget>
#include <QGraphicsScene>
#include <QGraphicsView>

class RoboCamera
{
public:
    RoboCamera();
    RoboCamera(QGraphicsView *gv, QObject *gv_parent, const QByteArray &deviceName);
    ~RoboCamera();
    QGraphicsScene *getScene() const;
private:
    void sceneInit();
    QCamera *_camera = nullptr;
    QGraphicsScene *_scene = nullptr;
    QVideoWidget *_videoWidget = nullptr;
};

#endif // ROBOCAMERA_H
