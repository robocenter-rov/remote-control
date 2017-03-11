#ifndef FIGURE_H
#define FIGURE_H

#include "graphicsscene.h"
#include <string>

using namespace std;

class Figure
{
public:
    Figure();
    ~Figure(){}
    void virtual draw(QGraphicsScene *scene) {}
    void virtual drawArea(QGraphicsScene *scene) {}
    bool virtual inArea(QPointF p) { return true; }
    void virtual resetPoints(QPointF deltaPoint) {}
    void virtual calcArea() {}
    QString virtual getInfo() { return ""; }
protected:
    QPen _pen;
    const double _offset = 3;
};

class LineFigure : public Figure
{
public:
    LineFigure(QPointF p1, QPointF p2);
    ~LineFigure(){}
    void draw(QGraphicsScene *scene) override;
    void drawArea(QGraphicsScene *scene) override;
    bool inArea(QPointF p) override;
    void resetPoints(QPointF deltaPoint) override;
    void calcArea() override;
    QString getInfo() override;
    int length();
    QPointF rotatedEndPoint2();
private:
    QPointF _p1;
    QPointF _p2;
    QPolygonF _area;
    double _angle;
};

class RectFigure : public Figure
{
public:
    RectFigure(QPointF p1, QPointF p2);
    ~RectFigure(){}
    void draw(QGraphicsScene *scene) override;
    void drawArea(QGraphicsScene *scene) override;
    bool inArea(QPointF p) override;
    void resetPoints(QPointF deltaPoint) override;
    void calcArea() override;
    QString getInfo() override;
    QVector <QPointF> rotatedPoints();
    int width();
    int height();
private:
    QVector <QPointF> _points;
    QPointF _pCenter;
    QPolygonF _area;
    double _angle;
};

extern void calcPoints(QPointF &p1, QPointF &p2);
#endif // FIGURE_H
