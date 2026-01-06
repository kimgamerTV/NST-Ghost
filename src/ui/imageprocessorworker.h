#ifndef IMAGEPROCESSORWORKER_H
#define IMAGEPROCESSORWORKER_H

#include <QObject>
#include <QJsonArray>

class ImageProcessorWorker : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessorWorker(QObject *parent = nullptr);
    ~ImageProcessorWorker();

public slots:
    void initialize();
    void processImage(const QString &imagePath, const QString &sourceLang, bool useGcv, const QString &gcvKeyPath);

signals:
    void initialized(bool available, const QString &status, bool useGpu, const QString &deviceName);
    void progress(const QString &message);
    void processingFinished(const QString &imagePath, const QJsonArray &detections, const QString &inpaintedPath);
    void errorOccurred(const QString &error);

private:
    struct Private;
    Private *d;
};

#endif // IMAGEPROCESSORWORKER_H
