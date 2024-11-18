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
#include <QInputDialog>
#include <algorithm>

/**
 * @brief Constructs the MainWindow object and initializes the UI components.
 * @param parent The parent widget, if any.
 */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    firstResizeEvent(true),
    imageService(new ImageService(this)),
    controller(new MainWindowController(imageService, this)),
    isCropping(false),
    isCropMode(false),
    imageOffsetX(0),
    imageOffsetY(0),
    scaledImageSize(QSize()),
    currentFilter(MainWindowController::NoFilter)
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

    filterButtons[filter1Button] = MainWindowController::OilPainting;
    filterButtons[filter2Button] = MainWindowController::Grayscale;
    filterButtons[filter3Button] = MainWindowController::Dramatic;
    filterButtons[filter4Button] = MainWindowController::Warm;

    imageProcessor = new ImageProcessor();
    channelVisibility = { {"red", false}, {"green", false}, {"blue", false} };

    applyStylesheet();
    setupHistogram();

    connect(folderButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteSelectedImage);
    connect(ui.actionExit, &QAction::triggered, this, &MainWindow::exitApp);
    connect(ui.actionOpen, &QAction::triggered, this, &MainWindow::openFile);
    connect(ui.actionSave, &QAction::triggered, this, &MainWindow::saveImage);
    connect(controller, &MainWindowController::imagesFetched, this, &MainWindow::onImagesFetched);
    connect(controller, &MainWindowController::imageAdded, this, &MainWindow::onImageAdded);
    connect(controller, &MainWindowController::imageDeleted, this, &MainWindow::onImageDeleted);
    connect(imageList, &QListWidget::itemClicked, this, &MainWindow::onImageSelected);
    connect(redRGBButton, &QPushButton::clicked, this, [this] { toggleHistogram("red"); });
    connect(greenRGBButton, &QPushButton::clicked, this, [this] { toggleHistogram("green"); });
    connect(blueRGBButton, &QPushButton::clicked, this, [this] { toggleHistogram("blue"); });
    connect(controller, &MainWindowController::histogramCalculated, this, &MainWindow::onHistogramCalculated);

    connect(rotateRightButton, &QPushButton::clicked, this, &MainWindow::rotateImageRight);
    connect(rotateLeftButton, &QPushButton::clicked, this, &MainWindow::rotateImageLeft);
    connect(flipButton, &QPushButton::clicked, this, &MainWindow::flipImage);
    connect(cropButton, &QPushButton::clicked, this, &MainWindow::cropImage);

    connect(controller, &MainWindowController::filterApplied, this, &MainWindow::displayFilteredResult);

    for (auto button : filterButtons.keys()) {
        connect(button, &QPushButton::clicked, this, [=]() {
            onFilterButtonClicked(filterButtons[button]);
            });
    }

    loadImages();
}

/**
 * @brief Destructor for MainWindow. Cleans up allocated resources.
 */
MainWindow::~MainWindow()
{
    delete histogramImage;
    delete imageService;
    delete controller;
}

/**
 * @brief Applies the stylesheet to the application.
 */
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

/**
 * @brief Sets up the histogram viewer with initial settings.
 */
void MainWindow::setupHistogram()
{
    histogramViewer->setFixedSize(370, 270);
    histogramViewer->setStyleSheet("background-color: #f5f5dc;");

    histogramImage = new QImage(370, 270, QImage::Format_RGB32);
    histogramImage->fill(QColor("#f5f5dc"));

    QPainter painter(histogramImage);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(0, 0, histogramImage->width(), histogramImage->height(), 40, 40);
    painter.setClipPath(path);

    QPen axisPen(Qt::black, 1.5);
    painter.setPen(axisPen);

    int xAxisStartX = 30;
    int xAxisStartY = histogramImage->height() - 30;
    int yAxisStartX = 30;
    int yAxisStartY = 30;
    painter.drawLine(xAxisStartX, xAxisStartY, 315, xAxisStartY);
    painter.drawLine(xAxisStartX, xAxisStartY, xAxisStartX, yAxisStartY);

    QPen arrowPen(Qt::black, 1.5);
    painter.setPen(arrowPen);

    painter.drawLine(315, xAxisStartY, 305, xAxisStartY - 5);
    painter.drawLine(315, xAxisStartY, 305, xAxisStartY + 5);

    painter.drawLine(xAxisStartX, yAxisStartY, xAxisStartX - 5, yAxisStartY + 10);
    painter.drawLine(xAxisStartX, yAxisStartY, xAxisStartX + 5, yAxisStartY + 10);

    histogramViewer->setPixmap(QPixmap::fromImage(*histogramImage));
}

/**
 * @brief Handles the resize event to adjust UI components accordingly.
 * @param event The resize event.
 */
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
    const int buttonPadding = 310;
    const int bottomPadding = 30;
    const int columnWidth = 250;
    const int histogramSize = 270;
    const int buttonGap = 25;
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

    histogramViewer->move(leftPadding - 15, newSize.height() - histogramSize - bottomPadding - 35);
    histogramViewer->resize(histogramSize, histogramSize);

    imageViewer->move(columnWidth + (6.7 * padding), padding);
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
    filter1Button->move(rightButtonXOffset, buttonGap + 70);
    filter2Button->move(rightButtonXOffset, filter1Button->geometry().bottom() + buttonGap);
    filter3Button->move(rightButtonXOffset, filter2Button->geometry().bottom() + buttonGap);
    filter4Button->move(rightButtonXOffset, filter3Button->geometry().bottom() + buttonGap);

    int dynamicGapBetweenFilterAndRGB = baseGapBetweenRGBAndFilters + (newSize.height() - 710);
    redRGBButton->move(rightButtonXOffset + 5, filter4Button->geometry().bottom() + dynamicGapBetweenFilterAndRGB);
    blueRGBButton->move(rightButtonXOffset + 5, redRGBButton->geometry().bottom() + buttonGap - 20);
    greenRGBButton->move(rightButtonXOffset + 5, blueRGBButton->geometry().bottom() + buttonGap - 20);

    filter1Label->move(filter1Button->geometry().left() + (filter1Button->width() / 2) - (filter1Label->width() / 2), filter1Button->geometry().bottom() + 5);
    filter2Label->move(filter2Button->geometry().left() + (filter2Button->width() / 2) - (filter2Label->width() / 2), filter2Button->geometry().bottom() + 5);
    filter3Label->move(filter3Button->geometry().left() + (filter3Button->width() / 2) - (filter3Label->width() / 2), filter3Button->geometry().bottom() + 5);
    filter4Label->move(filter4Button->geometry().left() + (filter4Button->width() / 2) - (filter4Label->width() / 2), filter4Button->geometry().bottom() + 5);

    QMainWindow::resizeEvent(event);

    updateImageDisplay();
}

/**
 * @brief Handles the paint event to draw custom UI elements.
 * @param event The paint event.
 */
void MainWindow::paintEvent(QPaintEvent* event)
{
    QMainWindow::paintEvent(event);

    QPainter painter(this);
    drawColumnsAndCircles(painter);
}

/**
 * @brief Draws custom columns and circles on the UI for aesthetic purposes.
 * @param painter The QPainter object used for drawing.
 */
void MainWindow::drawColumnsAndCircles(QPainter& painter)
{
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor("#f5f5dc")));

    int leftColumnWidth = 55;
    int leftColumnX = folderButton->x() - 8;
    int leftColumnY = 45;
    int leftColumnHeight = flipButton->geometry().bottom() - leftColumnY + 10;

    painter.drawRoundedRect(leftColumnX, leftColumnY, leftColumnWidth, leftColumnHeight + 20, 0, 0);

    int circleDiameter = leftColumnWidth;
    int circleX = leftColumnX;
    int circleY = leftColumnY + leftColumnHeight - 10;

    painter.drawEllipse(circleX, circleY, circleDiameter, circleDiameter);

    int rightColumnWidth = 63;
    int rightColumnX = this->width() - rightColumnWidth - 10;
    int rightColumnY = filter1Button->y();
    int rightColumnHeight = this->height() - rightColumnY + 50;

    painter.drawRoundedRect(rightColumnX, rightColumnY, rightColumnWidth, rightColumnHeight, 30, 35);
}

/**
 * @brief Opens a file dialog to select images and loads them into the application.
 */
void MainWindow::openFile()
{
    static int tempId = -1;

    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Images"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileNames.isEmpty()) {
        for (int i = 0; i < fileNames.size(); ++i) {
            QString fileName = fileNames[i];
            if (fileName.isEmpty() || isImageInList(fileName))
                continue;

            Image imageMeta;
            imageMeta.id = tempId--;
            imageMeta.name = QFileInfo(fileName).fileName();
            imageMeta.path = fileName;

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

                if (i == 0) {
                    originalImage = image;
                    currentImage = image;
                    currentFilter = MainWindowController::NoFilter;
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

/**
 * @brief Adds an image to the image list widget.
 * @param image The image metadata to add.
 */
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

/**
 * @brief Checks if an image is already in the image list.
 * @param path The file path of the image.
 * @return True if the image is in the list, false otherwise.
 */
bool MainWindow::isImageInList(const QString& path)
{
    return std::any_of(images.begin(), images.end(), [&path](const Image& img) {
        return img.path == path;
        });
}

/**
 * @brief Scales the given image to fit the image viewer.
 * @param image The image to scale.
 * @return The scaled QPixmap.
 */
QPixmap MainWindow::scaleImageToViewer(const QImage& image)
{
    QSize viewerSize = imageViewer->size();
    QPixmap pixmap = QPixmap::fromImage(image);
    return pixmap.scaled(viewerSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

/**
 * @brief Deletes the currently selected image from the application.
 */
void MainWindow::deleteSelectedImage()
{
    QListWidgetItem* selectedItem = imageList->currentItem();
    if (!selectedItem)
        return;

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

/**
 * @brief Loads images from the image service asynchronously.
 */
void MainWindow::loadImages()
{
    controller->fetchImagesAsync();
}

/**
 * @brief Displays the list of images in the image list widget.
 * @param images The list of images to display.
 */
void MainWindow::displayImages(const QList<Image>& images)
{
    imageList->clear();
    for (const Image& img : images) {
        addImageToList(img);
    }
}

/**
 * @brief Loads the first image in the image list and displays it.
 */
void MainWindow::loadFirstImage()
{
    if (!images.isEmpty()) {
        QListWidgetItem* firstItem = imageList->item(0);
        onImageSelected(firstItem);
    }
}

/**
 * @brief Exits the application.
 */
void MainWindow::exitApp()
{
    QApplication::quit();
}

/**
 * @brief Slot called when images are fetched from the image service.
 * @param fetchedImages The list of images fetched.
 */
void MainWindow::onImagesFetched(const QList<Image>& fetchedImages)
{
    images = fetchedImages;
    displayImages(images);
    loadFirstImage();
}

/**
 * @brief Slot called when an image is added to the image service.
 * @param image The image that was added.
 */
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

/**
 * @brief Slot called when an image is deleted from the image service.
 * @param id The ID of the image that was deleted.
 */
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

/**
 * @brief Slot called when an image is selected from the image list.
 * @param item The list widget item representing the selected image.
 */
void MainWindow::onImageSelected(QListWidgetItem* item)
{
    if (!item)
        return;

    channelVisibility = { {"red", false}, {"green", false}, {"blue", false} };
    updateHistogramDisplay();

    Image selectedImage = item->data(Qt::UserRole).value<Image>();

    if (currentImagePath != selectedImage.path) {
        histogramCache.remove(currentImagePath);
    }

    currentImagePath = selectedImage.path;

    if (loadedImages.contains(selectedImage.path)) {
        originalImage = loadedImages[selectedImage.path];
        currentImage = originalImage;
        currentFilter = MainWindowController::NoFilter;
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
            originalImage = image;
            currentImage = originalImage;
            currentFilter = MainWindowController::NoFilter;
            updateImageDisplay();
        }
    }

    for (const auto& channel : channelVisibility.keys()) {
        if (channelVisibility[channel]) {
            controller->calculateHistogramAsync(currentImage, channel, selectedImage.path);
        }
    }
}

/**
 * @brief Slot called when a histogram calculation is completed.
 * @param imageIdentifier The identifier of the image.
 * @param channel The color channel.
 * @param histogram The calculated histogram data.
 */
void MainWindow::onHistogramCalculated(const QString& imageIdentifier, const QString& channel, const QVector<int>& histogram)
{
    histogramCache[imageIdentifier][channel] = histogram;

    if (currentImagePath == imageIdentifier) {
        updateHistogramDisplay();
    }
}

/**
 * @brief Updates the image display area with the current image.
 */
void MainWindow::updateImageDisplay()
{
    if (currentImage.isNull())
        return;

    QPixmap pixmap = QPixmap::fromImage(currentImage);
    QSize viewerSize = imageViewer->size();

    QPixmap scaledPixmap = pixmap.scaled(viewerSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    scaledImageSize = scaledPixmap.size();

    imageOffsetX = (imageViewer->width() - scaledPixmap.width()) / 2;
    imageOffsetY = (imageViewer->height() - scaledPixmap.height()) / 2;

    QPixmap displayPixmap = QPixmap(imageViewer->size());
    displayPixmap.fill(Qt::transparent);

    QPainter painter(&displayPixmap);
    painter.drawPixmap(imageOffsetX, imageOffsetY, scaledPixmap);

    if (isCropping && isCropMode) {
        painter.setPen(QPen(Qt::DashLine));
        painter.drawRect(cropRect);
    }

    imageViewer->setPixmap(displayPixmap);
}

/**
 * @brief Toggles the visibility of a histogram channel.
 * @param channel The color channel to toggle ("red", "green", "blue").
 */
void MainWindow::toggleHistogram(const QString& channel)
{
    channelVisibility[channel] = !channelVisibility[channel];
    QString imageIdentifier = currentImagePath;

    if (histogramCache.contains(imageIdentifier) && histogramCache[imageIdentifier].contains(channel)) {
        updateHistogramDisplay();
        return;
    }

    controller->calculateHistogramAsync(currentImage, channel, imageIdentifier);
}

/**
 * @brief Updates the histogram display with the current histogram data.
 */
void MainWindow::updateHistogramDisplay()
{
    histogramImage->fill(QColor("#f5f5dc"));

    QPainter painter(histogramImage);
    painter.setRenderHint(QPainter::Antialiasing);

    QString imageIdentifier = currentImagePath;

    if (channelVisibility["red"]) {
        if (histogramCache[imageIdentifier].contains("red")) {
            QVector<int> redHist = histogramCache[imageIdentifier]["red"];
            drawHistogram(painter, redHist, Qt::red);
        }
    }
    if (channelVisibility["green"]) {
        if (histogramCache[imageIdentifier].contains("green")) {
            QVector<int> greenHist = histogramCache[imageIdentifier]["green"];
            drawHistogram(painter, greenHist, Qt::green);
        }
    }
    if (channelVisibility["blue"]) {
        if (histogramCache[imageIdentifier].contains("blue")) {
            QVector<int> blueHist = histogramCache[imageIdentifier]["blue"];
            drawHistogram(painter, blueHist, Qt::blue);
        }
    }

    drawAxes(painter);

    histogramViewer->setPixmap(QPixmap::fromImage(*histogramImage));
}

/**
 * @brief Draws the histogram data onto the histogram image.
 * @param painter The QPainter object used for drawing.
 * @param histogram The histogram data to draw.
 * @param color The color to use for the histogram bars.
 */
void MainWindow::drawHistogram(QPainter& painter, const QVector<int>& histogram, QColor color)
{
    painter.setPen(color);

    int maxVal = *std::max_element(histogram.begin(), histogram.end());
    int width = histogramImage->width();
    int height = histogramImage->height();

    int availableHeight = height - 40;
    int yOffset = 20;

    float scalingFactor = static_cast<float>(availableHeight) / maxVal;

    int barWidth = (width - 40) / histogram.size();

    for (int i = 0; i < histogram.size(); ++i) {
        int barHeight = histogram[i] * scalingFactor;
        int xPosition = 30 + (i * barWidth);
        int yPosition = height - 20;

        painter.drawLine(xPosition, yPosition, xPosition, yPosition - barHeight);
    }
}

/**
 * @brief Draws the axes on the histogram image.
 * @param painter The QPainter object used for drawing.
 */
void MainWindow::drawAxes(QPainter& painter)
{
    QPen axisPen(Qt::black, 1.5);
    painter.setPen(axisPen);

    int xAxisStartX = 25;
    int xAxisStartY = histogramImage->height() - 20;
    int yAxisStartX = 25;
    int yAxisStartY = 20;
    int xAxisEndX = histogramImage->width() - 25;

    painter.drawLine(xAxisStartX, xAxisStartY, xAxisEndX, xAxisStartY);
    painter.drawLine(yAxisStartX, xAxisStartY, yAxisStartX, yAxisStartY);

    painter.drawLine(xAxisEndX, xAxisStartY, xAxisEndX - 10, xAxisStartY - 5);
    painter.drawLine(xAxisEndX, xAxisStartY, xAxisEndX - 10, xAxisStartY + 5);

    painter.drawLine(yAxisStartX, yAxisStartY, yAxisStartX - 5, yAxisStartY + 10);
    painter.drawLine(yAxisStartX, yAxisStartY, yAxisStartX + 5, yAxisStartY + 10);
}

/**
 * @brief Saves the current image to a file.
 */
void MainWindow::saveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Image (*.png);;JPEG Image (*.jpg)"));
    if (!fileName.isEmpty()) {
        if (!currentImage.save(fileName)) {
            QMessageBox::warning(this, tr("Save Error"), tr("Failed to save the image."));
        }
    }
}

/**
 * @brief Rotates the current image 90 degrees to the right.
 */
void MainWindow::rotateImageRight()
{
    currentImage = currentImage.transformed(QTransform().rotate(90));
    originalImage = originalImage.transformed(QTransform().rotate(90));
    updateImageDisplay();
}

/**
 * @brief Rotates the current image 90 degrees to the left.
 */
void MainWindow::rotateImageLeft()
{
    currentImage = currentImage.transformed(QTransform().rotate(-90));
    originalImage = originalImage.transformed(QTransform().rotate(-90));
    updateImageDisplay();
}

/**
 * @brief Flips the current image horizontally.
 */
void MainWindow::flipImage()
{
    currentImage = currentImage.mirrored(true, false);
    originalImage = originalImage.mirrored(true, false);
    updateImageDisplay();
}

/**
 * @brief Handles the mouse press event for cropping functionality.
 * @param event The mouse event.
 */
void MainWindow::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isCropMode) {
        QPoint imageViewerPos = imageViewer->mapFrom(this, event->pos());
        if (imageViewer->rect().contains(imageViewerPos)) {
            isCropping = true;
            cropStartPoint = imageViewerPos;
            cropRect = QRect(cropStartPoint, cropStartPoint);
        }
    }
}

/**
 * @brief Handles the mouse move event for cropping functionality.
 * @param event The mouse event.
 */
void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (isCropping && isCropMode) {
        QPoint imageViewerPos = imageViewer->mapFrom(this, event->pos());
        cropRect = QRect(cropStartPoint, imageViewerPos).normalized();
        updateImageDisplay();
    }
}

/**
 * @brief Handles the mouse release event for cropping functionality.
 * @param event The mouse event.
 */
void MainWindow::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && isCropping && isCropMode) {
        isCropping = false;

        QRect adjustedRect = cropRect.translated(-imageOffsetX, -imageOffsetY);

        float scaleX = static_cast<float>(currentImage.width()) / scaledImageSize.width();
        float scaleY = static_cast<float>(currentImage.height()) / scaledImageSize.height();

        QRect imageCropRect = QRect(
            adjustedRect.left() * scaleX,
            adjustedRect.top() * scaleY,
            adjustedRect.width() * scaleX,
            adjustedRect.height() * scaleY
        ).normalized();

        if (!imageCropRect.isEmpty() && currentImage.rect().contains(imageCropRect)) {
            currentImage = currentImage.copy(imageCropRect);
            originalImage = originalImage.copy(imageCropRect);
            updateImageDisplay();
        }
        cropRect = QRect();
        isCropMode = false;
    }
}

/**
 * @brief Initiates the cropping mode for the image.
 */
void MainWindow::cropImage()
{
    isCropMode = true;
    cropRect = QRect();
}

/**
 * @brief Slot called when a filter button is clicked.
 * @param filterType The type of filter to apply.
 */
void MainWindow::onFilterButtonClicked(int filterType)
{
    if (currentImage.isNull()) {
        QMessageBox::warning(this, "No Image", "Please load an image first.");
        return;
    }

    if (currentFilter == filterType) {
        currentImage = originalImage;
        currentFilter = MainWindowController::NoFilter;
        updateImageDisplay();
    }
    else {
        currentFilter = static_cast<MainWindowController::FilterType>(filterType);
        controller->applyFilter(originalImage, currentFilter);
    }
}

/**
 * @brief Slot called when a filtered image is ready to be displayed.
 * @param filteredImage The filtered image.
 * @param filterType The type of filter applied.
 */
void MainWindow::displayFilteredResult(const QImage& filteredImage, MainWindowController::FilterType filterType)
{
    if (currentFilter == filterType) {
        currentImage = filteredImage;
        updateImageDisplay();
    }
}
