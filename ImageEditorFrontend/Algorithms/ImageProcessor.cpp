
#include "ImageProcessor.h"
#include <QColor>

QVector<int> ImageProcessor::calculateHistogram(const QImage& image, const QString& channel) {

    int channelIndex = -1;
    if (channel == "red") channelIndex = 0;
    else if (channel == "green") channelIndex = 1;
    else if (channel == "blue") channelIndex = 2;

    return (channelIndex != -1) ? calculateChannelHistogram(image, channelIndex) : QVector<int>();
}

QVector<int> ImageProcessor::calculateChannelHistogram(const QImage& image, int channelIndex) {

    QVector<int> histogram(256, 0);

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {

            QColor color = image.pixelColor(x, y);
            int value = (channelIndex == 0) ? color.red() :
                (channelIndex == 1) ? color.green() : color.blue();
            histogram[value]++;
        }
    }

    return histogram;
}
