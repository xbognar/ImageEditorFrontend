
#include "GrayscaleAlgorithm.h"
#include <QColor>

QImage GrayscaleAlgorithm::process(const QImage& image)
{
    QImage outputImage = image.convertToFormat(QImage::Format_RGB32);
    int width = outputImage.width();
    int height = outputImage.height();

    for (int y = 0; y < height; y++) {
        QRgb* scanLine = reinterpret_cast<QRgb*>(outputImage.scanLine(y));
        for (int x = 0; x < width; x++) {
            QColor color = QColor::fromRgb(scanLine[x]);
            int grayValue = qGray(color.rgb());
            scanLine[x] = qRgb(grayValue, grayValue, grayValue);
        }
    }

    return outputImage;
}
