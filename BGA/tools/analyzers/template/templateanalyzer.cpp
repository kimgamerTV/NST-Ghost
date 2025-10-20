#include "templateanalyzer.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

core::AnalyzerOutput TemplateAnalyzer::analyze(const QString &inputPath)
{
    QJsonObject root;
    root.insert(QStringLiteral("engine"), QStringLiteral("template"));
    root.insert(QStringLiteral("source"), inputPath);

    QJsonArray items;
    QJsonObject sampleItem;
    sampleItem.insert(QStringLiteral("id"), QStringLiteral("sample"));
    sampleItem.insert(QStringLiteral("text"), QStringLiteral("Sample extracted text"));
    items.append(sampleItem);

    root.insert(QStringLiteral("entries"), items);

    QJsonDocument document(root);

    core::AnalyzerOutput output;
    output.format = QStringLiteral("application/json");
    output.payload = document.toJson(QJsonDocument::Indented);
    return output;
}
