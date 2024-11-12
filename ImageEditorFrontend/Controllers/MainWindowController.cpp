#include "MainWindowController.h"
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <QDebug>

MainWindowController::MainWindowController(ImageService* service, QObject* parent)
    : QObject(parent), imageService(service) {}

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

