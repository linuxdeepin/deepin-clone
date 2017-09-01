#include "ddevicediskinfo.h"
#include "ddiskinfo_p.h"
#include "helper.h"
#include "ddevicepartinfo.h"
#include "dpartinfo_p.h"

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProcess>
#include <QBuffer>

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

    // Unfulfilled
    qint64 readableDataSize(DDiskInfo::DataScope scope) const Q_DECL_OVERRIDE;

    qint64 totalReadableDataSize() const Q_DECL_OVERRIDE;
    qint64 maxReadableDataSize() const Q_DECL_OVERRIDE;
    qint64 totalWritableDataSize() const Q_DECL_OVERRIDE;

    qint64 read(char *data, qint64 maxSize) Q_DECL_OVERRIDE;
    qint64 write(const char *data, qint64 maxSize) Q_DECL_OVERRIDE;

    bool atEnd() const Q_DECL_OVERRIDE;

    QString errorString() const Q_DECL_OVERRIDE;

    QProcess *process = NULL;
    QBuffer buffer;
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
    model = obj.value("model").toString();
    name = obj.value("name").toString();
    kname = obj.value("kname").toString();
    size = obj.value("size").toString().toLongLong();
    typeName = obj.value("type").toString();
    readonly = obj.value("ro").toString() == "1";
    removeable = obj.value("rm").toString() == "1";
    transport = obj.value("tran").toString();

    if (typeName == "part") {
        type = DDiskInfo::Part;
    } else if (typeName == "disk") {
        type = DDiskInfo::Disk;
    } else if (typeName == "loop") {
        model = "Loop Disk Device";

        if (name.length() == 5)
            type = DDiskInfo::Disk;
        else
            type = DDiskInfo::Part;
    } else if (typeName == "rom") {
        type = DDiskInfo::Part;
    }

    const QJsonArray &list = obj.value("children").toArray();
    QStringList children_uuids;

    for (const QJsonValue &part : list) {
        const QJsonObject &obj = part.toObject();

        if (children_uuids.contains(obj.value("partuuid").toString()))
            continue;

        DDevicePartInfo info;

        info.init(obj);
        info.d->transport = transport;
        children << info;
        children_uuids << info.partUUID();
    }

    if (!obj.value("fstype").isNull()) {
        DDevicePartInfo info;

        info.init(obj);
        info.d->transport = transport;
        children << info;
    }

    ptTypeName = getPTName(name);

    if (ptTypeName == "dos") {
        ptType = DDiskInfo::MBR;
    } else if (ptTypeName == "gpt") {
        ptType = DDiskInfo::GPT;
    } else {
        ptType = DDiskInfo::Unknow;
        havePartitionTable = false;
    }
}

QString DDeviceDiskInfoPrivate::filePath() const
{
    return name;
}

void DDeviceDiskInfoPrivate::refresh()
{
    children.clear();

    const QJsonArray &block_devices = Helper::getBlockDevices(name);

    if (!block_devices.isEmpty())
        init(block_devices.first().toObject());
}

bool DDeviceDiskInfoPrivate::hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode) const
{
    if (mode == DDiskInfo::Read) {
        if (scope == DDiskInfo::Headgear) {
            return havePartitionTable && (children.isEmpty() || children.first().sizeStart() >= 1048576);
        } else if (scope == DDiskInfo::JsonInfo) {
            return true;
        }

        return scope == DDiskInfo::PartitionTable ? havePartitionTable : !children.isEmpty();
    } else if (readonly || scope == DDiskInfo::JsonInfo) {
        return false;
    }

    return (scope == DDiskInfo::Headgear || scope == DDiskInfo::PartitionTable) ? type == DDiskInfo::Disk : true;
}

bool DDeviceDiskInfoPrivate::openDataStream(int index)
{
    if (process) {
        process->deleteLater();
    }

    process = new QProcess();

    QObject::connect(process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     process, [this] (int code, QProcess::ExitStatus status) {
        if (status == QProcess::CrashExit) {
            setErrorString(QObject::tr("The process %1 %2 crashed").arg(process->program()).arg(process->arguments().join(" ")));
        } else if (code != 0) {
            setErrorString(QObject::tr("The \"%1 %2\" command execution failed: %3").arg(process->program(), process->arguments().join(" "), process->readAllStandardError()));
        }
    });

    switch (currentScope) {
    case DDiskInfo::Headgear: {
        if (type != DDiskInfo::Disk) {
            setErrorString(QObject::tr("%s not is disk").arg(filePath()));

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
            setErrorString(QObject::tr("%s not is disk").arg(filePath()));

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

        if (part.isMounted()) {
            if (Helper::umountDevice(part.filePath())) {
                children[index].d->mountPoint.clear();
            } else {
                setErrorString(QObject::tr("%1 is busy").arg(part.filePath()));

                return false;
            }
        }

        if (currentMode == DDiskInfo::Read) {
            const QString &executer = Helper::getPartcloneExecuter(part);
            process->start(QStringLiteral("%1 -s %2 -o - -c -z %3").arg(executer).arg(part.filePath()).arg(Global::bufferSize), QIODevice::ReadOnly);
        } else {
            process->start(QStringLiteral("partclone.restore -s - -o %2 -z %3").arg(part.filePath()).arg(Global::bufferSize), QIODevice::WriteOnly);
        }

        break;
    }
    case DDiskInfo::JsonInfo: {
        process->deleteLater();
        process = 0;
        buffer.setData(q->toJson());

        break;
    }
    default:
        return false;
    }

    if (process) {
        if (!process->waitForStarted()) {
            setErrorString(QObject::tr("The \"%1 %2\" command start failed: %3").arg(process->program(), process->arguments().join(" "), process->errorString()));

            return false;
        }
    }

    bool ok = process ? process->isOpen() : buffer.open(QIODevice::ReadOnly);

    if (!ok) {
        setErrorString(QObject::tr("Device open failed, %1").arg(process ? process->errorString(): buffer.errorString()));
    }

    return ok;
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
        } else {
            dCWarning("Refresh the devcie %s failed", qPrintable(filePath()));
        }
    }

    if (currentScope == DDiskInfo::JsonInfo)
        buffer.close();
}

qint64 DDeviceDiskInfoPrivate::readableDataSize(DDiskInfo::DataScope scope) const
{
    Q_UNUSED(scope)

    return -1;
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
            size += 512;
        } else if (ptType == DDiskInfo::GPT) {
            size += 17408;
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

    return children.last().sizeEnd() + 1;
}

qint64 DDeviceDiskInfoPrivate::totalWritableDataSize() const
{
    return size;
}

qint64 DDeviceDiskInfoPrivate::read(char *data, qint64 maxSize)
{
    if (!process) {
        return buffer.read(data, maxSize);
    }

    process->waitForReadyRead(-1);

    return process->read(data, maxSize);
}

qint64 DDeviceDiskInfoPrivate::write(const char *data, qint64 maxSize)
{
    if (!process)
        return -1;

    qint64 size = process->write(data, maxSize);

    if (size > 0)
        while (process->waitForBytesWritten());

    return size;
}

bool DDeviceDiskInfoPrivate::atEnd() const
{
    if (!process) {
        return buffer.atEnd();
    }

    process->waitForReadyRead(-1);

    return process->atEnd();
}

QString DDeviceDiskInfoPrivate::errorString() const
{
    if (error.isEmpty())
        return process ? process->errorString() : buffer.errorString();

    return error;
}

DDeviceDiskInfo::DDeviceDiskInfo()
{

}

DDeviceDiskInfo::DDeviceDiskInfo(const QString &filePath)
{
    const QJsonArray &block_devices = Helper::getBlockDevices(filePath);

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

        info.d = new DDeviceDiskInfoPrivate(&info);
        info.d_func()->init(value.toObject());
        list << info;
    }

    return list;
}
