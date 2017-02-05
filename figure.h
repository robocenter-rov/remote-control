#ifndef FIGURE_H
#define FIGURE_H

#include "graphicsscene.h"

class Figure
{
public:
    Figure();
    ~Figure(){}
    void virtual draw(GraphicsScene *scene) {}
    void virtual drawArea(GraphicsScene *scene) {}
    void drawWithArea(GraphicsScene *scene);
    bool virtual inArea(QPointF p) { return true; }
    void virtual resetPoints(QPointF deltaPoint) {}
    void virtual calcArea() {}
protected:
    QPen _pen;
    const double _offset = 3;
};

class LineFigure : public Figure
{
public:
    LineFigure(QPointF p1, QPointF p2);
    ~LineFigure(){}
    void draw(GraphicsScene *scene) override;
    void drawArea(GraphicsScene *scene) override;
    bool inArea(QPointF p) override;
    void resetPoints(QPointF deltaPoint) override;
    void calcArea() override;
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
    void draw(GraphicsScene *scene) override;
    void drawArea(GraphicsScene *scene) override;
    bool inArea(QPointF p) override;
    void resetPoints(QPointF deltaPoint) override;
    void calcArea() override;
private:
    QPointF _p1;
    QPointF _p2;
    QRectF _area;
};

extern void calcPoints(QPointF &p1, QPointF &p2);
#endif // FIGURE_H
