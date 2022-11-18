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
    QString objectType;

public:
    static SceneObject* selectedObject;

    SceneObject(QString objectType, int width, int height, QPen pen, QBrush brush);
    QRectF boundingRect() const;
    QString getObjectType() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // SCENEOBJECT_H
