#include "Image.h"

Image::Image() : id(0), width(0), height(0), pixelFormat("RGBA"), path("") {}  

Image::Image(int id, const QString& name, const QByteArray& imageData, int width, int height, const QString& pixelFormat, const QString& path)
    : id(id), name(name), imageData(imageData), width(width), height(height), pixelFormat(pixelFormat), path(path) {} 
