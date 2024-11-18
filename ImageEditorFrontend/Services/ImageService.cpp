#include "ImageService.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>
#include <QEventLoop>

/**
 * @brief Constructs the ImageService object.
 * @param parent The parent QObject.
 */
ImageService::ImageService(QObject* parent) : BaseService(parent) {}

/**
 * @brief Retrieves all images from the server.
 * @return A QList of Image objects.
 */
QList<Image> ImageService::getAllImages() {

    QNetworkRequest request(QUrl("http://localhost:8080/api/images"));
    QNetworkReply* reply = getNetworkManager()->get(request);

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    QList<Image> images;
    if (reply->error() == QNetworkReply::NoError) {

        QByteArray response = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
        QJsonArray jsonArray = jsonResponse.array();

        for (const QJsonValue& value : jsonArray) {
            QJsonObject obj = value.toObject();
            Image image(
                obj["id"].toInt(),
                obj["name"].toString(),
                QByteArray::fromBase64(obj["imageData"].toString().toUtf8()),
                obj["width"].toInt(),
                obj["height"].toInt(),
                obj["pixelFormat"].toString(),
                obj["path"].toString()
            );
            images.append(image);
        }
    }

    reply->deleteLater();
    return images;
}

/**
 * @brief Retrieves a single image by its ID.
 * @param id The ID of the image to retrieve.
 * @return The Image object with the specified ID.
 */
Image ImageService::getImageById(int id) {

    QNetworkRequest request(QUrl("http://localhost:8080/api/images/" + QString::number(id)));
    QNetworkReply* reply = getNetworkManager()->get(request);

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    Image image;
    if (reply->error() == QNetworkReply::NoError) {

        QByteArray response = reply->readAll();
        QJsonObject obj = QJsonDocument::fromJson(response).object();

        image = Image(
            obj["id"].toInt(),
            obj["name"].toString(),
            QByteArray::fromBase64(obj["imageData"].toString().toUtf8()),
            obj["width"].toInt(),
            obj["height"].toInt(),
            obj["pixelFormat"].toString(),
            obj["path"].toString()
        );
    }

    reply->deleteLater();
    return image;
}

/**
 * @brief Adds a new image to the server.
 * @param image The Image object to add.
 * @return The newly added Image object with updated ID.
 */
Image ImageService::addImage(const Image& image) {

    QNetworkRequest request(QUrl("http://localhost:8080/api/images"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["name"] = image.name;
    json["imageData"] = QString::fromUtf8(image.imageData.toBase64());
    json["width"] = image.width;
    json["height"] = image.height;
    json["pixelFormat"] = image.pixelFormat;
    json["path"] = image.path;

    QNetworkReply* reply = getNetworkManager()->post(request, QJsonDocument(json).toJson());

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    Image newImage = image;

    if (reply->error() == QNetworkReply::NoError) {

        QByteArray response = reply->readAll();
        QJsonObject obj = QJsonDocument::fromJson(response).object();

        newImage.id = obj["id"].toInt();
    }
    else {

        qDebug() << "Error adding image:" << reply->errorString();

    }

    reply->deleteLater();
    return newImage;
}

/**
 * @brief Updates an existing image on the server.
 * @param id The ID of the image to update.
 * @param image The Image object with updated data.
 */
void ImageService::updateImage(int id, const Image& image) {

    QNetworkRequest request(QUrl("http://localhost:8080/api/images/" + QString::number(id)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["name"] = image.name;
    json["imageData"] = QString::fromUtf8(image.imageData.toBase64());
    json["width"] = image.width;
    json["height"] = image.height;
    json["pixelFormat"] = image.pixelFormat;
    json["path"] = image.path;

    QNetworkReply* reply = getNetworkManager()->put(request, QJsonDocument(json).toJson());

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    if (reply->error() != QNetworkReply::NoError) {

        qDebug() << "Error updating image:" << reply->errorString();

    }

    reply->deleteLater();
}

/**
 * @brief Deletes an image from the server.
 * @param id The ID of the image to delete.
 */
void ImageService::deleteImage(int id) {

    QNetworkRequest request(QUrl("http://localhost:8080/api/images/" + QString::number(id)));
    QNetworkReply* reply = getNetworkManager()->deleteResource(request);

    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    if (reply->error() != QNetworkReply::NoError) {

        qDebug() << "Error deleting image:" << reply->errorString();

    }

    reply->deleteLater();
}
