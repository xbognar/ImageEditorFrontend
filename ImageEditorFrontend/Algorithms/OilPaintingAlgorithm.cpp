#include "OilPaintingAlgorithm.h"
#include <QColor>
#include <algorithm>

/**
 * @brief Applies an oil painting effect to the given image.
 * @param image The input QImage.
 * @return The processed QImage with the oil painting effect applied.
 */
QImage OilPaintingAlgorithm::process(const QImage& image)
{
    QImage outputImage = image.convertToFormat(QImage::Format_RGB32);
    int radius = 3;
    int intensityLevels = 20;
    int width = image.width();
    int height = image.height();

    for (int y = radius; y < height - radius; y++) {
        for (int x = radius; x < width - radius; x++) {

            QVector<int> intensityCount(intensityLevels, 0);
            QVector<int> sumR(intensityLevels, 0);
            QVector<int> sumG(intensityLevels, 0);
            QVector<int> sumB(intensityLevels, 0);

            for (int dy = -radius; dy <= radius; dy++) {
                for (int dx = -radius; dx <= radius; dx++) {
                    QColor color = QColor::fromRgb(image.pixel(x + dx, y + dy));
                    int intensity = (color.red() + color.green() + color.blue()) / 3;
                    intensity = (intensity * intensityLevels) / 256;
                    intensity = qBound(0, intensity, intensityLevels - 1);

                    intensityCount[intensity]++;
                    sumR[intensity] += color.red();
                    sumG[intensity] += color.green();
                    sumB[intensity] += color.blue();
                }
            }

            int maxCount = 0;
            int maxIndex = 0;
            for (int i = 0; i < intensityLevels; i++) {
                if (intensityCount[i] > maxCount) {
                    maxCount = intensityCount[i];
                    maxIndex = i;
                }
            }

            int r = sumR[maxIndex] / maxCount;
            int g = sumG[maxIndex] / maxCount;
            int b = sumB[maxIndex] / maxCount;

            outputImage.setPixelColor(x, y, QColor(r, g, b));
        }
    }

    return outputImage;
}
