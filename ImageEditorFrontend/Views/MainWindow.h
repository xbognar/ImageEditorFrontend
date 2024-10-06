#pragma once

#include <QtWidgets/QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QWidget>
#include <QListWidget>

#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent* event) override; // Event for resizing

private:
    Ui::MainWindowClass ui;

    QLabel* imageViewer;
    QLabel* histogramViewer;
    QListWidget* imageList;

    QImage* histogramImage;

    // Add references to the buttons
    QPushButton* cropButton;
    QPushButton* rotateRightButton;
    QPushButton* rotateLeftButton;
    QPushButton* flipButton;
    QPushButton* folderButton;
    QPushButton* saveButton;
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

    // Constants for widget sizes
    const int buttonWidth = 64;
    const int buttonHeight = 64;
    const int histogramWidth = 340;
    const int histogramHeight = 251;
    const int listWidth = 200;

    bool firstResizeEvent;

    void applyStylesheet();
	void setupHistogram();

private slots:
    void openFile();
    void saveFile();
    void exitApp();
};
