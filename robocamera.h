#ifndef ROBOCAMERA_H
#define ROBOCAMERA_H

#include <QCamera>
#include <QVideoWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QCameraImageCapture>
#include <QCoreApplication>

class VideoWidget : public QVideoWidget
{
public:
    VideoWidget();
    VideoWidget(QGraphicsScene *scene);
    ~VideoWidget();
    void setCurrentDepth(double value);
protected:
    void paintEvent(QPaintEvent *event);
private:
    double _currentDepth;
    QGraphicsScene *_scene = nullptr;
    double _sceneHeight, _sceneWidth; // Temp vars too
};

class RoboCamera
{
public:
    RoboCamera(QGraphicsScene *scene = nullptr);
    RoboCamera(QGraphicsView *gv, QObject *gv_parent, QGraphicsScene *scene = nullptr, unsigned int idx = 0);
    ~RoboCamera();
    QGraphicsScene *getScene() const;
    VideoWidget *getVideoWidget();
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
    int _idx = 0;
};

#endif // ROBOCAMERA_H
