#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <QObject>
#include <QList>
#include <QtConcurrent>
#include "Models/Image.h"
#include "Services/ImageService.h"

class MainWindowController : public QObject {
    Q_OBJECT

public:
    explicit MainWindowController(ImageService* imageService, QObject* parent = nullptr);

    void fetchImages();
    void fetchImageById(int id);
    void addNewImage(const Image& image);
    void updateImage(int id, const Image& image);
    void deleteImage(int id);

signals:
    void imagesFetched(const QList<Image>& images);
    void imageFetched(const Image& image);
    void imageAdded();
    void imageUpdated();
    void imageDeleted();

private:
    ImageService* imageService;
};

#endif // MAINWINDOWCONTROLLER_H
