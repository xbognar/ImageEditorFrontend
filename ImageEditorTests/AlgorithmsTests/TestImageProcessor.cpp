#include <QtTest/QtTest>
#include <QImage>
#include <QVector>
#include "../../ImageEditorFrontend/Algorithms/ImageProcessor.h"
#include "../../ImageEditorFrontend/Algorithms/ImageProcessor.cpp"

class TestImageProcessor : public QObject
{
    Q_OBJECT

private slots:
    
    void testRedHistogram_NotEmpty();
    void testRedHistogram_CorrectSize();
    void testRedHistogram_CorrectValueAt255();
    void testRedHistogram_ZeroAtValue0();
    void testRedHistogram_NoNonRedPixels();

    void testGreenHistogram_NotEmpty();
    void testGreenHistogram_CorrectSize();
    void testGreenHistogram_CorrectValueAt255();
    void testGreenHistogram_ZeroAtValue0();
    void testGreenHistogram_NoNonGreenPixels();

    void testBlueHistogram_NotEmpty();
    void testBlueHistogram_CorrectSize();
    void testBlueHistogram_CorrectValueAt255();
    void testBlueHistogram_ZeroAtValue0();
    void testBlueHistogram_NoNonBluePixels();

};


void TestImageProcessor::testRedHistogram_NotEmpty()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::red);

    QVector<int> redHistogram = ImageProcessor::calculateHistogram(testImage, "red");

    QVERIFY(!redHistogram.isEmpty());
}

void TestImageProcessor::testRedHistogram_CorrectSize()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::red);

    QVector<int> redHistogram = ImageProcessor::calculateHistogram(testImage, "red");

    QCOMPARE(redHistogram.size(), 256);
}

void TestImageProcessor::testRedHistogram_CorrectValueAt255()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::red);

    QVector<int> redHistogram = ImageProcessor::calculateHistogram(testImage, "red");

    QCOMPARE(redHistogram[255], 10000);
}

void TestImageProcessor::testRedHistogram_ZeroAtValue0()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::red);

    QVector<int> redHistogram = ImageProcessor::calculateHistogram(testImage, "red");

    QCOMPARE(redHistogram[0], 0);
}

void TestImageProcessor::testRedHistogram_NoNonRedPixels()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::red);

    QVector<int> greenHistogram = ImageProcessor::calculateHistogram(testImage, "green");
    QVector<int> blueHistogram = ImageProcessor::calculateHistogram(testImage, "blue");

    QCOMPARE(greenHistogram[255], 0);
    QCOMPARE(blueHistogram[255], 0);
}

void TestImageProcessor::testGreenHistogram_NotEmpty()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::green);

    QVector<int> greenHistogram = ImageProcessor::calculateHistogram(testImage, "green");

    QVERIFY(!greenHistogram.isEmpty());
}

void TestImageProcessor::testGreenHistogram_CorrectSize()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::green);

    QVector<int> greenHistogram = ImageProcessor::calculateHistogram(testImage, "green");

    QCOMPARE(greenHistogram.size(), 256);
}

void TestImageProcessor::testGreenHistogram_CorrectValueAt255()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::green);

    QVector<int> greenHistogram = ImageProcessor::calculateHistogram(testImage, "green");

    QCOMPARE(greenHistogram[255], 10000);
}

void TestImageProcessor::testGreenHistogram_ZeroAtValue0()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::green);

    QVector<int> greenHistogram = ImageProcessor::calculateHistogram(testImage, "green");

    QCOMPARE(greenHistogram[0], 0);
}

void TestImageProcessor::testGreenHistogram_NoNonGreenPixels()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::green);

    QVector<int> redHistogram = ImageProcessor::calculateHistogram(testImage, "red");
    QVector<int> blueHistogram = ImageProcessor::calculateHistogram(testImage, "blue");

    QCOMPARE(redHistogram[255], 0);
    QCOMPARE(blueHistogram[255], 0);
}

void TestImageProcessor::testBlueHistogram_NotEmpty()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::blue);

    QVector<int> blueHistogram = ImageProcessor::calculateHistogram(testImage, "blue");

    QVERIFY(!blueHistogram.isEmpty());
}

void TestImageProcessor::testBlueHistogram_CorrectSize()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::blue);

    QVector<int> blueHistogram = ImageProcessor::calculateHistogram(testImage, "blue");

    QCOMPARE(blueHistogram.size(), 256);
}

void TestImageProcessor::testBlueHistogram_CorrectValueAt255()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::blue);

    QVector<int> blueHistogram = ImageProcessor::calculateHistogram(testImage, "blue");

    QCOMPARE(blueHistogram[255], 10000);
}

void TestImageProcessor::testBlueHistogram_ZeroAtValue0()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::blue);

    QVector<int> blueHistogram = ImageProcessor::calculateHistogram(testImage, "blue");

    QCOMPARE(blueHistogram[0], 0);
}

void TestImageProcessor::testBlueHistogram_NoNonBluePixels()
{

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::blue);

    QVector<int> redHistogram = ImageProcessor::calculateHistogram(testImage, "red");
    QVector<int> greenHistogram = ImageProcessor::calculateHistogram(testImage, "green");

    QCOMPARE(redHistogram[255], 0);
    QCOMPARE(greenHistogram[255], 0);
}

QTEST_MAIN(TestImageProcessor)
#include "TestImageProcessor.moc"
