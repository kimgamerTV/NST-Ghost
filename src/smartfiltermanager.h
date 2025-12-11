#ifndef SMARTFILTERMANAGER_H
#define SMARTFILTERMANAGER_H

#include <QObject>
#include <QStringList>
#include <QRegularExpression>
#include <QSettings>
#include <QRegularExpression>
#include <QSettings>

#pragma push_macro("slots")
#undef slots
#include <pybind11/embed.h>
#pragma pop_macro("slots")

namespace py = pybind11;

class SmartFilterManager : public QObject
{
    Q_OBJECT
public:
    explicit SmartFilterManager(QObject *parent = nullptr);

    // Adds a text pattern to the ignore list
    void learn(const QString &text);

    // Removes a text pattern from the ignore list
    void unlearn(const QString &text);

    // Sets the current engine context
    void setEngine(const QString &engineName);

    // Export all learned rules to a JSON file
    bool exportRules(const QString &filePath);

    // Import rules from a JSON file and merge with existing ones
    bool importRules(const QString &filePath);

    // Checks if the text should be skipped based on learned patterns or heuristics
    bool shouldSkip(const QString &text) const;

    // Checks a batch of texts and returns a list of booleans (true = skip)
    QList<bool> shouldSkipBatch(const QStringList &texts) const;

    // Returns the list of ignored patterns
    QStringList ignoredPatterns() const;
    
    // AI Configuration
    void setAIEnabled(bool enabled);
    bool isAIEnabled() const;
    void setAIThreshold(double threshold);
    double aiThreshold() const;

public slots:
    void savePatterns();
    void loadPatterns();

private:
    QStringList m_ignoredPatterns;
    QList<QRegularExpression> m_compiledPatterns;
    QString m_currentEngine = "Global"; // Default to Global
    
    // Python Integration
    py::object m_pyFilter;
    bool m_aiEnabled = true;
    double m_aiThreshold = 0.75;

    // Heuristic checks
    bool isNumericOrSymbol(const QString &text) const;
    bool isFilePath(const QString &text) const;
    bool isVariableLike(const QString &text) const;
    bool isCamelCase(const QString &text) const;
    bool isSnakeCase(const QString &text) const;
    bool isTagOrMarkup(const QString &text) const;
    bool isTechnicalString(const QString &text) const;
    bool isRepeatedSymbol(const QString &text) const;
};

#endif // SMARTFILTERMANAGER_H
