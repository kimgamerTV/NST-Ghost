#ifndef IMAGEPROCESSORWORKER_H
#define IMAGEPROCESSORWORKER_H

#include <QObject>
#include <QJsonArray>
#include <QStringList>

// Forward declare pybind11 object to avoid exposing Python headers in our header
namespace pybind11 {
class object;
}

class ImageProcessorWorker : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessorWorker(QObject *parent = nullptr);
    ~ImageProcessorWorker();

public slots:
    void initialize();
    void processImage(const QString &imagePath, const QString &sourceLang);

signals:
    void initialized(bool success, const QString &statusMessage, bool useGpu, const QString &deviceName);
    void progress(const QString &message);
    void processingFinished(const QString &imagePath, const QJsonArray &detections, const QString &inpaintedPath);
    void errorOccurred(const QString &message);

private:
    struct Private;
    Private *d;
};

#endif // IMAGEPROCESSORWORKER_H
