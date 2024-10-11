#include "MainWindow.h"
#include <QFile>
#include <QIcon>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QBuffer>
#include <algorithm>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), firstResizeEvent(true), imageService(new ImageService(this)),
    controller(new MainWindowController(imageService, this))
{
    ui.setupUi(this);

    imageViewer = ui.imageViewer;
    imageList = ui.imageList;
    histogramViewer = ui.histogramViewer;

    cropButton = ui.cropButton;
    rotateRightButton = ui.rotateRightButton;
    rotateLeftButton = ui.rotateLeftButton;
    flipButton = ui.flipButton;
    folderButton = ui.openButton;
    deleteButton = ui.deleteButton;

    filter1Button = ui.filter1Button;
    filter2Button = ui.filter2Button;
    filter3Button = ui.filter3Button;
    filter4Button = ui.filter4Button;
    redRGBButton = ui.redRGBButton;
    blueRGBButton = ui.blueRGBButton;
    greenRGBButton = ui.greenRGBButton;

    filter1Label = ui.filter1Label;
    filter2Label = ui.filter2Label;
    filter3Label = ui.filter3Label;
    filter4Label = ui.filter4Label;

    applyStylesheet();
    setupHistogram();

    connect(folderButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedImage);
    connect(ui.actionExit, &QAction::triggered, this, &MainWindow::exitApp);
    connect(controller, &MainWindowController::imagesFetched, this, &MainWindow::onImagesFetched);
    connect(controller, &MainWindowController::imageAdded, this, &MainWindow::onImageAdded);
    connect(imageList, &QListWidget::itemClicked, this, &MainWindow::onImageSelected);

    loadImages();
}

MainWindow::~MainWindow()
{
    delete histogramImage;
    delete imageService;
    delete controller;
}

void MainWindow::applyStylesheet()
{
    QFile file(":/MainWindow/Styles/Styles.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }
    else {
        qDebug() << "Failed to load stylesheet!";
    }
}

void MainWindow::setupHistogram()
{
    histogramViewer->setFixedSize(340, 251);
    histogramViewer->setStyleSheet("background-color: #4b4b4b;");

    histogramImage = new QImage(340, 251, QImage::Format_RGB32);
    histogramImage->fill(QColor("#4b4b4b"));

    QPainter painter(histogramImage);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(0, 0, histogramImage->width(), histogramImage->height(), 40, 40);
    painter.setClipPath(path);

    QPen axisPen(Qt::white, 1.5);
    painter.setPen(axisPen);

    int xAxisStartX = 30;
    int xAxisStartY = histogramImage->height() - 30;
    int yAxisStartX = 30;
    int yAxisStartY = 30;
    painter.drawLine(xAxisStartX, xAxisStartY, 315, xAxisStartY);
    painter.drawLine(xAxisStartX, xAxisStartY, xAxisStartX, yAxisStartY);

    QPen arrowPen(Qt::white, 1.5);
    painter.setPen(arrowPen);

    painter.drawLine(315, xAxisStartY, 305, xAxisStartY - 5);
    painter.drawLine(315, xAxisStartY, 305, xAxisStartY + 5);

    painter.drawLine(xAxisStartX, yAxisStartY, xAxisStartX - 5, yAxisStartY + 10);
    painter.drawLine(xAxisStartX, yAxisStartY, xAxisStartX + 5, yAxisStartY + 10);

    histogramViewer->setPixmap(QPixmap::fromImage(*histogramImage));
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    if (firstResizeEvent) {
        firstResizeEvent = false;
        return;
    }

    QSize newSize = event->size();

    const int padding = 20;
    const int leftPadding = 20;
    const int buttonWidth = 35;
    const int buttonPadding = 300;
    const int bottomPadding = 30;
    const int columnWidth = 250;
    const int histogramSize = 270;
    const int buttonGap = 30;
    const int baseGapBetweenRGBAndFilters = 150;

    if (newSize.width() < 800) {
        this->resize(800, newSize.height());
        return;
    }
    else if (newSize.height() < 590) {
        this->resize(newSize.width(), 590);
        return;
    }
    else if (newSize.width() < 800 || newSize.height() < 590) {
        this->resize(800, 590);
        return;
    }

    histogramViewer->move(leftPadding - 10, newSize.height() - histogramSize - bottomPadding - 20);
    histogramViewer->resize(histogramSize, histogramSize);

    imageViewer->move(columnWidth + (5.5 * padding), padding);
    imageViewer->setFixedSize(newSize.width() - imageViewer->x() - (5 * padding) + 15, newSize.height() - bottomPadding - (3 * padding));

    int fileListHeight = histogramViewer->y() - (2 * padding) + 10;
    imageList->move(leftPadding, padding);
    imageList->setFixedSize(columnWidth + 10, fileListHeight);

    int proportionalLeftButtonSpacing = ((fileListHeight + 30) - (35 * 6)) / 7;
    folderButton->move(buttonPadding, proportionalLeftButtonSpacing + 10);
    deleteButton->move(buttonPadding, folderButton->geometry().bottom() + proportionalLeftButtonSpacing);
    cropButton->move(buttonPadding, deleteButton->geometry().bottom() + proportionalLeftButtonSpacing);
    rotateRightButton->move(buttonPadding, cropButton->geometry().bottom() + proportionalLeftButtonSpacing);
    rotateLeftButton->move(buttonPadding, rotateRightButton->geometry().bottom() + proportionalLeftButtonSpacing);
    flipButton->move(buttonPadding, rotateLeftButton->geometry().bottom() + proportionalLeftButtonSpacing);

    int rightButtonXOffset = newSize.width() - buttonWidth - padding - 15;
    filter1Button->move(rightButtonXOffset, buttonGap + 20);
    filter2Button->move(rightButtonXOffset, filter1Button->geometry().bottom() + buttonGap);
    filter3Button->move(rightButtonXOffset, filter2Button->geometry().bottom() + buttonGap);
    filter4Button->move(rightButtonXOffset, filter3Button->geometry().bottom() + buttonGap);

    int dynamicGapBetweenFilterAndRGB = baseGapBetweenRGBAndFilters + (newSize.height() - 700);
    redRGBButton->move(rightButtonXOffset, filter4Button->geometry().bottom() + dynamicGapBetweenFilterAndRGB);
    blueRGBButton->move(rightButtonXOffset, redRGBButton->geometry().bottom() + buttonGap - 20);
    greenRGBButton->move(rightButtonXOffset, blueRGBButton->geometry().bottom() + buttonGap - 20);

    filter1Label->move(filter1Button->geometry().left() + (filter1Button->width() / 2) - (filter1Label->width() / 2), filter1Button->geometry().bottom() + 5);
    filter2Label->move(filter2Button->geometry().left() + (filter2Button->width() / 2) - (filter2Label->width() / 2), filter2Button->geometry().bottom() + 5);
    filter3Label->move(filter3Button->geometry().left() + (filter3Button->width() / 2) - (filter3Label->width() / 2), filter3Button->geometry().bottom() + 5);
    filter4Label->move(filter4Button->geometry().left() + (filter4Button->width() / 2) - (filter4Label->width() / 2), filter4Button->geometry().bottom() + 5);

    QMainWindow::resizeEvent(event);
}

void MainWindow::openFile()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Images"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileNames.isEmpty()) {
        for (int i = 0; i < fileNames.size(); ++i) {
            QString fileName = fileNames[i];
            if (fileName.isEmpty() || isImageInList(fileName)) continue;

            if (i == 0) {
                
                QImage image(fileName);
                if (!image.isNull()) {
                    QByteArray imageData;
                    QBuffer buffer(&imageData);
                    if (buffer.open(QIODevice::WriteOnly)) {
                        image.save(&buffer, "PNG");
                        buffer.close();
                    }
                    Image newImage;
                    newImage.name = QFileInfo(fileName).fileName();
                    newImage.imageData = imageData;
                    newImage.width = image.width();
                    newImage.height = image.height();
                    newImage.pixelFormat = "RGBA";
                    newImage.path = fileName;

                    imageViewer->setPixmap(scaleImageToViewer(image));
                    controller->addNewImage(newImage);
                    images.append(newImage);
                    addImageToList(newImage, true);
                }
            }
            else {
                
                Image imageMeta;
                imageMeta.name = QFileInfo(fileName).fileName();
                imageMeta.path = fileName;
                images.append(imageMeta);
                addImageToList(imageMeta, false);
            }
        }
    }
}

void MainWindow::addImageToList(const Image& image, bool loadThumbnail)
{
    QIcon icon = loadThumbnail && !image.imageData.isEmpty()
        ? QIcon(QPixmap::fromImage(QImage::fromData(image.imageData).scaled(50, 50, Qt::KeepAspectRatio)))
        : QIcon("C:/Users/matth/JobPractice/ImageEditorFrontend/ImageEditorFrontend/Resources/Icons/placeholder.png");
    QListWidgetItem* item = new QListWidgetItem(icon, QString("%1 | %2 | %3x%4")
        .arg(image.id).arg(image.name).arg(image.width).arg(image.height));
    item->setData(Qt::UserRole, QVariant::fromValue(image));
    imageList->addItem(item);
}

bool MainWindow::isImageInList(const QString& path)
{
    return std::any_of(images.begin(), images.end(), [&path](const Image& img) {
        return img.path == path;
        });
}

QPixmap MainWindow::scaleImageToViewer(const QImage& image)
{
    QSize viewerSize = imageViewer->size();
    QPixmap pixmap = QPixmap::fromImage(image);
    return pixmap.scaled(viewerSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void MainWindow::deleteSelectedImage()
{
    QListWidgetItem* selectedItem = imageList->currentItem();
    if (!selectedItem) return;

    Image selectedImage = selectedItem->data(Qt::UserRole).value<Image>();
    controller->deleteImage(selectedImage.id);
    images.removeOne(selectedImage);
    delete selectedItem;

    if (!images.isEmpty()) {
        onImageSelected(imageList->item(0));
    }
    else {
        imageViewer->clear();
    }
}

void MainWindow::loadImages()
{
    controller->fetchImages();
}

void MainWindow::displayImages(const QList<Image>& images)
{
    imageList->clear();
    for (const Image& img : images) {
        addImageToList(img, !img.imageData.isEmpty());
    }
}

void MainWindow::loadFirstImage()
{
    if (!images.isEmpty()) {
        Image firstImage = images.first();
        QImage image(firstImage.path);
        if (!image.isNull()) {
            imageViewer->setPixmap(scaleImageToViewer(image));
        }
    }
}

void MainWindow::exitApp()
{
    QApplication::quit();
}

void MainWindow::onImagesFetched(const QList<Image>& fetchedImages)
{
    images = fetchedImages;
    displayImages(images);
    loadFirstImage();
}

void MainWindow::onImageAdded()
{
    loadImages();
}

void MainWindow::onImageSelected(QListWidgetItem* item)
{
    // Get a copy of the Image object from the QVariant
    Image selectedImage = item->data(Qt::UserRole).value<Image>();

    // Load binary data only if not already loaded
    if (selectedImage.imageData.isEmpty()) {
        QImage image(selectedImage.path);
        if (!image.isNull()) {
            QByteArray imageData;
            QBuffer buffer(&imageData);
            if (buffer.open(QIODevice::WriteOnly)) {
                image.save(&buffer, "PNG");
                buffer.close();
            }
            selectedImage.imageData = imageData;

            // Update the item with the new thumbnail
            item->setIcon(QIcon(QPixmap::fromImage(image).scaled(50, 50, Qt::KeepAspectRatio)));

            // Store the updated Image object back in the QListWidgetItem
            item->setData(Qt::UserRole, QVariant::fromValue(selectedImage));
        }
    }

    // Display the image in the viewer
    imageViewer->setPixmap(scaleImageToViewer(QImage::fromData(selectedImage.imageData)));
}
