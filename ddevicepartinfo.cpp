#include "ddevicepartinfo.h"
#include "dpartinfo_p.h"
#include "util.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QDebug>

class DDevicePartInfoPrivate : public DPartInfoPrivate
{
public:
    DDevicePartInfoPrivate(DDevicePartInfo *qq);

    QString device() const Q_DECL_OVERRIDE;
    void refresh() Q_DECL_OVERRIDE;

    void init(const QJsonObject &obj);
};

DDevicePartInfoPrivate::DDevicePartInfoPrivate(DDevicePartInfo *qq)
    : DPartInfoPrivate(qq)
{

}

QString DDevicePartInfoPrivate::device() const
{
    return Util::getDeviceByName(name);
}

void DDevicePartInfoPrivate::refresh()
{
    *q = DDevicePartInfo(name);
}

void DDevicePartInfoPrivate::init(const QJsonObject &obj)
{
    name = obj.value("name").toString();
    kname = obj.value("kname").toString();
    sizeDisplay = obj.value("size").toString();
    typeName = obj.value("fstype").toString();
    type = toType(typeName);
    mountPoint = obj.value("mountpoint").toString();
    label = obj.value("label").toString();
    partLabel = obj.value("partlabel").toString();
    partType = obj.value("parttype").toString();
    guidType = DPartInfo::guidType(partType.toLatin1().toUpper());

    QString device = Util::getDeviceByName(name);

    int code = -1;
    const QByteArray &data = Util::processExec(QStringLiteral("partx %1 -b -P -o START,END,SECTORS,SIZE").arg(device), -1, &code);

    if (code == 0) {
        const QByteArrayList &list = data.split(' ');

        if (list.count() != 4) {
            qWarning() << "Get device START/END/SECTORS/SIZE info error by partx:" << device;

            return;
        }

        quint64 start = list.first().split('"').at(1).toULongLong();
        quint64 end = list.at(1).split('"').at(1).toULongLong();
        quint64 sectors = list.at(2).split('"').at(1).toULongLong();

        Q_ASSERT(sectors > 0);

        size = list.last().split('"').at(1).toULongLong();
        sizeStart = start * size / sectors;
        sizeEnd = end * size / sectors;
    }
}

DDevicePartInfo::DDevicePartInfo()
    : DPartInfo(new DDevicePartInfoPrivate(this))
{

}

DDevicePartInfo::DDevicePartInfo(const QString &name)
    : DPartInfo(new DDevicePartInfoPrivate(this))
{
    const QJsonArray &block_devices = Util::getBlockDevices(Util::getDeviceByName(name));

    if (!block_devices.isEmpty())
        d_func()->init(block_devices.first().toObject());
}

QList<DDevicePartInfo> DDevicePartInfo::localePartList()
{
    const QJsonArray &block_devices = Util::getBlockDevices("-l");

    QList<DDevicePartInfo> list;

    for (const QJsonValue &value : block_devices) {
        const QJsonObject &obj = value.toObject();
        const QString &fstype = obj.value("fstype").toString();

        if (fstype.isEmpty())
            continue;

        DDevicePartInfo info;

        info.d_func()->init(obj);
        list << info;
    }

    return list;
}

void DDevicePartInfo::init(const QJsonObject &obj)
{
    d_func()->init(obj);
}
