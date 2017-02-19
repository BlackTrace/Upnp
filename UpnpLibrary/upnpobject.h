#ifndef UPNPOBJECT_H
#define UPNPOBJECT_H

#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QHostAddress>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDomDocument>
#include "ssdpmessage.h"
#include "Models/listitem.h"

class UpnpObject : public ListItem
{
    Q_OBJECT

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString description READ strDescription  NOTIFY descriptionChanged)
    Q_PROPERTY(bool available READ available WRITE setAvailable NOTIFY availableChanged)

public:
    enum TypeObject { RootDevice, Device, Service };
    enum Status { Null, Loading, Ready, Error };

    explicit UpnpObject(QObject *parent = 0);
    explicit UpnpObject(TypeObject type, QHostAddress host, QObject *parent = 0);

    TypeObject type() const;
    void setType(const TypeObject &type);

    virtual QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;
    virtual bool setData(const QVariant &value, const int &role) Q_DECL_OVERRIDE;

    void setRoles(QHash<int, QByteArray> roles);

    Status status() const;
    void setStatus(const Status &status);

    bool available() const;
    void setAvailable(bool flag);

    QDateTime timeoutDateTime() const;

    void update(const SsdpMessage &message);

    QNetworkAccessManager *getNetworkManager() const;
    void setNetworkManager(QNetworkAccessManager *nam);

    QHostAddress host() const;

    QUrl url() const;
    void setUrl(QUrl url);

    QUrl urlFromRelativePath(QString path);

    QDomNode description() const;
    QString strDescription() const;
    void setDescription(QDomNode node);

    QString valueFromDescription(const QString &param) const;

    QNetworkReply *get(const QString &location);

signals:
    void statusChanged();
    void descriptionChanged();
    void availableChanged();

public slots:

private slots:
    void timeout();

private:
    Status m_status;
    QHash<int, QByteArray> m_roles;
    TypeObject m_type;
    QDateTime m_timeout;
    QTimer m_timer;
    bool m_available;
    QHostAddress m_host;
    QUrl m_url;
    QDomNode m_description;

    QNetworkAccessManager *netManager;
};

#endif // UPNPOBJECT_H
