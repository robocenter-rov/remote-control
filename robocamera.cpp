#include "robocamera.h"
#include <QCameraInfo>

RoboCamera::RoboCamera(QGraphicsScene *scene)
{
    sceneInit(scene);
}

RoboCamera::RoboCamera(QGraphicsView *gv, QObject *gv_parent, QGraphicsScene *scene, unsigned int idx)
{
    _idx = idx;
    sceneInit(scene);

    if (QCamera::availableDevices().count() < idx + 1) {
        qDebug() << "Unavaible video device\n";
        return;
    }
    _camera = new QCamera(QCameraInfo::availableCameras().at(idx));
    _imageCapture = new QCameraImageCapture(_camera);
    _imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    _camera->setCaptureMode(QCamera::CaptureStillImage);
    _camera->setViewfinder(_videoWidget);
    _camera->start();

    gv->setScene(_scene);
    gv->viewport()->installEventFilter(gv_parent);
    gv->show();
}

RoboCamera::~RoboCamera()
{
    delete _scene;
    delete _videoWidget;
    delete _camera;
    delete _imageCapture;
    _scene = nullptr;
    _videoWidget = nullptr;
    _camera = nullptr;
    _imageCapture = nullptr;
}

QGraphicsScene *RoboCamera::getScene() const
{
    return this->_scene;
}

void RoboCamera::sceneInit(QGraphicsScene *scene)
{
    if (scene == nullptr) _scene = new QGraphicsScene();
    else _scene = scene;
    addVideoWidget();
}

void RoboCamera::imageCapture()
{
    _camera->searchAndLock();
    char buf[10];
    itoa(_imgId++, (char *)buf, 10);
    QString path(QCoreApplication::applicationDirPath());
    _imageCapture->capture(path);
    _lastSavedImg = _videoWidget->grab().toImage();
    _camera->unlock();
}

QImage RoboCamera::getLastSavedImage()
{
    return _lastSavedImg;
}

void RoboCamera::addVideoWidget()
{
    if (_videoWidget == nullptr) {
        if (_idx == 0) {
            _videoWidget = new VideoWidget(_scene);

        } else {
            _videoWidget = new QVideoWidget();
        }
    }
    _scene->addWidget(_videoWidget);
}

VideoWidget *RoboCamera::getVideoWidget()
{
    return dynamic_cast<VideoWidget *>(_videoWidget);
}

VideoWidget::VideoWidget() : QVideoWidget::QVideoWidget()
{

}

VideoWidget::VideoWidget(QGraphicsScene *scene) : VideoWidget::VideoWidget()
{
    _scene = scene;
}

VideoWidget::~VideoWidget()
{
    QVideoWidget::~QVideoWidget();
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
    _sceneHeight = _scene->height();
    _sceneWidth = _scene->width();
    QVideoWidget::paintEvent(event);
    qreal h = _sceneHeight/15;
    qreal middle_y = _sceneHeight/2.0;
    qreal height = _sceneHeight;
    qreal nearestUp = int(_currentDepth) - (int(_currentDepth) == _currentDepth),
          nearestDown = int(_currentDepth) + 1;
    qreal p = _currentDepth - int(_currentDepth), q = 1 - p;

    if (p == 0) p = 1;
    QString s;

    QPainter painter(this);

    for (qreal i = middle_y + q*h; i < height + h/2; i += h){
        painter.drawLine(1, i, 8, i);
        painter.drawLine(1, i - h/2, 4, i - h/2);
        s.clear(); s.setNum(nearestDown++);
        painter.drawText(20.0, i - 12, s);
    }

    for (qreal i = middle_y - p*h; i > 0; i -= h){
        painter.drawLine(1, i, 8, i);
        painter.drawLine(1, i + h/2, 4, i + h/2);
        s.clear(); s.setNum(nearestUp--);
        painter.drawText(20.0, i - 12, s);
    }

    QVector<QPointF> points;

    points << QPointF(1, middle_y - 4) << QPointF(20, middle_y) << QPointF(1, middle_y + 4);
    QPolygonF pointer(points);
    painter.drawPolygon(pointer);
    s.clear(); s.setNum(_currentDepth);
    painter.drawText(36.0, middle_y + 4, s);
}

void VideoWidget::setCurrentDepth(double value)
{
    _currentDepth = value;
}
