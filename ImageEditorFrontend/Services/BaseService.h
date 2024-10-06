#ifndef BASESERVICE_H
#define BASESERVICE_H

#include <QObject>
#include <QNetworkAccessManager>

class BaseService : public QObject {
    Q_OBJECT

public:
    explicit BaseService(QObject* parent = nullptr);
    static QNetworkAccessManager* getNetworkManager();

private:
    static QNetworkAccessManager* networkManager;
};

#endif // BASESERVICE_H

