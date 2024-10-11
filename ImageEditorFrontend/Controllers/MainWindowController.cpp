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
