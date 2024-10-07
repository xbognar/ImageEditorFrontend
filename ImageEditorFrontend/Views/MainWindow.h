#pragma once

#include <QtWidgets/QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QListWidget>
#include <QList>
#include "Models/Image.h"
#include "Services/ImageService.h"
#include "Controllers/MainWindowController.h"

#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    
    void resizeEvent(QResizeEvent* event) override;

private:
    
    Ui::MainWindowClass ui;

    QLabel* imageViewer;
    QLabel* histogramViewer;
    QListWidget* imageList;

    QImage* histogramImage;
    ImageService* imageService;
    MainWindowController* controller;

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

    QList<Image> images;
    bool firstResizeEvent;

    void applyStylesheet();
    void setupHistogram();
    void displayImages(const QList<Image>& images);
    void deleteSelectedImage();                   

private slots:

    void openFile();
    void loadImages();
    void onImageAdded();
    void onImagesFetched(const QList<Image>& images);
    void exitApp();
};
