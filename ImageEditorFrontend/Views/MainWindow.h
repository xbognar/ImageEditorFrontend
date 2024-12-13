#pragma once

#include <QtWidgets/QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QListWidget>
#include <QList>
#include <QMap>
#include <QSet>
#include <QMouseEvent>
#include <QRect>
#include <QImage>
#include "ui_MainWindow.h"
#include "../Services/ImageService.h"
#include "../Controllers/MainWindowController.h"
#include "../Algorithms/ImageProcessor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    
    Ui::MainWindowClass ui;

    QLabel* imageViewer;
    QLabel* histogramViewer;
    QListWidget* imageList;

    QImage* histogramImage;
    ImageService* imageService;
    MainWindowController* controller;
    ImageProcessor* imageProcessor;

    QPushButton* cropButton;
    QPushButton* rotateRightButton;
    QPushButton* rotateLeftButton;
    QPushButton* flipButton;
    QPushButton* folderButton;
    QPushButton* deleteButton;
    QPushButton* filter1Button;
    QPushButton* filter2Button;
    QPushButton* filter3Button;
    QPushButton* filter4Button;
    QLabel* filter1Label;
    QLabel* filter2Label;
    QLabel* filter3Label;
    QLabel* filter4Label;
    QPushButton* redRGBButton;
    QPushButton* blueRGBButton;
    QPushButton* greenRGBButton;

    bool isCropping;
    bool isCropMode;
    bool firstResizeEvent;
    int imageOffsetX;
    int imageOffsetY;
    QList<Image> images;
    QMap<QString, QImage> loadedImages;
    QImage currentImage;
    QString currentImagePath;
    QMap<QString, bool> channelVisibility;
    QMap<QString, QMap<QString, QVector<int>>> histogramCache;
    QRect cropRect;
    QPoint cropStartPoint;
    QSize scaledImageSize;
    QImage originalImage;
    MainWindowController::FilterType currentFilter;
    QMap<QPushButton*, MainWindowController::FilterType> filterButtons;
    QPixmap scaleImageToViewer(const QImage& image);

    void applyStylesheet();
    void setupHistogram();
    void displayImages(const QList<Image>& images);
    void deleteSelectedImage();
    void loadFirstImage();
    void updateImageDisplay();
    void drawColumnsAndCircles(QPainter& painter);
    void addImageToList(const Image& image);
    bool isImageInList(const QString& path);
    void toggleHistogram(const QString& channel);
    void updateHistogramDisplay();
    void drawHistogram(QPainter& painter, const QVector<int>& histogram, QColor color);
    void drawAxes(QPainter& painter);
    

private slots:

    void openFile();
    void loadImages();
    void exitApp();
    void onImagesFetched(const QList<Image>& images);
    void onImageAdded(const Image& image);
    void onImageDeleted(int id);
    void onImageSelected(QListWidgetItem* item);
    void onHistogramCalculated(const QString& imageIdentifier, const QString& channel, const QVector<int>& histogram);
    void rotateImageRight();
    void rotateImageLeft();
    void flipImage();
    void cropImage();
    void saveImage();
    void onFilterButtonClicked(int filterType);
    void displayFilteredResult(const QImage& filteredImage, MainWindowController::FilterType filterType);

};

