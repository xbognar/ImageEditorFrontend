#include "BaseService.h"

QNetworkAccessManager* BaseService::networkManager = nullptr;

BaseService::BaseService(QObject* parent) : QObject(parent) {}

QNetworkAccessManager* BaseService::getNetworkManager() {
    
    if (!networkManager) {
        networkManager = new QNetworkAccessManager();
    }
    return networkManager;
}
