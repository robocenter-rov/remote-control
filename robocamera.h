#ifndef ROBOCAMERA_H
#define ROBOCAMERA_H

#include <QCamera>
#include <QVideoWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QCameraImageCapture>
#include <QCoreApplication>

class RoboCamera
{
public:
    RoboCamera(QGraphicsScene *scene = nullptr);
    RoboCamera(QGraphicsView *gv, QObject *gv_parent, QGraphicsScene *scene = nullptr);
    ~RoboCamera();
    QGraphicsScene *getScene() const;
    QImage getLastSavedImage();
    void imageCapture();
    void addVideoWidget();
private:
    void sceneInit(QGraphicsScene *scene = nullptr);
    QCamera *_camera = nullptr;
    QGraphicsScene *_scene = nullptr;
    QVideoWidget *_videoWidget = nullptr;
    QCameraImageCapture *_imageCapture = nullptr;
    QImage _lastSavedImg;
    int _imgId = 0;
};

#endif // ROBOCAMERA_H
