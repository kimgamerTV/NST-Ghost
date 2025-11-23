#include "smartfiltermanager.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QSet>

SmartFilterManager::SmartFilterManager(QObject *parent)
    : QObject(parent)
{
    loadPatterns();
}

void SmartFilterManager::learn(const QString &text)
{
    if (text.isEmpty()) return;

    // Simple learning: Add exact match or simple wildcard if it looks like a pattern
    // For now, we'll just add it as an exact match or a simple regex if it contains numbers
    
    QString pattern = QRegularExpression::escape(text);
    
    // If it ends with numbers, generalize it (e.g., "Var_001" -> "Var_\d+")
    static QRegularExpression numberSuffix("_?\\d+$");
    if (text.contains(numberSuffix)) {
        pattern = text;
        pattern.replace(numberSuffix, "_?\\\\d+");
        // Un-escape the rest but keep the regex part? No, safer to escape everything then replace.
        pattern = QRegularExpression::escape(text);
        pattern.replace(QRegularExpression("_?\\d+$"), "_?\\\\d+");
    }

    if (!m_ignoredPatterns.contains(pattern)) {
        m_ignoredPatterns.append(pattern);
        m_compiledPatterns.append(QRegularExpression(pattern));
        savePatterns();
        qDebug() << "SmartFilterManager: Learned pattern:" << pattern << "for engine:" << m_currentEngine;
    }
}

void SmartFilterManager::unlearn(const QString &text)
{
    if (text.isEmpty()) return;

    QString pattern = QRegularExpression::escape(text);
    // Try to match the generalized pattern logic from learn()
    static QRegularExpression numberSuffix("_?\\d+$");
    if (text.contains(numberSuffix)) {
        pattern = text;
        pattern.replace(numberSuffix, "_?\\\\d+");
        pattern = QRegularExpression::escape(text);
        pattern.replace(QRegularExpression("_?\\d+$"), "_?\\\\d+");
    }

    if (m_ignoredPatterns.contains(pattern)) {
        m_ignoredPatterns.removeOne(pattern);
        
        // Rebuild compiled patterns
        m_compiledPatterns.clear();
        for (const QString &p : m_ignoredPatterns) {
            m_compiledPatterns.append(QRegularExpression(p));
        }
        
        savePatterns();
        qDebug() << "SmartFilterManager: Unlearned pattern:" << pattern << "for engine:" << m_currentEngine;
    }
}

void SmartFilterManager::setEngine(const QString &engineName)
{
    if (m_currentEngine != engineName) {
        m_currentEngine = engineName;
        if (m_currentEngine.isEmpty()) m_currentEngine = "Global";
        loadPatterns();
    }
}

bool SmartFilterManager::exportRules(const QString &filePath)
{
    QJsonObject rootObject;
    QSettings settings("MySoft", "NST");
    
    settings.beginGroup("SmartFilter");
    QStringList engines = settings.childGroups();
    
    // Also handle the case where "IgnoredPatterns" might be at the root of SmartFilter (legacy or Global)
    // But based on previous code, we save as SmartFilter/<Engine>/IgnoredPatterns
    // Let's iterate through all groups
    
    for (const QString &engine : engines) {
        settings.beginGroup(engine);
        QStringList patterns = settings.value("IgnoredPatterns").toStringList();
        if (!patterns.isEmpty()) {
            QJsonArray jsonPatterns;
            for (const QString &p : patterns) jsonPatterns.append(p);
            rootObject[engine] = jsonPatterns;
        }
        settings.endGroup();
    }
    settings.endGroup();

    QJsonDocument doc(rootObject);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open file for export:" << filePath;
        return false;
    }
    file.write(doc.toJson());
    return true;
}

bool SmartFilterManager::importRules(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for import:" << filePath;
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject()) return false;

    QJsonObject rootObject = doc.object();
    QSettings settings("MySoft", "NST");
    int newPatternsCount = 0;

    for (auto it = rootObject.begin(); it != rootObject.end(); ++it) {
        QString engine = it.key();
        QJsonArray jsonPatterns = it.value().toArray();
        
        // Load existing patterns for this engine
        QString key = QString("SmartFilter/%1/IgnoredPatterns").arg(engine);
        QStringList existingPatterns = settings.value(key).toStringList();
        QSet<QString> patternSet(existingPatterns.begin(), existingPatterns.end());
        
        for (const QJsonValue &val : jsonPatterns) {
            QString pattern = val.toString();
            if (!patternSet.contains(pattern)) {
                patternSet.insert(pattern);
                existingPatterns.append(pattern);
                newPatternsCount++;
            }
        }
        
        settings.setValue(key, existingPatterns);
    }
    
    // Reload patterns if we modified the current engine
    loadPatterns();
    qDebug() << "Imported" << newPatternsCount << "new patterns.";
    return true;
}

bool SmartFilterManager::shouldSkip(const QString &text) const
{
    if (text.isEmpty()) return true;

    // 1. Check Heuristics
    if (isNumericOrSymbol(text)) return true;
    if (isFilePath(text)) return true;
    if (isVariableLike(text)) return true;
    if (isCamelCase(text)) return true;
    if (isSnakeCase(text)) return true;
    if (isTagOrMarkup(text)) return true;
    if (isTechnicalString(text)) return true;
    if (isRepeatedSymbol(text)) return true;

    // 2. Check Learned Patterns
    for (const QRegularExpression &regex : m_compiledPatterns) {
        if (regex.match(text).hasMatch()) {
            return true;
        }
    }

    return false;
}

QStringList SmartFilterManager::ignoredPatterns() const
{
    return m_ignoredPatterns;
}

void SmartFilterManager::savePatterns()
{
    QSettings settings("MySoft", "NST");
    settings.setValue(QString("SmartFilter/%1/IgnoredPatterns").arg(m_currentEngine), m_ignoredPatterns);
}

void SmartFilterManager::loadPatterns()
{
    QSettings settings("MySoft", "NST");
    m_ignoredPatterns = settings.value(QString("SmartFilter/%1/IgnoredPatterns").arg(m_currentEngine)).toStringList();
    
    m_compiledPatterns.clear();
    for (const QString &pattern : m_ignoredPatterns) {
        m_compiledPatterns.append(QRegularExpression(pattern));
    }
}

bool SmartFilterManager::isNumericOrSymbol(const QString &text) const
{
    // Check if text contains only non-letter characters
    static QRegularExpression regex("^[^\\p{L}]+$"); 
    return regex.match(text).hasMatch();
}

bool SmartFilterManager::isFilePath(const QString &text) const
{
    // Simple check for file extensions or path separators
    return text.contains("/") || text.contains("\\") || text.contains(".png") || text.contains(".ogg") || text.contains(".json");
}

bool SmartFilterManager::isVariableLike(const QString &text) const
{
    // Check for common variable patterns like "Actor_1", "Switch_05"
    // Starts with letter, contains underscore and numbers, no spaces
    static QRegularExpression regex("^[A-Za-z]+_[0-9]+$");
    return regex.match(text).hasMatch();
}

bool SmartFilterManager::isCamelCase(const QString &text) const
{
    // No spaces, starts with letter, contains mixed case (e.g., "PlayerName", "maxHealth")
    // Must have at least one uppercase and one lowercase
    if (text.contains(" ")) return false;
    static QRegularExpression regex("^[a-zA-Z0-9]+$");
    if (!regex.match(text).hasMatch()) return false;

    bool hasUpper = false;
    bool hasLower = false;
    for (const QChar &c : text) {
        if (c.isUpper()) hasUpper = true;
        if (c.isLower()) hasLower = true;
    }
    return hasUpper && hasLower;
}

bool SmartFilterManager::isSnakeCase(const QString &text) const
{
    // Contains underscore, no spaces, mostly lowercase or uppercase (e.g., "enemy_id", "MAP_01")
    if (text.contains(" ")) return false;
    if (!text.contains("_")) return false;
    
    static QRegularExpression regex("^[a-zA-Z0-9_]+$");
    return regex.match(text).hasMatch();
}

bool SmartFilterManager::isTagOrMarkup(const QString &text) const
{
    // Enclosed in brackets <...>, [...], {...}
    static QRegularExpression regex("^(\\<.*\\>|\\[.*\\]|\\{.*\\})$");
    return regex.match(text).hasMatch();
}

bool SmartFilterManager::isTechnicalString(const QString &text) const
{
    // Mixed alphanumeric with no clear meaning, or hex codes (e.g., "0x123", "a1b2")
    // Or contains only symbols and numbers
    static QRegularExpression regex("^(0x[0-9A-Fa-f]+|[A-Za-z0-9]{1,4})$"); // Short alphanumeric codes
    if (regex.match(text).hasMatch()) return true;
    
    // Check for mostly symbols
    int symbolCount = 0;
    for (const QChar &c : text) {
        if (!c.isLetterOrNumber() && !c.isSpace()) symbolCount++;
    }
    if (text.length() > 0 && (double)symbolCount / text.length() > 0.5) return true;

    return false;
}

bool SmartFilterManager::isRepeatedSymbol(const QString &text) const
{
    // e.g., "======", "-----"
    if (text.length() < 3) return false;
    QChar first = text.at(0);
    if (first.isLetterOrNumber()) return false;
    
    for (const QChar &c : text) {
        if (c != first) return false;
    }
    return true;
}
