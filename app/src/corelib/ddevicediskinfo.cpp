/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define private public
#include <private/qiodevice_p.h>
#undef private

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

    bool hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode, int index = 0) const Q_DECL_OVERRIDE;
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

    bool isClosing() const;

    QProcess *process = NULL;
    QBuffer buffer;
    bool closing = false;
};

DDeviceDiskInfoPrivate::DDeviceDiskInfoPrivate(DDeviceDiskInfo *qq)
    : DDiskInfoPrivate(qq)
{

}

DDeviceDiskInfoPrivate::~DDeviceDiskInfoPrivate()
{
    closeDataStream();

    if (process)
        process->deleteLater();
}

void DDeviceDiskInfoPrivate::init(const QJsonObject &obj)
{
    model = obj.value("model").toString();
    name = obj.value("name").toString();
    kname = obj.value("kname").toString();
    size = obj.value("size").toString().toLongLong();
    typeName = obj.value("type").toString();
    readonly = obj.value("ro").toString() == "1" || typeName == "rom";
    removeable = obj.value("rm").toString() == "1";
    transport = obj.value("tran").toString();
    serial = obj.value("serial").toString();

    if (obj.value("pkname").isNull())
        type = DDiskInfo::Disk;
    else
        type = DDiskInfo::Part;

    const QJsonArray &list = obj.value("children").toArray();
    QStringList children_uuids;

    for (const QJsonValue &part : list) {
        const QJsonObject &obj = part.toObject();

        const QString &uuid = obj.value("partuuid").toString();

        if (!uuid.isEmpty() && children_uuids.contains(uuid))
            continue;

        DDevicePartInfo info;

        info.init(obj);

        if (!info.partUUID().isEmpty() && children_uuids.contains(info.partUUID()))
            continue;

        info.d->transport = transport;
        children << info;
        children_uuids << info.partUUID();
    }

    qSort(children.begin(), children.end(), [] (const DPartInfo &info1, const DPartInfo &info2) {
        return info1.sizeStart() < info2.sizeStart();
    });

    if (type == DDiskInfo::Disk)
        ptTypeName = getPTName(name);
    else
        ptTypeName = getPTName(obj.value("pkname").toString());

    if (ptTypeName == "dos") {
        ptType = DDiskInfo::MBR;
    } else if (ptTypeName == "gpt") {
        ptType = DDiskInfo::GPT;
    } else {
        ptType = DDiskInfo::Unknow;
        havePartitionTable = false;
    }

    if (type == DDiskInfo::Part)
        havePartitionTable = false;

    if ((!havePartitionTable && children.isEmpty()) || type == DDiskInfo::Part) {
        DDevicePartInfo info;

        info.init(obj);
        info.d->transport = transport;
        info.d->index = 0;
        children << info;
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

bool DDeviceDiskInfoPrivate::hasScope(DDiskInfo::DataScope scope, DDiskInfo::ScopeMode mode, int index) const
{
    if (mode == DDiskInfo::Read) {
        if (scope == DDiskInfo::Headgear) {
            return havePartitionTable && (children.isEmpty() || children.first().sizeStart() >= 1048576);
        } else if (scope == DDiskInfo::JsonInfo) {
            return true;
        }

        if (scope == DDiskInfo::PartitionTable)
            return havePartitionTable;
    } else if (readonly || scope == DDiskInfo::JsonInfo) {
        return false;
    }

    if (scope == DDiskInfo::Partition) {
        if (index == 0 && mode == DDiskInfo::Write)
            return true;

        const DPartInfo &info = q->getPartByNumber(index);

        if (!info) {
            dCDebug("Can not find parition by number(device: \"%s\"): %d", qPrintable(q->filePath()), index);

            return false;
        }

        if (info.isExtended() || (mode == DDiskInfo::Read
                                  && info.type() == DPartInfo::Unknow
                                  && info.fileSystemType() == DPartInfo::Invalid
                                  && info.guidType() == DPartInfo::InvalidGUID)) {
            dCDebug("Skip the \"%s\" partition, type: %s", qPrintable(info.filePath()), qPrintable(info.typeDescription(info.type())));

            return false;
        }

        return mode != DDiskInfo::Write || !info.isReadonly();
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
        if (isClosing())
            return;

        if (status == QProcess::CrashExit) {
            setErrorString(QObject::tr("process \"%1 %2\" crashed").arg(process->program()).arg(process->arguments().join(" ")));
        } else if (code != 0) {
            setErrorString(QObject::tr("Failed to perform process \"%1 %2\", error: %3").arg(process->program()).arg(process->arguments().join(" ")).arg(QString::fromUtf8(process->readAllStandardError())));
        }
    });

    switch (currentScope) {
    case DDiskInfo::Headgear: {
        if (type != DDiskInfo::Disk) {
            setErrorString(QObject::tr("\"%1\" is not a disk device").arg(filePath()));

            return false;
        }

        if (currentMode == DDiskInfo::Read) {
            process->start(QStringLiteral("dd if=%1 bs=512 count=2048 status=none").arg(filePath()), QIODevice::ReadOnly);
        } else {
            process->start(QStringLiteral("dd of=%1 bs=512 status=none conv=fsync").arg(filePath()));
        }

        break;
    }
    case DDiskInfo::PartitionTable: {
        if (type != DDiskInfo::Disk) {
            setErrorString(QObject::tr("\"%1\" is not a disk device").arg(filePath()));

            return false;
        }

        if (currentMode == DDiskInfo::Read)
            process->start(QStringLiteral("sfdisk -d %1").arg(filePath()), QIODevice::ReadOnly);
        else
            process->start(QStringLiteral("sfdisk %1 --no-reread").arg(filePath()));

        break;
    }
    case DDiskInfo::Partition: {
        const DPartInfo &part = (index == 0 && currentMode == DDiskInfo::Write) ? DDevicePartInfo(filePath()) : q->getPartByNumber(index);

        if (!part) {
            dCDebug("Part is null(index: %d)", index);

            return false;
        }

        dCDebug("Try open device: %s, mode: %s", qPrintable(part.filePath()), currentMode == DDiskInfo::Read ? "Read" : "Write");

        if (Helper::isMounted(part.filePath())) {
            if (Helper::umountDevice(part.filePath())) {
                part.d->mountPoint.clear();
            } else {
                setErrorString(QObject::tr("\"%1\" is busy").arg(part.filePath()));

                return false;
            }
        }

        if (currentMode == DDiskInfo::Read) {
            const QString &executer = Helper::getPartcloneExecuter(part);
            process->start(QStringLiteral("%1 -s %2 -o - -c -z %3 -L /var/log/partclone.log").arg(executer).arg(part.filePath()).arg(Global::bufferSize), QIODevice::ReadOnly);
        } else {
            process->start(QStringLiteral("partclone.restore -s - -o %2 -z %3 -L /var/log/partclone.log").arg(part.filePath()).arg(Global::bufferSize));
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
            setErrorString(QObject::tr("Failed to start \"%1 %2\", error: %3").arg(process->program()).arg(process->arguments().join(" ")).arg(process->errorString()));

            return false;
        }

        dCDebug("The \"%s %s\" command start finished", qPrintable(process->program()), qPrintable(process->arguments().join(" ")));
    }

    bool ok = process ? process->isOpen() : buffer.open(QIODevice::ReadOnly);

    if (!ok) {
        setErrorString(QObject::tr("Failed to open process, error: %1").arg(process ? process->errorString(): buffer.errorString()));
    }

    return ok;
}

void DDeviceDiskInfoPrivate::closeDataStream()
{
    closing = true;

    if (process) {
        if (process->state() != QProcess::NotRunning) {
            if (currentMode == DDiskInfo::Read) {
                process->closeReadChannel(QProcess::StandardOutput);
                process->terminate();
            } else {
                process->closeWriteChannel();
            }

            while (process->state() != QProcess::NotRunning) {
                QThread::currentThread()->sleep(1);

                if (!QFile::exists(QString("/proc/%2").arg(process->pid()))) {
                    process->waitForFinished(-1);

                    if (process->error() == QProcess::Timedout)
                        process->QIODevice::d_func()->errorString.clear();

                    break;
                }
            }
        }

        dCDebug("Process exit code: %d(%s %s)", process->exitCode(), qPrintable(process->program()), qPrintable(process->arguments().join(' ')));
    }

    if (currentMode == DDiskInfo::Write && currentScope == DDiskInfo::PartitionTable) {
        Helper::umountDevice(filePath());

        if (Helper::refreshSystemPartList(filePath())) {
            refresh();
        } else {
            dCWarning("Refresh the devcie %s failed", qPrintable(filePath()));
        }
    }

    if (currentScope == DDiskInfo::JsonInfo)
        buffer.close();

    closing = false;
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
        if (!part.isExtended())
            size += part.usedSize();
    }

    return size;
}

qint64 DDeviceDiskInfoPrivate::maxReadableDataSize() const
{
    if (children.isEmpty()) {
        return totalReadableDataSize();
    }

    if (type == DDiskInfo::Disk)
        return children.last().sizeEnd() + 1;

    return children.first().totalSize();
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

    if (process->bytesAvailable() > Global::bufferSize) {
        dCWarning("The \"%s %s\" process bytes available: %s", qPrintable(process->program()), qPrintable(process->arguments().join(" ")), qPrintable(Helper::sizeDisplay(process->bytesAvailable())));
    }

    return process->read(data, maxSize);
}

qint64 DDeviceDiskInfoPrivate::write(const char *data, qint64 maxSize)
{
    if (!process)
        return -1;

    if (process->state() != QProcess::Running)
        return -1;

    qint64 size = process->write(data, maxSize);

    QElapsedTimer timer;

    timer.start();

    int timeout = 5000;

    while (process->state() == QProcess::Running && process->bytesToWrite() > 0) {
        process->waitForBytesWritten();

        if (timer.elapsed() > timeout) {
            timeout += 5000;

            dCWarning("Wait for bytes written timeout, elapsed: %lld, bytes to write: %lld", timer.elapsed(), process->bytesToWrite());
        }
    }

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
    if (error.isEmpty()) {
        if (process) {
            if (process->error() == QProcess::UnknownError)
                return QString();

            return QString("%1 %2: %3").arg(process->program()).arg(process->arguments().join(' ')).arg(process->errorString());
        }

        if (!buffer.QIODevice::d_func()->errorString.isEmpty())
            return buffer.errorString();
    }

    return error;
}

bool DDeviceDiskInfoPrivate::isClosing() const
{
    return closing;
}

DDeviceDiskInfo::DDeviceDiskInfo()
{

}

DDeviceDiskInfo::DDeviceDiskInfo(const QString &filePath)
{
    const QJsonArray &block_devices = Helper::getBlockDevices(filePath);

    if (!block_devices.isEmpty()) {
        const QJsonObject &obj = block_devices.first().toObject();

        d = new DDeviceDiskInfoPrivate(this);
        d_func()->init(obj);

        if (d->type == Part) {
            const QJsonArray &parent = Helper::getBlockDevices(obj.value("pkname").toString());

            if (!parent.isEmpty()) {
                const QJsonObject &parent_obj = parent.first().toObject();

                d->transport = parent_obj.value("tran").toString();
                d->model = parent_obj.value("model").toString();
                d->serial = parent_obj.value("serial").toString();
            }

            if (!d->children.isEmpty())
                d->children.first().d->transport = d->transport;
        }
    }
}

QList<DDeviceDiskInfo> DDeviceDiskInfo::localeDiskList()
{
    const QJsonArray &block_devices = Helper::getBlockDevices();

    QList<DDeviceDiskInfo> list;

    for (const QJsonValue &value : block_devices) {
        const QJsonObject &obj = value.toObject();

        if (Global::disableLoopDevice && obj.value("type").toString() == "loop")
            continue;

        DDeviceDiskInfo info;

        info.d = new DDeviceDiskInfoPrivate(&info);
        info.d_func()->init(obj);
        list << info;
    }

    return list;
}
