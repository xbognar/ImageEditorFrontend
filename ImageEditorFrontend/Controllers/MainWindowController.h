#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QSet>
#include <QtConcurrent>
#include <QFutureWatcher>
#include "../Services/ImageService.h"
#include "../Models/Image.h"
#include "../Algorithms/ImageProcessor.h"

class MainWindowController : public QObject {
    Q_OBJECT

public:
    
    explicit MainWindowController(ImageService* imageService, QObject* parent = nullptr);

    void fetchImagesAsync();
    void addImageAsync(const Image& image);
    void updateImageAsync(int id, const Image& image);
    void deleteImageAsync(int id);
    void calculateHistogramAsync(const QImage& image, const QString& channel, const QString& imageIdentifier);

signals:
    
    void imagesFetched(const QList<Image>& images);
    void imageAdded(const Image& image);
    void imageUpdated(int id);
    void imageDeleted(int id);
    void histogramCalculated(const QString& imageIdentifier, const QString& channel, const QVector<int>& histogram);
    void operationFailed(const QString& error);

private:
    
    ImageService* imageService;
    QMap<QString, QMap<QString, QVector<int>>> histogramCache; // Caching histograms
    QSet<QString> runningCalculations; // Tracking running calculations
    QMap<QString, QFutureWatcher<QVector<int>>*> histogramWatchers; // For handling asynchronous operations
};

#endif // MAINWINDOWCONTROLLER_H
