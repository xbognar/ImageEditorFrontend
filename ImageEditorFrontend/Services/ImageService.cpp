#include "ImageService.h"
#include <QNetworkAccessManager>
#include <QNetworkReply> 
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication> 

ImageService::ImageService(QObject* parent) : BaseService(parent) {}

QList<Image> ImageService::getAllImages() {

    QNetworkRequest request(QUrl("http://localhost:80/api/images"));
    QNetworkReply* reply = getNetworkManager()->get(request);

    while (!reply->isFinished()) {
        qApp->processEvents();
    }

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

Image ImageService::getImageById(int id) {

    QNetworkRequest request(QUrl("http://localhost:80/api/images/" + QString::number(id)));
    QNetworkReply* reply = getNetworkManager()->get(request);

    Image image;
    while (!reply->isFinished()) {
        qApp->processEvents();
    }

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

void ImageService::addImage(const Image& image) {

    QNetworkRequest request(QUrl("http://localhost:80/api/images"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["name"] = image.name;
    json["imageData"] = QString::fromUtf8(image.imageData.toBase64());
    json["width"] = image.width;
    json["height"] = image.height;
    json["pixelFormat"] = image.pixelFormat;
    json["path"] = image.path;

    QNetworkReply* reply = getNetworkManager()->post(request, QJsonDocument(json).toJson());
    reply->deleteLater();
}

void ImageService::updateImage(int id, const Image& image) {

    QNetworkRequest request(QUrl("http://localhost:80/api/images/" + QString::number(id)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["name"] = image.name;
    json["imageData"] = QString::fromUtf8(image.imageData.toBase64());
    json["width"] = image.width;
    json["height"] = image.height;
    json["pixelFormat"] = image.pixelFormat;
    json["path"] = image.path; 

    QNetworkReply* reply = getNetworkManager()->put(request, QJsonDocument(json).toJson());
    reply->deleteLater();
}

void ImageService::deleteImage(int id) {

    QNetworkRequest request(QUrl("http://localhost:80/api/images/" + QString::number(id)));
    QNetworkReply* reply = getNetworkManager()->deleteResource(request);
    reply->deleteLater();
}
