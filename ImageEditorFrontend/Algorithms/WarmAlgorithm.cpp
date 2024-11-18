
#include "WarmAlgorithm.h"
#include <QColor>

QImage WarmAlgorithm::process(const QImage& image)
{
    QImage outputImage = image.convertToFormat(QImage::Format_RGB32);
    int width = outputImage.width();
    int height = outputImage.height();

    for (int y = 0; y < height; y++) {
        
        QRgb* scanLine = reinterpret_cast<QRgb*>(outputImage.scanLine(y));
        for (int x = 0; x < width; x++) {
            QColor color = QColor::fromRgb(scanLine[x]);
            int r = qBound(0, color.red() + 20, 255);
            int g = qBound(0, color.green() + 10, 255);
            int b = color.blue();
            scanLine[x] = qRgb(r, g, b);
        }
    }

    return outputImage;
}
