#include "ddiskinfo.h"
#include "util.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QDebug>

class DPartInfoPrivate : public QSharedData
{
public:
    void init(const QJsonObject &obj);
    DPartInfo::Type toType(const QString &name);

    QString name;
    QString kname;
    // Unit: bytes
    quint64 sizeStart = 0;
    quint64 sizeEnd = 0;
    quint64 size = 0;
    QString sizeDisplay;
    QString typeName;
    DPartInfo::Type type;
    QString mountPoint;
    QString label;
};

void DPartInfoPrivate::init(const QJsonObject &obj)
{
    name = obj.value("name").toString();
    kname = obj.value("kname").toString();
    sizeDisplay = obj.value("size").toString();
    typeName = obj.value("fstype").toString();
    type = toType(typeName);
    mountPoint = obj.value("mountpoint").toString();
    label = obj.value("label").toString();

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

DPartInfo::Type DPartInfoPrivate::toType(const QString &name)
{
    if (name.isEmpty())
        return DPartInfo::Unknow;

    if (name == "ext2" ) {
        return DPartInfo::EXT2;
    } else if (name == "ext3") {
        return DPartInfo::EXT3;
    } else if (name == "ext4") {
        return DPartInfo::EXT4;
    } else if (name == "fat12") {
        return DPartInfo::FAT12;
    } else if (name == "fat16") {
        return DPartInfo::FAT16;
    } else if (name == "fat32") {
        return DPartInfo::FAT32;
    } else if (name == "btrfs") {
        return DPartInfo::Btrfs;
    } else if (name == "f2fs") {
        return DPartInfo::F2FS;
    } else if (name == "hfs+") {
        return DPartInfo::HFS_Plus;
    } else if (name == "minix") {
        return DPartInfo::Minix;
    } else if (name == "nilfs2") {
        return DPartInfo::Nilfs2;
    } else if (name == "ntfs") {
        return DPartInfo::NTFS;
    } else if (name == "reiser4") {
        return DPartInfo::Reiser4;
    } else if (name == "vfat") {
        return DPartInfo::VFAT;
    }

    return DPartInfo::Unknow;
}

DPartInfo::DPartInfo()
    : d(new DPartInfoPrivate)
{

}

DPartInfo::DPartInfo(const QString &name)
    : d(new DPartInfoPrivate)
{
    const QJsonArray &block_devices = Util::getBlockDevices(Util::getDeviceByName(name));

    if (!block_devices.isEmpty())
        d->init(block_devices.first().toObject());
}

DPartInfo::DPartInfo(const DPartInfo &other)
    : d(other.d)
{

}

DPartInfo::~DPartInfo()
{

}

QString DPartInfo::device() const
{
    return Util::getDeviceByName(name());
}

DPartInfo &DPartInfo::operator=(const DPartInfo &other)
{
    d = other.d;
    return *this;
}

QString DPartInfo::name() const
{
    return d->name;
}

QString DPartInfo::kname() const
{
    return d->kname;
}

qint64 DPartInfo::totalSize() const
{
    return d->size;
}

QString DPartInfo::displaySize() const
{
    return d->sizeDisplay;
}

QString DPartInfo::typeName() const
{
    return d->typeName;
}

DPartInfo::Type DPartInfo::type() const
{
    return d->type;
}

QString DPartInfo::mountPoint() const
{
    return d->mountPoint;
}

bool DPartInfo::isMounted() const
{
    return !mountPoint().isEmpty();
}

QString DPartInfo::label() const
{
    return d->label;
}

quint64 DPartInfo::sizeStart() const
{
    return d->sizeStart;
}

quint64 DPartInfo::sizeEnd() const
{
    return d->sizeEnd;
}

quint64 DPartInfo::size() const
{
    return d->size;
}

void DPartInfo::refresh()
{
    *this = DPartInfo(name());
}

QList<DPartInfo> DPartInfo::localePartList()
{
    const QJsonArray &block_devices = Util::getBlockDevices("-l");

    QList<DPartInfo> list;

    for (const QJsonValue &value : block_devices) {
        const QJsonObject &obj = value.toObject();
        const QString &fstype = obj.value("fstype").toString();

        if (fstype.isEmpty())
            continue;

        DPartInfo info;

        info.d->init(obj);
        list << info;
    }

    return list;
}

void DPartInfo::init(const QJsonObject &obj)
{
    d->init(obj);
}

QT_BEGIN_NAMESPACE
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug deg, const DPartInfo &info)
{
    QDebugStateSaver saver(deg);
    Q_UNUSED(saver)

    deg.space() << "name:" << info.name()
                << "type:" << info.typeName()
                << "size:" << info.displaySize()
                << "mount point:" << info.mountPoint()
                << "label:" << info.label();

    return deg;
}
#endif
QT_END_NAMESPACE
