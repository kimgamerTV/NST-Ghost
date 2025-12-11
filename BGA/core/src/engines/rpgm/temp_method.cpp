QPair<QString, QString> RpgmAnalyzer::splitStringPrefix(const QString &text)
{
    // Check for common script prefixes like "if(s[66])"
    // Regex: ^if\(s\[\d+\]\)
    static QRegularExpression prefixRegex("^(if\\(s\\[\\d+\\]\\))");
    QRegularExpressionMatch match = prefixRegex.match(text);
    
    if (match.hasMatch()) {
        QString prefix = match.captured(1);
        QString cleanText = text.mid(prefix.length());
        return qMakePair(prefix, cleanText);
    }
    
    // No prefix found
    return qMakePair(QString(""), text);
}
