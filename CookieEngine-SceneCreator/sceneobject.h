#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <QGraphicsItem>
#include <QPen>
#include <QBrush>
#include <QPainter>

class SceneObject : public QGraphicsItem
{
private:
    QRect rect;
    QPen pen;
    QBrush brush;
public:
    SceneObject(int width, int height, QPen pen, QBrush brush);
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

};

#endif // SCENEOBJECT_H
