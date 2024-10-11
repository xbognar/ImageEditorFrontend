#ifndef MAINWINDOWCONTROLLER_H
#define MAINWINDOWCONTROLLER_H

#include <QObject>
#include <QList>
#include "Models/Image.h"
#include "Services/ImageService.h"

class MainWindowController : public QObject {
    Q_OBJECT

public:
    explicit MainWindowController(ImageService* imageService, QObject* parent = nullptr);

    void fetchImagesAsync();
    void addImageAsync(const Image& image);
    void updateImageAsync(int id, const Image& image);
    void deleteImageAsync(int id);

signals:
    void imagesFetched(const QList<Image>& images);
    void imageAdded(const Image& image);
    void imageUpdated(int id);
    void imageDeleted(int id);
    void operationFailed(const QString& error);

private:
    ImageService* imageService;
};

#endif // MAINWINDOWCONTROLLER_H
