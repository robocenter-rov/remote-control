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
    RoboCamera();
    RoboCamera(QGraphicsView *gv, QObject *gv_parent, const QByteArray &deviceName);
    ~RoboCamera();
    QGraphicsScene *getScene() const;
    QImage getLastSavedImage();
    void imageCapture();
private:
    void sceneInit();
    QCamera *_camera = nullptr;
    QGraphicsScene *_scene = nullptr;
    QVideoWidget *_videoWidget = nullptr;
    QCameraImageCapture *_imageCapture = nullptr;
    QImage _lastSavedImg;
    int _imgId = 0;
};

#endif // ROBOCAMERA_H
