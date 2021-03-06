#ifndef MYAPPLICATION_H
#define MYAPPLICATION_H

#include "application.h"
#include "upnpcontrolpoint.h"
#include "Models/listmodel.h"
#include "messageitem.h"
#include "upnptimer.h"
#include "upnprootdevice.h"

class MyApplication : public Application
{
    Q_OBJECT

    Q_PROPERTY(ListModel *messageModel READ messageModel NOTIFY messageModelChanged)
    Q_PROPERTY(UpnpControlPoint *upnpControlPoint READ upnpControlPoint  NOTIFY upnpControlPointChanged)

public:
    explicit MyApplication(int &argc, char **argv);

    ListModel *messageModel() const { return m_messageModel; }
    UpnpControlPoint *upnpControlPoint() const { return m_upnp; }

signals:
    void messageModelChanged();
    void upnpControlPointChanged();

private slots:
    void messageReceived(const QHostAddress &host, const int &port, const SsdpMessage &message);
    void advertiseSlot();

private:
    QNetworkAccessManager netManager;
    UpnpControlPoint *m_upnp;
    ListModel *m_messageModel;
    UpnpTimer m_timerDiscover;
};

#endif // MYAPPLICATION_H
