#include "MainWindowController.h"
#include "../Algorithms/OilPaintingAlgorithm.h"
#include "../Algorithms/GrayscaleAlgorithm.h"
#include "../Algorithms/DramaticAlgorithm.h"
#include "../Algorithms/WarmAlgorithm.h"
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QCryptographicHash>
#include <QDebug>

MainWindowController::MainWindowController(ImageService* service, QObject* parent)
    : QObject(parent), imageService(service)
{
    filterCache.clear();
}

void MainWindowController::fetchImagesAsync() {
    
    QFuture<QList<Image>> future = QtConcurrent::run([this]() {
        return imageService->getAllImages();
        });

    QFutureWatcher<QList<Image>>* watcher = new QFutureWatcher<QList<Image>>(this);
    connect(watcher, &QFutureWatcher<QList<Image>>::finished, [this, watcher]() {
        QList<Image> images = watcher->result();
        emit imagesFetched(images);
        watcher->deleteLater();
        });

    watcher->setFuture(future);
}

void MainWindowController::addImageAsync(const Image& image) {
    
    QFuture<Image> future = QtConcurrent::run([this, image]() {
        return imageService->addImage(image);
        });

    QFutureWatcher<Image>* watcher = new QFutureWatcher<Image>(this);
    connect(watcher, &QFutureWatcher<Image>::finished, [this, watcher]() {
        Image newImage = watcher->result();
        emit imageAdded(newImage);
        watcher->deleteLater();
        });

    watcher->setFuture(future);
}

void MainWindowController::updateImageAsync(int id, const Image& image) {
    
    QFuture<void> future = QtConcurrent::run([this, id, image]() {
        imageService->updateImage(id, image);
        });

    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, [this, watcher, id]() {
        emit imageUpdated(id);
        watcher->deleteLater();
        });

    watcher->setFuture(future);
}

void MainWindowController::deleteImageAsync(int id) {
    
    QFuture<void> future = QtConcurrent::run([this, id]() {
        imageService->deleteImage(id);
        });

    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, [this, watcher, id]() {
        emit imageDeleted(id);
        watcher->deleteLater();
        });

    watcher->setFuture(future);
}

void MainWindowController::calculateHistogramAsync(const QImage& image, const QString& channel, const QString& imageIdentifier) {
    
    if (histogramCache.contains(imageIdentifier) && histogramCache[imageIdentifier].contains(channel)) {

        emit histogramCalculated(imageIdentifier, channel, histogramCache[imageIdentifier][channel]);
        return;
    }

    QString calcKey = imageIdentifier + channel;
    if (runningCalculations.contains(calcKey)) {
        return;
    }

    runningCalculations.insert(calcKey);

    QFuture<QVector<int>> future = QtConcurrent::run(&ImageProcessor::calculateHistogram, image, channel);
    QFutureWatcher<QVector<int>>* watcher = new QFutureWatcher<QVector<int>>(this);
    histogramWatchers[calcKey] = watcher;

    connect(watcher, &QFutureWatcher<QVector<int>>::finished, this, [this, watcher, imageIdentifier, channel, calcKey]() {
        
        QVector<int> histogram = watcher->result();

        histogramCache[imageIdentifier][channel] = histogram;
        runningCalculations.remove(calcKey);
        watcher->deleteLater();
        histogramWatchers.remove(calcKey);

        emit histogramCalculated(imageIdentifier, channel, histogram);
        });

    watcher->setFuture(future);
}

void MainWindowController::applyFilter(const QImage& image, FilterType filterType)
{
    QString cacheKey = generateCacheKey(image, filterType);

    if (filterCache.contains(cacheKey)) {
        emit filterApplied(filterCache[cacheKey], filterType);
        return;
    }

    QFuture<QImage> future;
    switch (filterType) {
    case OilPainting:
        future = QtConcurrent::run([=]() { return applyOilPaintingFilter(image); });
        break;
    case Grayscale:
        future = QtConcurrent::run([=]() { return applyGrayscaleFilter(image); });
        break;
    case Dramatic:
        future = QtConcurrent::run([=]() { return applyDramaticFilter(image); });
        break;
    case Warm:
        future = QtConcurrent::run([=]() { return applyWarmFilter(image); });
        break;
    default:
        qDebug() << "Unknown filter type";
        return;
    }

    QFutureWatcher<QImage>* watcher = new QFutureWatcher<QImage>(this);
    connect(watcher, &QFutureWatcher<QImage>::finished, this, [=]() {
        QImage result = watcher->result();
        filterCache[cacheKey] = result;
        emit filterApplied(result, filterType);
        watcher->deleteLater();
        });

    watcher->setFuture(future);
}

QImage MainWindowController::applyOilPaintingFilter(const QImage& image)
{
    OilPaintingAlgorithm algorithm;
    return algorithm.process(image);
}

QImage MainWindowController::applyGrayscaleFilter(const QImage& image)
{
    GrayscaleAlgorithm algorithm;
    return algorithm.process(image);
}

QImage MainWindowController::applyDramaticFilter(const QImage& image)
{
    DramaticAlgorithm algorithm;
    return algorithm.process(image);
}

QImage MainWindowController::applyWarmFilter(const QImage& image)
{
    WarmAlgorithm algorithm;
    return algorithm.process(image);
}

QString MainWindowController::generateCacheKey(const QImage& image, FilterType filterType)
{
    QByteArray imageData((const char*)image.bits(), image.sizeInBytes());
    QByteArray hashData = QCryptographicHash::hash(imageData, QCryptographicHash::Md5);
    return hashData.toHex() + "_" + QString::number(filterType);
}