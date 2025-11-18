#include "core/engines/renpy/renpyanalyzer.h"
#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QDir>
#include <QProcess>
#include <QStandardPaths>

namespace core {
namespace engines {
namespace renpy {

core::AnalyzerOutput RenpyAnalyzer::analyze(const QString &inputPath)
{
    core::AnalyzerOutput output;
    output.format = QStringLiteral("application/json");
    
    QJsonArray extractedStrings;
    QDir dir(inputPath);
    
    if (!dir.exists()) {
        output.errorMessage = QStringLiteral("Path does not exist");
        return output;
    }
    
    // ตรวจสอบและ decompile ไฟล์ .rpyc
    QStringList rpycFilters;
    rpycFilters << "*.rpyc";
    QFileInfoList rpycFiles = dir.entryInfoList(rpycFilters, QDir::Files);
    
    if (!rpycFiles.isEmpty()) {
        // ลองหา unrpyc
        QString unrpycCmd;
        QProcess testProcess;
        
        // ลอง 1: python -m unrpyc
        testProcess.start("python3", QStringList() << "-m" << "unrpyc" << "--help");
        if (testProcess.waitForFinished(2000) && testProcess.exitCode() == 0) {
            unrpycCmd = "python3 -m unrpyc";
        } else {
            // ลอง 2: unrpyc command
            testProcess.start("unrpyc", QStringList() << "--help");
            if (testProcess.waitForFinished(2000) && testProcess.exitCode() == 0) {
                unrpycCmd = "unrpyc";
            }
        }
        
        if (unrpycCmd.isEmpty()) {
            output.errorMessage = QStringLiteral("unrpyc not found. Install: pip install unrpyc-ng");
            return output;
        }
        
        // Decompile แต่ละไฟล์
        for (const QFileInfo &rpycFile : rpycFiles) {
            QString rpycPath = rpycFile.absoluteFilePath();
            QString rpyPath = rpycPath.left(rpycPath.length() - 1);
            
            if (QFile::exists(rpyPath))
                continue;
            
            QProcess process;
            process.setWorkingDirectory(dir.absolutePath());
            
            if (unrpycCmd.startsWith("python")) {
                process.start("python3", QStringList() << "-m" << "unrpyc" << rpycPath);
            } else {
                process.start("unrpyc", QStringList() << rpycPath);
            }
            
            if (!process.waitForFinished(10000)) {
                output.errorMessage = QStringLiteral("Decompile timeout");
                return output;
            }
            
            if (process.exitCode() != 0) {
                output.errorMessage = QString("Decompile failed: %1").arg(QString::fromUtf8(process.readAllStandardError()));
                return output;
            }
        }
    }
    
    // อ่านไฟล์ .rpy
    QStringList filters;
    filters << "*.rpy";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    
    if (files.isEmpty()) {
        output.errorMessage = QStringLiteral("No .rpy files found");
        return output;
    }
    
    static QRegularExpression dialogPattern(R"(^\s*(?:[a-zA-Z_]\w*\s+)?\"([^\"]+)\")");
    static QRegularExpression menuPattern(R"(^\s*\"([^\"]+)\"\s*:)");
    
    for (const QFileInfo &fileInfo : files) {
        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;
        
        QTextStream in(&file);
        int lineNumber = 0;
        
        while (!in.atEnd()) {
            QString line = in.readLine();
            lineNumber++;
            
            QRegularExpressionMatch match = dialogPattern.match(line);
            if (!match.hasMatch())
                match = menuPattern.match(line);
            
            if (match.hasMatch()) {
                QString text = match.captured(1);
                if (!text.trimmed().isEmpty()) {
                    QJsonObject entry;
                    entry["file"] = fileInfo.fileName();
                    entry["line"] = lineNumber;
                    entry["original"] = text;
                    entry["translation"] = "";
                    extractedStrings.append(entry);
                }
            }
        }
        file.close();
    }
    
    QJsonDocument doc(extractedStrings);
    output.payload = doc.toJson();
    return output;
}

bool RenpyAnalyzer::save(const QString &outputPath, const QJsonArray &texts)
{
    QFile file(outputPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    
    QTextStream out(&file);
    
    for (const QJsonValue &value : texts) {
        QJsonObject obj = value.toObject();
        QString original = obj["original"].toString();
        QString translation = obj["translation"].toString();
        
        if (!translation.isEmpty()) {
            out << "translate None:\n";
            out << "    old \"" << original << "\"\n";
            out << "    new \"" << translation << "\"\n\n";
        }
    }
    
    file.close();
    return true;
}

} // namespace renpy
} // namespace engines
} // namespace core
