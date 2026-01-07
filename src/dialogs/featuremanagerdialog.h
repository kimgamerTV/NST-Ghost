#ifndef FEATUREMANAGERDIALOG_H
#define FEATUREMANAGERDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QProgressBar>
#include <QProcess>

class FeatureManagerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FeatureManagerDialog(QWidget *parent = nullptr);

    // Feature detection
    static bool hasPythonSupport();  // Compiled with HAS_PYTHON
    static bool hasEasyOCR();        // Python + easyocr installed
    static bool hasPyTorch();        // Python + torch installed
    static bool hasInpainting();     // Python + simple-lama-inpainting

private slots:
    void onInstallAIFeatures();
    void onProcessOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);

private:
    void setupUI();
    void refreshStatus();
    QString getStatusIcon(bool installed) const;
    QString getStatusText(bool installed) const;
    
    // UI Elements
    QLabel *m_pythonStatus;
    QLabel *m_easyocrStatus;
    QLabel *m_pytorchStatus;
    QLabel *m_inpaintingStatus;
    
    QPushButton *m_installButton;
    QProgressBar *m_progressBar;
    QLabel *m_progressLabel;
    
    QProcess *m_installProcess;
};

#endif // FEATUREMANAGERDIALOG_H
