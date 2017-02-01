#include "figure.h"

Figure::Figure()
{
    _pen = QPen(QColor(0, 0, 127, 127));
}

LineFigure::LineFigure(QPointF p1, QPointF p2) :
    Figure()
{
    _p1 = p1;
    _p2 = p2;
}

void LineFigure::draw(GraphicsScene *scene)
{
    scene->addLine(_p1.x(), _p1.y(), _p2.x(), _p2.y(), _pen);
}

RectFigure::RectFigure(QPointF p1, QPointF p2) :
    Figure()
{
    _p1 = p1;
    _p2 = p2;
}

void RectFigure::draw(GraphicsScene *scene)
{
    scene->addRect(QRectF(_p1, _p2), _pen);
}
