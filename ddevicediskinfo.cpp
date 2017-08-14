#include "ddevicediskinfo.h"
#include "ddiskinfo_p.h"
#include "util.h"
#include "ddevicepartinfo.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>

static QString getPTName(const QString &device)
{
    const QByteArray &data = Util::processExec(QStringLiteral("/sbin/blkid -p -s PTTYPE -d -i %1").arg(device));

    if (data.isEmpty())
        return QString();

    const QByteArrayList &list = data.split('=');

    if (list.count() != 3)
        return QString();

    return list.last().simplified();
}

class DDeviceDiskInfoPrivate : public DDiskInfoPrivate
{
public:
    DDeviceDiskInfoPrivate(DDeviceDiskInfo *qq);

    void init(const QJsonObject &obj);

    QString device() const Q_DECL_OVERRIDE;
    void refresh() Q_DECL_OVERRIDE;
};

DDeviceDiskInfoPrivate::DDeviceDiskInfoPrivate(DDeviceDiskInfo *qq)
    : DDiskInfoPrivate(qq)
{

}

void DDeviceDiskInfoPrivate::init(const QJsonObject &obj)
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
        DDevicePartInfo info;

        info.init(part.toObject());
        children << info;
    }

    if (!obj.value("fstype").isNull()) {
        DDevicePartInfo info;

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

QString DDeviceDiskInfoPrivate::device() const
{
    return Util::getDeviceByName(name);
}

void DDeviceDiskInfoPrivate::refresh()
{
    *q = DDeviceDiskInfo(name);
}

DDeviceDiskInfo::DDeviceDiskInfo()
    : DDiskInfo(new DDeviceDiskInfoPrivate(this))
{

}

DDeviceDiskInfo::DDeviceDiskInfo(const QString &name)
    : DDiskInfo(new DDeviceDiskInfoPrivate(this))
{
    const QJsonArray &block_devices = Util::getBlockDevices(Util::getDeviceByName(name));

    if (!block_devices.isEmpty())
        d_func()->init(block_devices.first().toObject());
}

QList<DDeviceDiskInfo> DDeviceDiskInfo::localeDiskList()
{
    const QJsonArray &block_devices = Util::getBlockDevices();

    QList<DDeviceDiskInfo> list;

    for (const QJsonValue &value : block_devices) {
        DDeviceDiskInfo info;

        info.d_func()->init(value.toObject());
        list << info;
    }

    return list;
}

DDeviceDiskInfoPrivate *DDeviceDiskInfo::d_func() const
{
    return static_cast<DDeviceDiskInfoPrivate*>(d.data());
}
