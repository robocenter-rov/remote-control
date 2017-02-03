#ifndef BASETOOLS_H
#define BASETOOLS_H

#include <QObject>
#include <QPushButton>
#include <QtCore>
#include <QDoubleSpinBox>
#include "figure.h"

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
    void virtual createToolProperties() {}
    void virtual deleteToolProperties() {}
    void setLevel(int level);
protected:
    QPushButton *_button;
    QString _iconsPath;
    QWidget *_parent;
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
    QPointF _endPos;
    LineFigure *_line;
};

class OptionTool : public LineTool
{
public:
    OptionTool(QWidget *parent = nullptr);
    ~OptionTool();
    void createToolProperties() override;
    void deleteToolProperties() override;
    void drawOnMouseRelease(GraphicsScene *scene, QPointF point) override;
protected:
    QDoubleSpinBox *_spinBox;
};

class RectTool : public BaseTool
{
public:
    RectTool(QWidget *parent = nullptr);
    ~RectTool();
    void drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point) override;
    void drawOnMousePress(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseMove(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseRelease(GraphicsScene *scene, QPointF point) override;
protected:
    QPointF _p1;
    QPointF _p2;
};
extern QList <BaseTool *> tools;
extern BaseTool *currentTool;
extern double scaleCoef;

class SelectTool : public BaseTool
{
public:
    SelectTool(QWidget *parent = nullptr);
    ~SelectTool(){}
    void drawOnMouseDoubleClick(GraphicsScene *scene, QPointF point) override;
    void drawOnMousePress(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseMove(GraphicsScene *scene, QPointF point) override;
    void drawOnMouseRelease(GraphicsScene *scene, QPointF point) override;
private:
    QPointF _startPoint;
    QList <Figure *> _selectedFigures;
};

#endif // BASETOOLS_H
