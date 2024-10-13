#ifndef IMAGE_H
#define IMAGE_H

#include <QString>
#include <QByteArray>

class Image {
public:
    int id;
    QString name;
    QByteArray imageData;
    int width;
    int height;
    QString pixelFormat;
    QString path;

    Image();
    Image(int id, const QString& name, const QByteArray& imageData, int width, int height, const QString& pixelFormat, const QString& path);

    bool operator==(const Image& other) const {
        return this->id == other.id &&
            this->name == other.name &&
            this->width == other.width &&
            this->height == other.height &&
            this->pixelFormat == other.pixelFormat &&
            this->path == other.path &&
            this->imageData == other.imageData;
    }

};

#endif 
