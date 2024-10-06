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
};

#endif // IMAGE_H
