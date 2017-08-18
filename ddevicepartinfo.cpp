#include "ddevicepartinfo.h"
#include "dpartinfo_p.h"
#include "helper.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QDebug>

class DDevicePartInfoPrivate : public DPartInfoPrivate
{
public:
    DDevicePartInfoPrivate(DDevicePartInfo *qq);

    QString filePath() const Q_DECL_OVERRIDE;
    void refresh() Q_DECL_OVERRIDE;

    void init(const QJsonObject &obj);
};

DDevicePartInfoPrivate::DDevicePartInfoPrivate(DDevicePartInfo *qq)
    : DPartInfoPrivate(qq)
{

}

QString DDevicePartInfoPrivate::filePath() const
{
    return Helper::getDeviceByName(name);
}

void DDevicePartInfoPrivate::refresh()
{
    *q = DDevicePartInfo(name);
}

static bool isDiskType(const QString &name, const QString &type)
{
    return type == "disk" || (type == "loop" && name.length() == 5);
}

void DDevicePartInfoPrivate::init(const QJsonObject &obj)
{
    name = obj.value("name").toString();
    kname = obj.value("kname").toString();
    size = obj.value("size").toString().toLongLong();
    typeName = obj.value("fstype").toString();
    type = toType(typeName);
    mountPoint = obj.value("mountpoint").toString();
    label = obj.value("label").toString();
    partLabel = obj.value("partlabel").toString();
    partType = obj.value("parttype").toString();
    guidType = DPartInfo::guidType(partType.toLatin1().toUpper());

    const QString &device = Helper::getDeviceByName(name);

    if (isDiskType(name, obj.value("type").toString())) {
        sizeStart = 0;
        sizeEnd = size - 1;
    } else {
        int code = Helper::processExec(QStringLiteral("partx %1 -b -P -o START,END,SECTORS").arg(device));

        if (code == 0) {
            const QByteArray &data = Helper::lastProcessStandardOutput();
            const QByteArrayList &list = data.split(' ');

            if (list.count() != 3) {
                dCError("Get partition START/END/SECTORS/SIZE info error by partx, device: %s", qPrintable(device));

                return;
            }

            qint64 start = list.first().split('"').at(1).toLongLong();
            qint64 end = list.at(1).split('"').at(1).toLongLong();
            qint64 sectors = list.at(2).split('"').at(1).toLongLong();

            Q_ASSERT(sectors > 0);

            sizeStart = start * size / sectors;
            sizeEnd = (end + 1) * size / sectors - 1;
        }
    }

    if (type == DPartInfo::Invalid || type == DPartInfo::Unknow) {
        usedSize = size;
        freeSize = 0;
    } else if (!Helper::getPartitionSizeInfo(*q, usedSize, freeSize, blockSize)) {
        dCError("Get partition used sieze/free size info failed, device: %s", qPrintable(device));
    }
}

DDevicePartInfo::DDevicePartInfo()
    : DPartInfo(new DDevicePartInfoPrivate(this))
{

}

DDevicePartInfo::DDevicePartInfo(const QString &name)
    : DPartInfo(new DDevicePartInfoPrivate(this))
{
    const QJsonArray &block_devices = Helper::getBlockDevices(Helper::getDeviceByName(name));

    if (!block_devices.isEmpty())
        d_func()->init(block_devices.first().toObject());
}

QList<DDevicePartInfo> DDevicePartInfo::localePartList()
{
    const QJsonArray &block_devices = Helper::getBlockDevices("-l");

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
