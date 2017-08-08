#include "ddiskinfo.h"
#include "dpartinfo.h"
#include "util.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QDebug>

class DDiskInfoPrivate : public QSharedData
{
public:
    void init(const QJsonObject &obj);

    QString name;
    QString kname;
    quint64 size;
    QString sizeDisplay;
    QString typeName;
    DDiskInfo::Type type;
    QList<DPartInfo> children;
    QString ptTypeName;
    DDiskInfo::PTType ptType;
};

QString getPTName(const QString &device)
{
    const QByteArray &data = Util::processExec(QStringLiteral("/sbin/blkid -p -s PTTYPE -d -i %1").arg(device));

    if (data.isEmpty())
        return QString();

    const QByteArrayList &list = data.split('=');

    if (list.count() != 3)
        return QString();

    return list.last().simplified();
}

void DDiskInfoPrivate::init(const QJsonObject &obj)
{
    name = obj.value("name").toString();
    kname = obj.value("kname").toString();
    sizeDisplay = obj.value("size").toString();
    typeName = obj.value("type").toString();

    if (typeName == "loop")
        type = DDiskInfo::Loop;
    else
        type = DDiskInfo::Disk;

    const QJsonArray &list = obj.value("children").toArray();

    for (const QJsonValue &part : list) {
        DPartInfo info;

        info.init(part.toObject());
        children << info;
    }

    if (!obj.value("fstype").isNull()) {
        DPartInfo info;

        info.init(obj);
        children << info;
    }

    ptTypeName = getPTName(Util::getDeviceByName(name));

    if (ptTypeName == "dos")
        ptType = DDiskInfo::MBR;
    else if (ptTypeName == "gpt")
        ptType = DDiskInfo::GPT;
    else ptType = DDiskInfo::Unknow;
}

DDiskInfo::DDiskInfo()
    : d(new DDiskInfoPrivate)
{

}

DDiskInfo::DDiskInfo(const QString &name)
    : d(new DDiskInfoPrivate)
{
    d->name = name;

    refresh();
}

DDiskInfo::DDiskInfo(const DDiskInfo &other)
    : d(other.d)
{

}

DDiskInfo::~DDiskInfo()
{

}

QString DDiskInfo::device() const
{
    return Util::getDeviceByName(name());
}

DDiskInfo &DDiskInfo::operator=(const DDiskInfo &other)
{
    d = other.d;
    return *this;
}

QString DDiskInfo::name() const
{
    return d->name;
}

QString DDiskInfo::kname() const
{
    return d->kname;
}

qint64 DDiskInfo::totalSize() const
{
    return d->size;
}

QString DDiskInfo::displaySize() const
{
    return d->sizeDisplay;
}

QString DDiskInfo::typeName() const
{
    return d->typeName;
}

DDiskInfo::Type DDiskInfo::type() const
{
    return d->type;
}

DDiskInfo::PTType DDiskInfo::ptType() const
{
    return d->ptType;
}

QList<DPartInfo> DDiskInfo::childrenPartList() const
{
    return d->children;
}

QList<DDiskInfo> DDiskInfo::localeDiskList()
{
    const QJsonArray &block_devices = Util::getBlockDevices();

    QList<DDiskInfo> list;

    for (const QJsonValue &value : block_devices) {
        DDiskInfo info;

        info.d->init(value.toObject());
        list << info;
    }

    return list;
}

void DDiskInfo::refresh()
{
    const QJsonArray &block_devices = Util::getBlockDevices(device());

    if (!block_devices.isEmpty())
        d->init(block_devices.first().toObject());
}

QT_BEGIN_NAMESPACE
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug deg, const DDiskInfo &info)
{
    QDebugStateSaver saver(deg);
    Q_UNUSED(saver)

    deg.space() << "name:" << info.name()
                << "size:" << info.displaySize();
    deg << "partitions:" << info.childrenPartList();

    return deg;
}
#endif
QT_END_NAMESPACE
