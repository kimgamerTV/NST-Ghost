#include "relationshipwidget.h"
#include <QGraphicsSimpleTextItem>
#include <cmath>

RelationshipWidget::RelationshipWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void RelationshipWidget::setupUi()
{
    m_layout = new QVBoxLayout(this);
    
    m_headerLabel = new QLabel("Data Relation Map", this);
    QFont headerFont = m_headerLabel->font();
    headerFont.setPointSize(16);
    headerFont.setBold(true);
    m_headerLabel->setFont(headerFont);
    
    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setDragMode(QGraphicsView::ScrollHandDrag);
    
    m_layout->addWidget(m_headerLabel);
    
    // Controls
    m_controlsLayout = new QHBoxLayout();
    QLabel *zoomLabel = new QLabel("Zoom:", this);
    m_zoomSlider = new QSlider(Qt::Horizontal, this);
    m_zoomSlider->setRange(10, 200); // 10% to 200%
    m_zoomSlider->setValue(100);
    
    connect(m_zoomSlider, &QSlider::valueChanged, this, [this](int value){
        qreal scale = value / 100.0;
        QTransform t;
        t.scale(scale, scale);
        m_view->setTransform(t);
    });
    
    m_controlsLayout->addWidget(zoomLabel);
    m_controlsLayout->addWidget(m_zoomSlider);
    m_controlsLayout->addStretch();
    
    m_layout->addLayout(m_controlsLayout);
    m_layout->addWidget(m_view);
}

void RelationshipWidget::clearGraph()
{
    m_scene->clear();
    qDeleteAll(m_edges);
    m_edges.clear();
    
    // Clean up nodes
    for(auto node : m_nodes) {
        delete node;
    }
    m_nodes.clear();
}

void RelationshipWidget::loadRelations(const QString &projectPath)
{
    clearGraph();
    
    // Mock Data Generation logic
    // We create a mix of connected and isolated nodes to test filtering
    
    // Helper to create node
    auto createNode = [&](const QString &id, const QString &label, const QString &type) {
        if (m_nodes.contains(id)) return m_nodes[id];
        RelationNode *node = new RelationNode{id, label, type};
        m_nodes.insert(id, node);
        return node;
    };
    
    // Helper to create edge
    auto createEdge = [&](const QString &fromId, const QString &toId, const QString &label) {
        RelationNode *src = m_nodes.value(fromId);
        RelationNode *dst = m_nodes.value(toId);
        if (src && dst) {
            RelationEdge *edgeData = new RelationEdge{src, dst, label};
             // Edge item creation moved to createGraphItems or here? 
             // Let's keep logic separated for now, or just store data.
             // But wait, RelationEdgeItem needs items to exist.
             // We'll create items later.
            m_edges.append(edgeData);
        }
    };

    // Use Real Data Analyzer
    RpgRelationAnalyzer analyzer(projectPath);
    QList<RpgDependency> realDeps = analyzer.analyze();
    
    if (!realDeps.isEmpty()) {
        for (const auto &dep : realDeps) {
            createNode(dep.sourceId, dep.sourceLabel, dep.sourceType);
            createNode(dep.targetId, dep.targetLabel, dep.targetType);
            createEdge(dep.sourceId, dep.targetId, dep.relationType);
        }
    } else {
        // Fallback to Mock Data if no real data found (e.g. invalid path or empty project)
        // --- Connected Component 1: Hero Party Logic ---
        createNode("actor_1", "Actor: Reid", "Actor");
        createNode("class_1", "Class: Swordsman", "Class");
        createNode("skill_1", "Skill: Slash", "Skill");
        createNode("skill_2", "Skill: Cross Cut", "Skill");
        createNode("anim_1", "Anim: Sword Hit", "Animation");
        
        createEdge("actor_1", "class_1", "Has Class");
        createEdge("class_1", "skill_1", "Learns");
        createEdge("class_1", "skill_2", "Learns");
        createEdge("skill_1", "anim_1", "Plays");
        createEdge("skill_2", "anim_1", "Plays");
    
        // --- Connected Component 2: Event System ---
        createNode("map_1", "Map001", "Map");
        createNode("ev_1", "Event: Chest", "Event");
        createNode("ce_1", "CE: Open Chest", "CommonEvent");
        createNode("sw_1", "Switch: Chest Open", "Switch");
        
        createEdge("map_1", "ev_1", "Contains");
        createEdge("ev_1", "ce_1", "Calls");
        createEdge("ce_1", "sw_1", "Toggles");
    
        // --- Isolated Nodes (Should be filtered out) ---
        createNode("actor_2", "Actor: Unused", "Actor");
        createNode("item_99", "Item: Debug Sword", "Item");
    }
    
    filterIsolatedNodes();
    createGraphItems();
    layoutGraph();
}

void RelationshipWidget::filterIsolatedNodes()
{
    // Remove nodes that have no edges
    QList<QString> toRemove;
    for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it) {
        if (it.value()->edges.isEmpty()) {
            toRemove.append(it.key());
            delete it.value(); // Clean up memory
        }
    }
    
    for (const QString &key : toRemove) {
        m_nodes.remove(key);
    }
}

void RelationshipWidget::createGraphItems()
{
    for (auto node : m_nodes) {
        // Create interactive item
        RelationNodeItem *item = new RelationNodeItem(node->label, node->type);
        m_scene->addItem(item);
        node->item = item;
    }
    
    for (auto edgeData : m_edges) {
        // Only create edge if both nodes survived filtering
        if (m_nodes.contains(edgeData->from->id) && m_nodes.contains(edgeData->to->id)) {
            RelationEdgeItem *edgeItem = new RelationEdgeItem(edgeData->from->item, edgeData->to->item);
            edgeItem->setLabel(edgeData->label);
            m_scene->addItem(edgeItem);
            edgeData->item = edgeItem;
        }
    }
}

void RelationshipWidget::layoutGraph()
{
    // Extremely simple manual layout for demo purposes
    // Better apapproach would be a force-directed graph
    
    if (m_nodes.isEmpty()) return;

    // Arrange nodes in a grid to ensure they don't overlap initially
    int x = -400;
    int y = -300;
    int spacingX = 250;
    int spacingY = 150;
    int cols = std::ceil(std::sqrt(m_nodes.size()));
    
    int count = 0;
    for (auto node : m_nodes) {
        if (node->item) {
             node->item->setPos(x, y);
             x += spacingX;
             if (++count % cols == 0) {
                 x = -400;
                 y += spacingY;
             }
        }
    }
    
    // Edges auto-update via itemChange, no need to manually setLine here
    // However, we might want to trigger an initial adjust if needed, 
    // but constructor of EdgeItem calls adjust().
    
    m_scene->setSceneRect(m_scene->itemsBoundingRect().adjusted(-100, -100, 100, 100));
}
