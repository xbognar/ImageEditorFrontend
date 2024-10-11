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

    imageViewer->setAlignment(Qt::AlignCenter);
    imageViewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

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
    connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(controller, &MainWindowController::imagesFetched, this, &MainWindow::onImagesFetched);
    connect(controller, &MainWindowController::imageAdded, this, &MainWindow::onImageAdded);
    connect(controller, &MainWindowController::imageUpdated, this, &MainWindow::onImageUpdated);
    connect(controller, &MainWindowController::imageDeleted, this, &MainWindow::onImageDeleted);
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

    updateImageDisplay();
}

void MainWindow::openFile()
{
    static int tempId = -1;

    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Images"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileNames.isEmpty()) {
        for (int i = 0; i < fileNames.size(); ++i) {
            QString fileName = fileNames[i];
            if (fileName.isEmpty() || isImageInList(fileName)) continue;

            Image imageMeta;
            imageMeta.id = tempId--;
            imageMeta.name = QFileInfo(fileName).fileName();
            imageMeta.path = fileName;

            if (i == 0) {
                
                QImage image(fileName);
                if (!image.isNull()) {
                    imageMeta.width = image.width();
                    imageMeta.height = image.height();
                    imageMeta.pixelFormat = "RGBA";

                    QByteArray imageData;
                    QBuffer buffer(&imageData);
                    if (buffer.open(QIODevice::WriteOnly)) {
                        image.save(&buffer, "PNG");
                        buffer.close();
                    }
                    imageMeta.imageData = imageData;

                    currentImage = image;
                    updateImageDisplay(); 
                    loadedImages.insert(imageMeta.path, image);
                }
            }

            images.append(imageMeta);
            addImageToList(imageMeta);

            if (!imageMeta.imageData.isEmpty()) {
                controller->addImageAsync(imageMeta);
            }
        }
    }
}

void MainWindow::addImageToList(const Image& image)
{
    QIcon icon;
    
    if (!image.imageData.isEmpty()) {
        QImage img = QImage::fromData(image.imageData).scaled(50, 50, Qt::KeepAspectRatio);
        icon = QIcon(QPixmap::fromImage(img));
    }
    else {
        icon = QIcon(":/MainWindow/Icons/placeholder.png");
    }
    
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
    QSize adjustedSize = viewerSize - QSize(10, 10);
    QPixmap pixmap = QPixmap::fromImage(image);
    return pixmap.scaled(adjustedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void MainWindow::deleteSelectedImage()
{
    QListWidgetItem* selectedItem = imageList->currentItem();
    if (!selectedItem) return;

    Image selectedImage = selectedItem->data(Qt::UserRole).value<Image>();
    controller->deleteImageAsync(selectedImage.id);

    images.removeOne(selectedImage);
    loadedImages.remove(selectedImage.path);
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
    controller->fetchImagesAsync();
}

void MainWindow::displayImages(const QList<Image>& images)
{
    imageList->clear();
    for (const Image& img : images) {
        addImageToList(img);
    }
}

void MainWindow::loadFirstImage()
{
    if (!images.isEmpty()) {
        QListWidgetItem* firstItem = imageList->item(0);
        onImageSelected(firstItem);
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

void MainWindow::onImageAdded(const Image& image)
{
    for (int i = 0; i < images.size(); ++i) {
        if (images[i].path == image.path) {
            images[i] = image;
            break;
        }
    }

    for (int i = 0; i < imageList->count(); ++i) {
        QListWidgetItem* item = imageList->item(i);
        Image img = item->data(Qt::UserRole).value<Image>();
        if (img.path == image.path) {
            item->setData(Qt::UserRole, QVariant::fromValue(image));
            item->setText(QString("%1 | %2 | %3x%4")
                .arg(image.id).arg(image.name).arg(image.width).arg(image.height));
            break;
        }
    }
}

void MainWindow::onImageUpdated(int id)
{

}

void MainWindow::onImageDeleted(int id)
{
    auto it = std::find_if(images.begin(), images.end(), [id](const Image& img) {
        return img.id == id;
        });
    if (it != images.end()) {
        loadedImages.remove(it->path);
        images.erase(it);
    }

    for (int i = 0; i < imageList->count(); ++i) {
        QListWidgetItem* item = imageList->item(i);
        Image img = item->data(Qt::UserRole).value<Image>();
        if (img.id == id) {
            delete imageList->takeItem(i);
            break;
        }
    }

    if (!images.isEmpty()) {
        onImageSelected(imageList->item(0));
    }
    else {
        imageViewer->clear();
    }
}

void MainWindow::onImageSelected(QListWidgetItem* item)
{
    if (!item) return;

    Image selectedImage = item->data(Qt::UserRole).value<Image>();

    if (loadedImages.contains(selectedImage.path)) {
        
        currentImage = loadedImages[selectedImage.path]; 
        updateImageDisplay();

    }
    else {
        
        QImage image;
        if (!selectedImage.imageData.isEmpty()) {
            
            image = QImage::fromData(selectedImage.imageData);

        }
        else if (!selectedImage.path.isEmpty()) {
            
            image.load(selectedImage.path);
            if (!image.isNull()) {
                selectedImage.width = image.width();
                selectedImage.height = image.height();
                selectedImage.pixelFormat = "RGBA";

                QByteArray imageData;
                QBuffer buffer(&imageData);
                if (buffer.open(QIODevice::WriteOnly)) {
                    image.save(&buffer, "PNG");
                    buffer.close();
                }
                selectedImage.imageData = imageData;

                QIcon icon(QPixmap::fromImage(image).scaled(50, 50, Qt::KeepAspectRatio));
                item->setIcon(icon);

                item->setData(Qt::UserRole, QVariant::fromValue(selectedImage));

                controller->addImageAsync(selectedImage);
            }
        }

        if (!image.isNull()) {
            loadedImages.insert(selectedImage.path, image);
            currentImage = image;
            updateImageDisplay();
        }
    }
}

void MainWindow::updateImageDisplay()
{
    if (currentImage.isNull()) return;

    QPixmap pixmap = QPixmap::fromImage(currentImage);
    QSize viewerSize = imageViewer->size();

    QPixmap scaledPixmap = pixmap.scaled(viewerSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageViewer->setPixmap(scaledPixmap);
}

