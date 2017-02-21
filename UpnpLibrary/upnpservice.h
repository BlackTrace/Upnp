#ifndef UPNPSERVICE_H
#define UPNPSERVICE_H

#include "upnpobject.h"

class UpnpService : public UpnpObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType READ serviceType NOTIFY serviceTypeChanged)
    Q_PROPERTY(QStringList actionsModel READ actionsModel NOTIFY actionsModelChanged)

    enum Roles {
        ServiceTypeRole = Qt::UserRole+1,
        AvailableRole
    };

public:
    explicit UpnpService(QObject *parent = 0);
    explicit UpnpService(QDomNode info, QObject *parent = 0);

    virtual QString id() const Q_DECL_OVERRIDE;

    virtual QVariant data(int role) const Q_DECL_OVERRIDE;

    QString getInfo(const QString &param) const;

    QString serviceType() const;
    QString serviceId() const;

    QUrl scpdUrl() const;
    QUrl controlUrl() const;
    QUrl eventUrl() const;

    void requestDescription();

    QStringList actionsModel() const;

    Q_INVOKABLE void runAction(const int &index);

private:
    void initRoles();
    void readActions();

    QNetworkReply *sendAction(const QString &action);

signals:
    void serviceTypeChanged();
    void actionsModelChanged();

public slots:

private slots:
    void itemAvailableChanged();
    void networkError(QNetworkReply::NetworkError error);
    void descriptionReceived();
    void actionFinished();

private:
    QDomNode m_info;
    QStringList m_actionsModel;
};

#endif // UPNPSERVICE_H
