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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <Models/Image.h>
#include <QtConcurrent>
#include <Services/ImageService.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), firstResizeEvent(true)
{
    ui.setupUi(this);

    imageViewer = ui.imageViewer;
    imageList = ui.imageList;
    histogramViewer = ui.histogramViewer;

    // Initialize buttons
    cropButton = ui.cropButton;
    rotateRightButton = ui.rotateRightButton;
    rotateLeftButton = ui.rotateLeftButton;
    flipButton = ui.flipButton;

    folderButton = ui.openButton;
    saveButton = ui.saveButton;

    // Initialize filter buttons and RGB buttons
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

    // Initialize stylesheet and histogram
    applyStylesheet();
    setupHistogram();

    firstResizeEvent = true;

    // Connect buttons to their respective slots
    connect(folderButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveFile);
    connect(ui.actionExit, &QAction::triggered, this, &MainWindow::exitApp);
}

MainWindow::~MainWindow()
{
    delete histogramImage;
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

    int xAxisStartX = 30, xAxisStartY = histogramImage->height() - 30;
    int yAxisStartX = 30, yAxisStartY = 30;

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

    const int padding = 20;  // General padding between elements
    const int leftPadding = 20;  // Left padding for the main image and buttons
    const int buttonpadding = 300;  // General padding between elements
    const int bottomPadding = 30;  // Bottom padding for the main image and histogram
    const int columnWidth = 250;  // Fixed width for the left column (list + histogram)
    const int histogramSize = 270;  // Fixed size for the histogram
    const int buttonGap = 30;  // Adjusted gap between buttons
    const int baseGapBetweenRGBAndFilters = 150;  // Base gap between filter and RGB buttons

    int minWindowWidth = 800;
    int minWindowHeight = 590;

    if (newSize.width() < minWindowWidth || newSize.height() < minWindowHeight) {
        this->resize(minWindowWidth, minWindowHeight);
        return;
    }

    // Padding and positioning adjustments for left-side elements
    int histogramX = leftPadding - 10;  // Fixed position on the left
    int histogramY = newSize.height() - histogramSize - bottomPadding - 20;  // Bottom left corner
    histogramViewer->move(histogramX, histogramY);
    histogramViewer->resize(histogramSize, histogramSize);  // Fixed histogram size

    // Image viewer adjustments
    int imageX = columnWidth + (5.5 * padding);  // Left padding for the main image
    int imageWidth = newSize.width() - imageX - (5 * padding) + 15;  // Adjust width
    int imageHeight = newSize.height() - bottomPadding - (3 * padding);  // Adjust height
    imageViewer->move(imageX, padding);
    imageViewer->setFixedSize(imageWidth, imageHeight);

    int fileListHeight = histogramY - (2 * padding) + 10;  // Dynamic height for the file list
    imageList->move(leftPadding, padding);
    imageList->setFixedSize(columnWidth + 10, fileListHeight);

    // Dynamically calculate the spacing between buttons on the left side
    int proportionalLeftButtonSpacing = ((fileListHeight + 30) - (35 * 6)) / 7;

    // Adjust button positions on the left side with proportional gaps
    folderButton->move(buttonpadding, proportionalLeftButtonSpacing + 10);
    saveButton->move(buttonpadding, folderButton->geometry().bottom() + proportionalLeftButtonSpacing);
    cropButton->move(buttonpadding, saveButton->geometry().bottom() + proportionalLeftButtonSpacing);
    rotateRightButton->move(buttonpadding, cropButton->geometry().bottom() + proportionalLeftButtonSpacing);
    rotateLeftButton->move(buttonpadding, rotateRightButton->geometry().bottom() + proportionalLeftButtonSpacing);
    flipButton->move(buttonpadding, rotateLeftButton->geometry().bottom() + proportionalLeftButtonSpacing);

    // Adjust the button positions and gaps on the right side
    int rightButtonXOffset = newSize.width() - buttonWidth - padding + 15;
    int rightButtonSpacing = buttonGap;

    filter1Button->move(rightButtonXOffset, rightButtonSpacing + 20);
    filter2Button->move(rightButtonXOffset, filter1Button->geometry().bottom() + rightButtonSpacing);
    filter3Button->move(rightButtonXOffset, filter2Button->geometry().bottom() + rightButtonSpacing);
    filter4Button->move(rightButtonXOffset, filter3Button->geometry().bottom() + rightButtonSpacing);

    // Dynamically adjust the gap between the filter buttons and RGB buttons
    int dynamicGapBetweenFilterAndRGB = baseGapBetweenRGBAndFilters + (newSize.height() - 700);
    redRGBButton->move(rightButtonXOffset, filter4Button->geometry().bottom() + dynamicGapBetweenFilterAndRGB);
    blueRGBButton->move(rightButtonXOffset, redRGBButton->geometry().bottom() + rightButtonSpacing - 20);
    greenRGBButton->move(rightButtonXOffset, blueRGBButton->geometry().bottom() + rightButtonSpacing - 20);

    // Ensure labels are centered under the corresponding filter buttons
    filter1Label->move(filter1Button->geometry().left() + (filter1Button->width() / 2) - (filter1Label->width() / 2), filter1Button->geometry().bottom() + 5);
    filter2Label->move(filter2Button->geometry().left() + (filter2Button->width() / 2) - (filter2Label->width() / 2), filter2Button->geometry().bottom() + 5);
    filter3Label->move(filter3Button->geometry().left() + (filter3Button->width() / 2) - (filter3Label->width() / 2), filter3Button->geometry().bottom() + 5);
    filter4Label->move(filter4Button->geometry().left() + (filter4Button->width() / 2) - (filter4Label->width() / 2), filter4Button->geometry().bottom() + 5);

    QMainWindow::resizeEvent(event);
}

void MainWindow::openFile() {
    
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (image.isNull()) {
            QMessageBox::warning(this, tr("Open Image"), tr("Could not open image."));
            return;
        }

        // Extract image data
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer, "PNG"); // Save as PNG format

        // Create Image object
        Image newImage;
        newImage.name = QFileInfo(fileName).fileName();
        newImage.imageData = imageData;
        newImage.width = image.width();
        newImage.height = image.height();
        newImage.pixelFormat = "RGBA"; // Assuming RGBA, adjust if needed
        newImage.path = fileName;

        // Send PUT request
        ImageService imageService;
        imageService.addImage(newImage);

        // Update UI
        QListWidgetItem* item = new QListWidgetItem(QIcon(QPixmap::fromImage(image.scaled(50, 50))), newImage.name);
        item->setData(Qt::UserRole, QVariant::fromValue(newImage));
        imageList->addItem(item);
    }

}

void MainWindow::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), "", tr("PNG Image (*.png);;JPEG Image (*.jpg);;Bitmap (*.bmp)"));
    if (!fileName.isEmpty()) {
       
    }
}

void MainWindow::exitApp()
{
    QApplication::quit();
}
