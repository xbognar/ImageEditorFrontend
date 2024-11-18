
# Image Editor Frontend

## Overview

The **Image Editor Frontend** is a C++ and Qt-based application for editing images, designed to provide efficient, responsive image processing functionalities. The app allows users to load, modify, and apply various filters to images, with a multithreaded architecture ensuring smooth operation. The backend integration enables seamless image data management, and caching is utilized to improve performance by storing processed images for quicker access.

## Technologies Used

- **C++**: Core language for implementing the application logic and algorithms.
- **Qt Framework**: Used for UI components, event handling, and managing asynchronous operations.
- **QtConcurrent**: Enables multithreading, allowing computationally heavy tasks to run in parallel without blocking the main thread.
- **MVC Architecture**: Separates the application into Model, View, and Controller components for better organization and maintainability.

## Features

- **Multithreading**: Image processing tasks run on separate threads, keeping the UI responsive.
- **Cache Optimization**: Processed images are cached to avoid redundant computations and improve efficiency.
- **Backend Integration**: Connects to a backend API for image management, supporting operations like loading, saving, and deleting images.
- **Image Editing Functionalities**: Users can apply multiple filters and other effects to images, with each filter implemented as a separate algorithm.

## Project Structure

```
ImageEditorFrontend/
├── ImageEditorFrontend.sln
├── ImageEditorFrontend/
│   ├── Algorithms/
│   ├── Controllers/
│   ├── Models/
│   ├── Resources/
│   ├── Services/
│   ├── Views/
│   ├── main.cpp
├── ImageEditorTests/
│   ├── AlgorithmsTests/
│   │   ├── TestImageProcessor.cpp
└── x64/
    └── Debug/
```

## Detailed Description of Components

### Models
The Models represent data structures used within the application, such as the `Image` model, which stores image metadata and binary data. Models provide a structured way to represent image information, making it easier to manage and transfer data between the application components.

### Views
The Views consist of UI components for displaying images and interacting with users. The `MainWindow` handles the primary interface, including image viewing, filter selection, and image loading/saving options. Each UI element is designed to be intuitive, providing users with a seamless editing experience.

### Controllers
Controllers manage the application's logic, acting as intermediaries between Views and Models. For instance, `MainWindowController` handles tasks like fetching images, applying filters, and caching results. By managing multithreading through `QtConcurrent`, controllers ensure that operations are asynchronous, avoiding UI freezes.

### Services
Service classes facilitate communication with external resources, such as the backend server. The `ImageService` handles HTTP requests for loading, adding, updating, and deleting images from the backend, allowing for efficient image management in a connected environment.

### Algorithms
Algorithms contain the logic for each image processing effect, such as grayscale, oil painting, dramatic, and warm effects. Each algorithm is implemented as a separate class, allowing for modularity and scalability. Algorithms are designed to process images efficiently, utilizing multithreading for optimal performance.

## Unit Testing

A dedicated **ImageEditorTests** project provides unit tests for validating the image processing algorithms. Currently, the `TestImageProcessor` class includes tests for verifying histogram calculations in different color channels (red, green, blue). The tests check aspects like histogram size, values, and channel-specific accuracy. In the future, this project will cover all image processing algorithms, ensuring the robustness and accuracy of each effect applied within the app.
