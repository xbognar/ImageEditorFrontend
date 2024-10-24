#ifndef IMAGESERVICE_H
#define IMAGESERVICE_H

#include <QObject>
#include <QList>
#include "../Models/Image.h"
#include "BaseService.h"

class ImageService : public BaseService {
    Q_OBJECT

public:
    explicit ImageService(QObject* parent = nullptr);

    QList<Image> getAllImages();
    Image getImageById(int id);
    Image addImage(const Image& image);
    void updateImage(int id, const Image& image);
    void deleteImage(int id);
};

#endif // IMAGESERVICE_H
