#ifndef RELATIONITEM_H
#define RELATIONITEM_H

#include <QGraphicsObject>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>

class RelationEdgeItem;

class RelationNodeItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit RelationNodeItem(const QString &label, const QString &type, QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    void addEdge(RelationEdgeItem *edge);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    QString label() const { return m_label; }
    QString relationType() const { return m_type; }

private:
    QString m_label;
    QString m_type;
    QList<RelationEdgeItem*> m_edges;
    QColor m_backgroundColor;
};

class RelationEdgeItem : public QGraphicsLineItem
{
public:
    RelationEdgeItem(RelationNodeItem *source, RelationNodeItem *dest);

    void adjust();
    void setLabel(const QString &label);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    RelationNodeItem *m_source;
    RelationNodeItem *m_dest;
    QString m_text;
};

#endif // RELATIONITEM_H
