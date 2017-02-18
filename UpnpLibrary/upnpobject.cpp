#include "upnpobject.h"

UpnpObject::UpnpObject(QObject *parent) :
    ListItem(parent),
    m_timeout(QDateTime::currentDateTime()),
    m_available(false),
    m_host(),
    m_url(),
    m_description(),
    netManager(0)
{

}

UpnpObject::UpnpObject(TypeObject type, QHostAddress host, QObject *parent) :
    ListItem(parent),
    m_type(type),
    m_timeout(QDateTime::currentDateTime()),
    m_available(false),
    m_host(host),
    m_url(),
    m_description(),
    netManager(0)
{
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

void UpnpObject::setRoles(QHash<int, QByteArray> roles)
{
    m_roles = roles;
}

UpnpObject::TypeObject UpnpObject::type() const
{
    return m_type;
}

QHash<int, QByteArray>  UpnpObject::roleNames() const
{
    return m_roles;
}

bool UpnpObject::setData(const QVariant &value, const int &role)
{
    qWarning() << "unable to set data" << value << role;
    return false;
}

void UpnpObject::update(const SsdpMessage &message)
{
    QString nts = message.getHeader("NTS");

    if (nts == "ssdp:byebye")
    {
        setAvailable(false);
    }
    else
    {
        int cacheControl = message.cacheControl();
        if (cacheControl > 0)
        {
            m_timeout = QDateTime::currentDateTime().addSecs(cacheControl);

            m_timer.start(message.cacheControl()*1000);

            setAvailable(true);
        }
        else
        {
            qCritical() << "UpnpRootDevice, invalid cache-control" << cacheControl;
        }
    }

}

QNetworkAccessManager *UpnpObject::getNetworkManager() const
{
    return netManager;
}

void UpnpObject::setNetworkManager(QNetworkAccessManager *nam)
{
    if (thread() != nam->thread())
        qWarning() << "NetworkManager and UpnpObject are in different thread.";

    netManager = nam;
}

QNetworkReply *UpnpObject::get(const QString &location)
{
    if (netManager == 0)
    {
        qCritical() << "NetManager not initialized.";
    }
    else
    {
        if (location.isEmpty())
        {
            QString msg = QString("Unable to request description for host %1").arg(host().toString());
            qCritical() << msg;
        }
        else
        {
            QNetworkRequest request;
            request.setUrl(location);
            request.setRawHeader(QByteArray("HOST"), host().toString().toUtf8());

            return netManager->get(request);
        }
    }

    return 0;
}

QHostAddress UpnpObject::host() const
{
    return m_host;
}

QDomDocument UpnpObject::description() const
{
    return m_description;
}

QString UpnpObject::strDescription() const
{
    return m_description.toString();
}

QString UpnpObject::valueFromDescription(const QString &param) const
{
    QDomNodeList l_elt = m_description.elementsByTagName(param);

    if (l_elt.size() > 0)
    {
        QDomNode elt = l_elt.at(0);
        return elt.firstChild().nodeValue();
    }

    return QString();
}

void UpnpObject::setDescription(QByteArray data)
{
    m_description.setContent(data);
    emit descriptionChanged();
}

void UpnpObject::setDescription(QDomNode node)
{
    m_description.clear();

    QDomElement root = m_description.createElement("root");

    QDomNode elt = m_description.importNode(node, true);
    root.appendChild(elt);

    m_description.appendChild(root);

    emit descriptionChanged();
}

void UpnpObject::timeout()
{
    setAvailable(false);
}

bool UpnpObject::available() const
{
    return m_available;
}

void UpnpObject::setAvailable(bool flag)
{
    if (flag != m_available)
    {
        m_available = flag;
        emit availableChanged();
    }
}

QDateTime UpnpObject::timeoutDateTime() const
{
    return m_timeout;
}

QUrl UpnpObject::url() const
{
    return m_url;
}

void UpnpObject::setUrl(QUrl url)
{
    m_url = url.adjusted(QUrl::RemoveFilename);
}


QUrl UpnpObject::urlFromRelativePath(QString path)
{
    return m_url.resolved(path);
}
