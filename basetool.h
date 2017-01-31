#ifndef BASETOOLS_H
#define BASETOOLS_H

#include <QObject>
#include <QPushButton>
#include <QtCore>

class GraphicsScene;

class BaseTool : public QObject
{
    Q_OBJECT
public:
    BaseTool(QWidget *parent = nullptr);
    virtual ~BaseTool();
    void virtual drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point) { }
    void virtual drawOnMousePress(GraphicsScene *scene, QPointF point) {}
    void virtual drawOnMouseMove(GraphicsScene *scene, QPointF point) {}
    void virtual drawOnMouseRelease(GraphicsScene *scene, QPointF point) {}
protected:
    QPushButton *_button;
    QString _iconsPath;
private slots:
    void onToolButtonClick(bool checked = false);
};

class LineTool: public BaseTool
{
public:
    LineTool(QWidget *parent = nullptr);
    ~LineTool();
    void drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point) override;
    void drawOnMousePress(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseMove(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseRelease(GraphicsScene *scene, QPointF point) override;
protected:
    QPointF _startPos;
    bool _isDraw = false;
};

extern QList <BaseTool *> tools;
extern BaseTool *currentTool;
#endif // BASETOOLS_H
