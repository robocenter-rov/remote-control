#ifndef CALCWINDOW_H
#define CALCWINDOW_H

#include <QMainWindow>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include "robocamera.h"

namespace Ui {
class CalcWindow;
}

class GraphicsScene;
class BaseTool;

class CalcWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalcWindow(QWidget *parent = 0);
    ~CalcWindow();

private slots:
    void onMakeScreenButtonClicked();
private:
    void loadQSS();
    void generateTools();
    bool eventFilter(QObject *, QEvent *event);
    void onScreenViewMousePressEvent(QMouseEvent *event);

    Ui::CalcWindow *_ui;
    RoboCamera *_mainCamera;
    GraphicsScene *_screenScene;
    QGraphicsPixmapItem _screen;
};

#endif // CALCWINDOW_H
