#ifndef RELATIONSHIPWIDGET_H
#define RELATIONSHIPWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMap>
#include "relationitem.h"
#include "rpgrelationanalyzer.h"

// Forward declaration if needed, but we included header
// struct RelationEdge;

struct RelationNode {
    QString id;
    QString label;
    QString type;
    RelationNodeItem *item = nullptr;
    QList<RelationEdgeItem*> edges;
};

// RelationEdge struct is less critical if we use RelationEdgeItem directly, 
// but keeping it for data tracking is fine.
struct RelationEdge {
    RelationNode *from;
    RelationNode *to;
    QString label;
    RelationEdgeItem *item = nullptr;
};

class RelationshipWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RelationshipWidget(QWidget *parent = nullptr);

    // Placeholder for loading data
    void loadRelations(const QString &projectPath);

private:
    QVBoxLayout *m_layout;
    QHBoxLayout *m_controlsLayout;
    QLabel *m_headerLabel;
    QSlider *m_zoomSlider;
    QGraphicsView *m_view;
    QGraphicsScene *m_scene;

    QMap<QString, RelationNode*> m_nodes;
    QList<RelationEdge*> m_edges;

    void setupUi();
    void clearGraph();
    void layoutGraph();
    void createGraphItems();
    
    // Filtering
    void filterIsolatedNodes();
};

#endif // RELATIONSHIPWIDGET_H
