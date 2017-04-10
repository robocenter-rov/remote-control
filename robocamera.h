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
    void setCurrentDepth(double value = 0.0);
protected:
    void paintEvent(QPaintEvent *event);
private:
    double _currentDepth = 0.0;
    QGraphicsScene *_scene = nullptr;
};

class RoboCamera
{
public:
    RoboCamera(QGraphicsScene *scene = nullptr);
    RoboCamera(unsigned int idx, QGraphicsView *gv, QObject *gv_parent, QGraphicsScene *scene = new QGraphicsScene(), bool showInfo = false);
    ~RoboCamera();
    QGraphicsScene *getScene() const;
    VideoWidget *getVideoWidget();
    QImage getLastSavedImage();
    void imageCapture();
    void addVideoWidget();
private:
    void sceneInit(QGraphicsScene *scene = new QGraphicsScene());
    QCamera *_camera = nullptr;
    QGraphicsScene *_scene;
    QVideoWidget *_videoWidget = nullptr;
    QCameraImageCapture *_imageCapture = nullptr;
    QImage _lastSavedImg;
    int _imgId = 0;
    int _idx;
    bool _showInfo;
};

#endif // ROBOCAMERA_H
