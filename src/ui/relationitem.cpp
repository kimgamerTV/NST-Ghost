#include "relationitem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <cmath>

// --- RelationNodeItem ---

RelationNodeItem::RelationNodeItem(const QString &label, const QString &type, QGraphicsItem *parent)
    : QGraphicsObject(parent), m_label(label), m_type(type)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    
    // Simple color coding based on type
    if (type == "Map") m_backgroundColor = QColor(200, 230, 255);       // Light Blue
    else if (type == "Event") m_backgroundColor = QColor(255, 230, 200);   // Light Orange
    else if (type == "CommonEvent") m_backgroundColor = QColor(255, 200, 200); // Light Red
    else if (type == "Switch") m_backgroundColor = QColor(200, 255, 200);      // Light Green
    else if (type == "Variable") m_backgroundColor = QColor(220, 255, 220);    // Pale Green
    else if (type == "Actor") m_backgroundColor = QColor(230, 200, 255);       // Light Purple
    else if (type == "Skill") m_backgroundColor = QColor(255, 255, 200);       // Light Yellow
    else m_backgroundColor = Qt::white;
}

QRectF RelationNodeItem::boundingRect() const
{
    return QRectF(-75, -20, 150, 40);
}

void RelationNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    // Shadow
    painter->setBrush(Qt::darkGray);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(-72, -17, 150, 40, 5, 5);

    // Main Body
    QColor fillColor = (option->state & QStyle::State_Selected) ? m_backgroundColor.darker(120) : m_backgroundColor;
    painter->setBrush(fillColor);
    
    QPen pen(Qt::black);
    if (option->state & QStyle::State_Selected) {
        pen.setWidth(2);
    }
    painter->setPen(pen);
    
    painter->drawRoundedRect(-75, -20, 150, 40, 5, 5);
    
    // Label
    painter->drawText(boundingRect(), Qt::AlignCenter, m_label);
}

void RelationNodeItem::addEdge(RelationEdgeItem *edge)
{
    m_edges.append(edge);
}

QVariant RelationNodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange) {
        for (RelationEdgeItem *edge : m_edges) {
            edge->adjust();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

// --- RelationEdgeItem ---

RelationEdgeItem::RelationEdgeItem(RelationNodeItem *source, RelationNodeItem *dest)
    : m_source(source), m_dest(dest)
{
    setPen(QPen(Qt::black, 2));
    setZValue(-1); // Behind nodes
    m_source->addEdge(this);
    m_dest->addEdge(this);
    adjust();
}

void RelationEdgeItem::setLabel(const QString &label)
{
    m_text = label;
}

void RelationEdgeItem::adjust()
{
    if (!m_source || !m_dest) return;

    QLineF line(mapFromItem(m_source, 0, 0), mapFromItem(m_dest, 0, 0));
    setLine(line);
}

void RelationEdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsLineItem::paint(painter, option, widget);
    
    // Draw Arrowhead
    QLineF l = line();
    if (l.length() == 0) return;
    
    double angle = std::atan2(-l.dy(), l.dx());
    
    // Calculate intersection with destination node box roughly
    // For exact intersection we'd need more math, but let's just back off a bit from center
    // Node is 150x40. 
    QPointF destP = l.p2();
    QPointF srcP = l.p1();
    
    // Should actually calculate intersection with rect, but for now simple offset
    // Let's shorten the line visually for the arrow
    
    QPointF arrowP1 = destP - QPointF(sin(angle + M_PI / 3) * 10, cos(angle + M_PI / 3) * 10);
    QPointF arrowP2 = destP - QPointF(sin(angle + M_PI - M_PI / 3) * 10, cos(angle + M_PI - M_PI / 3) * 10);
    
    // Draw Text Label at midpoint
    if (!m_text.isEmpty()) {
        QPointF mid = (srcP + destP) / 2;
        // Draw background for text
        QRectF textRect(mid.x() - 40, mid.y() - 10, 80, 20);
        painter->setBrush(QColor(255, 255, 255, 200));
        painter->setPen(Qt::NoPen);
        painter->drawRect(textRect);
        
        painter->setPen(Qt::black);
        painter->drawText(textRect, Qt::AlignCenter, m_text);
    }

    // Actual arrow drawing logic is tricky without full intersection, 
    // sticking to simple line for now or just text is cleaner for MVP.
}
