#include "upnpobject.h"

UpnpObject::UpnpObject(QObject *parent) :
    ListItem(parent),
    m_status(Null),
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
    m_status(Null),
    m_type(type),
    m_timeout(QDateTime::currentDateTime()),
    m_available(false),
    m_host(host),
    m_url(),
    m_description(),
    netManager(0)
{
    connect(this, SIGNAL(descriptionChanged()), this, SIGNAL(itemChanged()));
    connect(this, SIGNAL(statusChanged()), this, SIGNAL(availableChanged()));

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

void UpnpObject::setType(const TypeObject &type)
{
    m_type = type;
}

UpnpObject::Status UpnpObject::status() const
{
    return m_status;
}

void UpnpObject::setStatus(const Status &status)
{
    if (status < m_status)
    {
        qCritical() << "cannot change status to " << status << ", status = " << m_status;
    }
    else if (status != m_status)
    {
        m_status = status;
        emit statusChanged();
    }
    else
    {
        qDebug() << "same status, not necessary to update anything" << m_status << status;
    }
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

QDomNode UpnpObject::description() const
{
    return m_description;
}

QString UpnpObject::strDescription() const
{
    QDomDocument doc;
    QDomElement root = doc.createElement("description");
    QDomNode elt = doc.importNode(m_description, true);
    root.appendChild(elt);
    doc.appendChild(root);
    return doc.toString();
}

QString UpnpObject::valueFromDescription(const QString &param) const
{
    QDomNode elt = m_description.firstChildElement(param);

    return elt.firstChild().nodeValue();
}

void UpnpObject::setDescription(QDomNode node)
{
    m_description = node;

    emit descriptionChanged();
}

void UpnpObject::timeout()
{
    setAvailable(false);
}

bool UpnpObject::available() const
{
    return m_available && m_status == Ready;
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
    m_url = url;
}


QUrl UpnpObject::urlFromRelativePath(QString path)
{
    return m_url.resolved(path);
}
