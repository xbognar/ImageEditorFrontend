#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QVector>
#include <QString>

class ImageProcessor {
public:

    static QVector<int> calculateHistogram(const QImage& image, const QString& channel);

private:
    static QVector<int> calculateChannelHistogram(const QImage& image, int channelIndex);
};

#endif // IMAGEPROCESSOR_H
