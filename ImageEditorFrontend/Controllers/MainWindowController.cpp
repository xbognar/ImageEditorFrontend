#include "MainWindowController.h"
#include "../Algorithms/OilPaintingAlgorithm.h"
#include "../Algorithms/GrayscaleAlgorithm.h"
#include "../Algorithms/DramaticAlgorithm.h"
#include "../Algorithms/WarmAlgorithm.h"
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <QCryptographicHash>
#include <QDebug>

/**
 * @brief Constructs the MainWindowController object.
 * @param service The ImageService instance for image operations.
 * @param parent The parent QObject.
 */
MainWindowController::MainWindowController(ImageService* service, QObject* parent)
    : QObject(parent), imageService(service)
{
    filterCache.clear();
}

/**
 * @brief Fetches images asynchronously from the image service.
 */
void MainWindowController::fetchImagesAsync()
{
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

/**
 * @brief Adds an image asynchronously to the image service.
 * @param image The image to add.
 */
void MainWindowController::addImageAsync(const Image& image)
{
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

/**
 * @brief Updates an image asynchronously in the image service.
 * @param id The ID of the image to update.
 * @param image The updated image data.
 */
void MainWindowController::updateImageAsync(int id, const Image& image)
{
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

/**
 * @brief Deletes an image asynchronously from the image service.
 * @param id The ID of the image to delete.
 */
void MainWindowController::deleteImageAsync(int id)
{
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

/**
 * @brief Calculates the histogram of an image asynchronously.
 * @param image The image to process.
 * @param channel The color channel ("red", "green", "blue").
 * @param imageIdentifier A unique identifier for the image.
 */
void MainWindowController::calculateHistogramAsync(const QImage& image, const QString& channel, const QString& imageIdentifier)
{
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

/**
 * @brief Applies a filter to an image asynchronously.
 * @param image The image to filter.
 * @param filterType The type of filter to apply.
 */
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

/**
 * @brief Applies the oil painting filter to an image.
 * @param image The image to filter.
 * @return The filtered image.
 */
QImage MainWindowController::applyOilPaintingFilter(const QImage& image)
{
    OilPaintingAlgorithm algorithm;
    return algorithm.process(image);
}

/**
 * @brief Applies the grayscale filter to an image.
 * @param image The image to filter.
 * @return The filtered image.
 */
QImage MainWindowController::applyGrayscaleFilter(const QImage& image)
{
    GrayscaleAlgorithm algorithm;
    return algorithm.process(image);
}

/**
 * @brief Applies the dramatic filter to an image.
 * @param image The image to filter.
 * @return The filtered image.
 */
QImage MainWindowController::applyDramaticFilter(const QImage& image)
{
    DramaticAlgorithm algorithm;
    return algorithm.process(image);
}

/**
 * @brief Applies the warm filter to an image.
 * @param image The image to filter.
 * @return The filtered image.
 */
QImage MainWindowController::applyWarmFilter(const QImage& image)
{
    WarmAlgorithm algorithm;
    return algorithm.process(image);
}

/**
 * @brief Generates a unique cache key for the image and filter type.
 * @param image The image to generate the key for.
 * @param filterType The type of filter.
 * @return A unique QString key.
 */
QString MainWindowController::generateCacheKey(const QImage& image, FilterType filterType)
{
    QByteArray imageData((const char*)image.bits(), image.sizeInBytes());
    QByteArray hashData = QCryptographicHash::hash(imageData, QCryptographicHash::Md5);
    return hashData.toHex() + "_" + QString::number(filterType);
}
