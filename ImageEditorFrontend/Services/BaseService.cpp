#include "BaseService.h"

QNetworkAccessManager* BaseService::networkManager = nullptr;

/**
 * @brief Constructs the BaseService object.
 * @param parent The parent QObject.
 */
BaseService::BaseService(QObject* parent) : QObject(parent) {}

/**
 * @brief Retrieves the shared QNetworkAccessManager instance.
 * @return A pointer to the QNetworkAccessManager.
 */
QNetworkAccessManager* BaseService::getNetworkManager() {

    if (!networkManager) {
        networkManager = new QNetworkAccessManager();
    }
    return networkManager;
}
