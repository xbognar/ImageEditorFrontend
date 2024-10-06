#include "MainWindowController.h"

MainWindowController::MainWindowController(ImageService* service, QObject* parent)
    : QObject(parent), imageService(service) {}

// Fetch images asynchronously
void MainWindowController::fetchImages() {
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

// Fetch a single image asynchronously
void MainWindowController::fetchImageById(int id) {
    QFuture<Image> future = QtConcurrent::run([this, id]() {
        return imageService->getImageById(id);
        });

    QFutureWatcher<Image>* watcher = new QFutureWatcher<Image>(this);
    connect(watcher, &QFutureWatcher<Image>::finished, [this, watcher]() {
        Image image = watcher->result();
        emit imageFetched(image);
        watcher->deleteLater();
        });

    watcher->setFuture(future);
}

// Add a new image asynchronously
void MainWindowController::addNewImage(const Image& image) {
    QFuture<void> future = QtConcurrent::run([this, image]() {
        imageService->addImage(image);
        });

    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, [this, watcher]() {
        emit imageAdded();
        watcher->deleteLater();
        });

    watcher->setFuture(future);
}

// Update an existing image asynchronously
void MainWindowController::updateImage(int id, const Image& image) {
    QFuture<void> future = QtConcurrent::run([this, id, image]() {
        imageService->updateImage(id, image);
        });

    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, [this, watcher]() {
        emit imageUpdated();
        watcher->deleteLater();
        });

    watcher->setFuture(future);
}

// Delete an image asynchronously
void MainWindowController::deleteImage(int id) {
    QFuture<void> future = QtConcurrent::run([this, id]() {
        imageService->deleteImage(id);
        });

    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, [this, watcher]() {
        emit imageDeleted();
        watcher->deleteLater();
        });

    watcher->setFuture(future);
}
