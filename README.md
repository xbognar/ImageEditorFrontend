
# Image Editor Frontend

## Overview

The Image Editor Frontend is a Qt-based C++ application that provides essential image editing features with a focus on filter application and image manipulation. This project is designed to work efficiently with large images, utilizing multithreading for better performance, and caching processed images to avoid redundant computations. The application interfaces with a backend to load and save images, offering a smooth and interactive user experience.

## Technologies Used

- **C++**: Core programming language for building the application.
- **Qt Framework**: For UI components, networking, and asynchronous operations.
- **Multithreading (QtConcurrent)**: Ensures responsive performance by running filter operations in separate threads.
- **REST API Integration**: Fetches, adds, updates, and deletes images through HTTP requests.
- **Unit Testing (QtTest)**: Verifies the functionality of the image processing algorithms.

## Screenshots

![Screenshot of the UI](https://github.com/xbognar/ImageEditorFrontend/blob/master/ImageEditorFrontend/Resources/Icons/AppScreenshot.png)

## Features

- **Multithreading**: Asynchronous filter application to avoid blocking the main UI thread.
- **Caching**: Processed images are cached, minimizing redundant calculations when switching between filters.
- **Backend Integration**: Retrieves and manages image data via REST API.
- **Image Editing Functionalities**: Supports applying multiple filters to images and performing transformations like cropping, rotation, and flipping.
- **Responsive UI**: User-friendly interface with dynamic updates based on user interactions.

## Project Structure

```plaintext
ImageEditorFrontend/
├── Algorithms/            
│   ├── DramaticAlgorithm.cpp
│   ├── DramaticAlgorithm.h
│   ├── GrayscaleAlgorithm.cpp
│   ├── GrayscaleAlgorithm.h
│   ├── ImageProcessor.cpp
│   ├── ImageProcessor.h
│   ├── OilPaintingAlgorithm.cpp
│   ├── OilPaintingAlgorithm.h
│   └── WarmAlgorithm.cpp
│   └── WarmAlgorithm.h
├── Controllers/               
│   ├── MainWindowController.cpp
│   └── MainWindowController.h
├── Models/                    
│   ├── Image.cpp
│   └── Image.h
├── Resources/                 
│   ├── MainWindow.qrc
│   ├── Icons/
│   │   ├── . . .
│   └── Styles/
│       └── Styles.qss
│   └── TestImages/
│       ├── . . .
├── Services/                  
│   ├── BaseService.cpp
│   ├── BaseService.h
│   ├── ImageService.cpp
│   └── ImageService.h
├── Views/                
│   ├── MainWindow.cpp
│   ├── MainWindow.h
│   └── MainWindow.ui
├── main.cpp                   
│
ImageEditorTests/
├── AlgorithmsTests/
│   └── TestImageProcessor.cpp
└── main.cpp                  
```

## Detailed Description of Components

- **Models**: Defines the structure of image-related data, including image properties like ID, name, dimensions, and path.
- **Views**: Manages the UI layout and elements, including the main window with buttons and image display areas.
- **Controllers**: Contains logic to handle user interactions, manage filter application, and communicate with backend services.
- **Services**: Handles HTTP requests and responses, allowing seamless integration with the backend API for image retrieval, addition, updating, and deletion.
- **Algorithms**: Contains various image processing algorithms that apply filters to images, such as grayscale, oil painting, and warm effects.

## Unit Testing

A separate project, `ImageEditorTests`, includes unit tests for validating the functionality of image processing algorithms. Current tests focus on verifying the correctness of histogram calculations for different color channels. Future tests will be implemented for all image processing algorithms.


