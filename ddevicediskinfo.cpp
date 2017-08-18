#include "ddevicediskinfo.h"
#include "ddiskinfo_p.h"
#include "helper.h"
#include "ddevicepartinfo.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>

static QString getPTName(const QString &device)
{
    Helper::processExec(QStringLiteral("/sbin/blkid -p -s PTTYPE -d -i %1").arg(device));

    const QByteArray &data = Helper::lastProcessStandardOutput();

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
    ~DDeviceDiskInfoPrivate();

    void init(const QJsonObject &obj);

    QString filePath() const Q_DECL_OVERRIDE;
    void refresh() Q_DECL_OVERRIDE;

    bool hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode) const Q_DECL_OVERRIDE;
    bool openDataStream(int index) Q_DECL_OVERRIDE;
    void closeDataStream() Q_DECL_OVERRIDE;

    qint64 totalReadableDataSize() const Q_DECL_OVERRIDE;
    qint64 maxReadableDataSize() const Q_DECL_OVERRIDE;
    qint64 totalWritableDataSize() const Q_DECL_OVERRIDE;

    qint64 read(char *data, qint64 maxSize) Q_DECL_OVERRIDE;
    qint64 write(const char *data, qint64 maxSize) Q_DECL_OVERRIDE;

    bool atEnd() const Q_DECL_OVERRIDE;

    QProcess *process = NULL;
};

DDeviceDiskInfoPrivate::DDeviceDiskInfoPrivate(DDeviceDiskInfo *qq)
    : DDiskInfoPrivate(qq)
{

}

DDeviceDiskInfoPrivate::~DDeviceDiskInfoPrivate()
{
    closeDataStream();
}

void DDeviceDiskInfoPrivate::init(const QJsonObject &obj)
{
    name = obj.value("name").toString();
    kname = obj.value("kname").toString();
    size = obj.value("size").toString().toLongLong();
    typeName = obj.value("type").toString();

    if (typeName == "part") {
        type = DDiskInfo::Part;
    } else if (typeName == "disk") {
        type = DDiskInfo::Disk;
    } else if (typeName == "loop") {
        if (name.length() == 5)
            type = DDiskInfo::Disk;
        else
            type = DDiskInfo::Part;
    }

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
        havePartitionTable = false;
    }

    ptTypeName = getPTName(Helper::getDeviceByName(name));

    if (ptTypeName == "dos")
        ptType = DDiskInfo::MBR;
    else if (ptTypeName == "gpt")
        ptType = DDiskInfo::GPT;
    else
        ptType = DDiskInfo::Unknow;
}

QString DDeviceDiskInfoPrivate::filePath() const
{
    return Helper::getDeviceByName(name);
}

void DDeviceDiskInfoPrivate::refresh()
{
    children.clear();

    const QJsonArray &block_devices = Helper::getBlockDevices(Helper::getDeviceByName(name));

    if (!block_devices.isEmpty())
        init(block_devices.first().toObject());
}

bool DDeviceDiskInfoPrivate::hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode) const
{
    if (mode == DDiskInfo::Read) {
        if (scope == DDiskInfo::Headgear) {
            return havePartitionTable && (children.isEmpty() || children.first().sizeStart() >= 1048576);
        }

        return scope == DDiskInfo::PartitionTable ? havePartitionTable : !children.isEmpty();
    }

    return (scope == DDiskInfo::Headgear || scope == DDiskInfo::PartitionTable) ? type == DDiskInfo::Disk : true;
}

bool DDeviceDiskInfoPrivate::openDataStream(int index)
{
    if (process) {
        process->deleteLater();
    }

    process = new QProcess();

    switch (currentScope) {
    case DDiskInfo::Headgear: {
        if (type != DDiskInfo::Disk) {
            dCError("%s not is disk", qPrintable(filePath()));

            return false;
        }

        if (currentMode == DDiskInfo::Read) {
            process->start(QStringLiteral("dd if=%1 bs=512 count=2048 status=none").arg(filePath()), QIODevice::ReadOnly);
        } else {
            process->start(QStringLiteral("dd of=%1 bs=512 status=none conv=fsync").arg(filePath()), QIODevice::WriteOnly);
        }

        break;
    }
    case DDiskInfo::PartitionTable: {
        if (type != DDiskInfo::Disk) {
            dCError("%s not is disk", qPrintable(filePath()));

            return false;
        }

        if (currentMode == DDiskInfo::Read)
            process->start(QStringLiteral("sfdisk -d -J %1").arg(filePath()), QIODevice::ReadOnly);
        else
            process->start(QStringLiteral("sfdisk %1").arg(filePath()), QIODevice::WriteOnly);

        break;
    }
    case DDiskInfo::Partition: {
        if (index >= children.count())
            return false;

        const DPartInfo &part = children.at(index);

        if (currentMode == DDiskInfo::Read) {
            const QString &executer = Helper::getPartcloneExecuter(part);
            process->start(QStringLiteral("%1 -s %2 -o - -c").arg(executer).arg(part.filePath()), QIODevice::ReadOnly);
        } else {
            process->start(QStringLiteral("partclone.restore -s - -o %2").arg(part.filePath()), QIODevice::WriteOnly);
        }

        break;
    }
    default:
        return false;
    }

    process->waitForStarted();

    return process->isOpen();
}

void DDeviceDiskInfoPrivate::closeDataStream()
{
    if (process) {
        if (process->state() != QProcess::NotRunning) {
            process->terminate();
            process->waitForFinished();
        }

        process->deleteLater();
        process = NULL;
    }

    if (currentMode == DDiskInfo::Write && currentScope == DDiskInfo::PartitionTable) {
        if (Helper::refreshSystemPartList(filePath())) {
            refresh();
        }
    }
}

qint64 DDeviceDiskInfoPrivate::totalReadableDataSize() const
{
    qint64 size = 0;

    if (hasScope(DDiskInfo::PartitionTable, DDiskInfo::Read)) {
        if (hasScope(DDiskInfo::Headgear, DDiskInfo::Read)) {
            size += 1048576;
        } else if (!children.isEmpty()) {
            size += children.first().sizeStart();
        }

        if (ptType == DDiskInfo::MBR) {
            size = qMax(size, qint64(512));
        } else if (ptType == DDiskInfo::GPT) {
            size = qMax(size, qint64(17408));
            size += 16896;
        }
    }

    for (const DPartInfo &part : children) {
        size += part.usedSize() + part.totalSize() / part.blockSize() + 100000;
    }

    return size;
}

qint64 DDeviceDiskInfoPrivate::maxReadableDataSize() const
{
    if (children.isEmpty()) {
        return totalReadableDataSize();
    }

    return children.last().sizeEnd();
}

qint64 DDeviceDiskInfoPrivate::totalWritableDataSize() const
{
    return size;
}

qint64 DDeviceDiskInfoPrivate::read(char *data, qint64 maxSize)
{
    if (!process)
        return -1;

    process->waitForReadyRead(-1);

    return process->read(data, maxSize);
}

qint64 DDeviceDiskInfoPrivate::write(const char *data, qint64 maxSize)
{
    if (!process)
        return -1;

    process->waitForBytesWritten(-1);

    return process->write(data, maxSize);
}

bool DDeviceDiskInfoPrivate::atEnd() const
{
    if (!process)
        return true;

    process->waitForReadyRead(-1);

    return process->atEnd();
}

DDeviceDiskInfo::DDeviceDiskInfo()
{

}

DDeviceDiskInfo::DDeviceDiskInfo(const QString &name)
{
    const QJsonArray &block_devices = Helper::getBlockDevices(Helper::getDeviceByName(name));

    if (!block_devices.isEmpty()) {
        d = new DDeviceDiskInfoPrivate(this);
        d_func()->init(block_devices.first().toObject());
    }
}

QList<DDeviceDiskInfo> DDeviceDiskInfo::localeDiskList()
{
    const QJsonArray &block_devices = Helper::getBlockDevices();

    QList<DDeviceDiskInfo> list;

    for (const QJsonValue &value : block_devices) {
        DDeviceDiskInfo info;

        info.d_func()->init(value.toObject());
        list << info;
    }

    return list;
}
