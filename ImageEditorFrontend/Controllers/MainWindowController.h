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

    enum FilterType {
        NoFilter = -1,
        OilPainting = 0,
        Grayscale = 1,
        Dramatic = 2,
        Warm = 3
    };
    
    explicit MainWindowController(ImageService* imageService, QObject* parent = nullptr);
    void fetchImagesAsync();
    void addImageAsync(const Image& image);
    void updateImageAsync(int id, const Image& image);
    void deleteImageAsync(int id);
    void calculateHistogramAsync(const QImage& image, const QString& channel, const QString& imageIdentifier);
    void applyFilter(const QImage& image, FilterType filterType);

signals:
    
    void filterApplied(const QImage& filteredImage, MainWindowController::FilterType filterType);
    void imagesFetched(const QList<Image>& images);
    void imageAdded(const Image& image);
    void imageUpdated(int id);
    void imageDeleted(int id);
    void histogramCalculated(const QString& imageIdentifier, const QString& channel, const QVector<int>& histogram);
    void operationFailed(const QString& error);

private:
    
    ImageService* imageService;
    QMap<QString, QMap<QString, QVector<int>>> histogramCache;
    QSet<QString> runningCalculations;
    QMap<QString, QFutureWatcher<QVector<int>>*> histogramWatchers;
    QMap<QString, QImage> filterCache;
    QImage applyOilPaintingFilter(const QImage& image);
    QImage applyGrayscaleFilter(const QImage& image);
    QImage applyDramaticFilter(const QImage& image);
    QImage applyWarmFilter(const QImage& image);
    QString generateCacheKey(const QImage& image, FilterType filterType);
};

#endif
