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

#include "helper.h"
#include "dpartinfo.h"
#include "dglobal.h"

#include <QProcess>
#include <QEventLoop>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QLoggingCategory>
#include <QRegularExpression>

#define COMMAND_LSBLK QStringLiteral("/bin/lsblk -J -b -p -o NAME,KNAME,PKNAME,FSTYPE,MOUNTPOINT,LABEL,SIZE,TYPE,PARTTYPE,PARTLABEL,PARTUUID,MODEL,PHY-SEC,RO,RM,TRAN,SERIAL %1")

QByteArray Helper::m_processStandardError;
QByteArray Helper::m_processStandardOutput;

Q_LOGGING_CATEGORY(lcDeepinGhost, "deepin.ghost")
Q_GLOBAL_STATIC(Helper, _g_globalHelper)

Helper *Helper::instance()
{
    return _g_globalHelper;
}

int Helper::processExec(QProcess *process, const QString &command, int timeout)
{
    m_processStandardOutput.clear();
    m_processStandardError.clear();

    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    timer.setInterval(timeout);

    timer.connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    loop.connect(process, SIGNAL(finished(int)), &loop, SLOT(quit()));

    if (timeout > 0) {
        timer.start();
    }

    if (Global::debugLevel > 1)
        dCDebug("Exec: \"%s\", timeout: %d", qPrintable(command), timeout);

    process->start(command, QIODevice::ReadOnly);
    process->waitForStarted();

    if (process->error() != QProcess::UnknownError) {
        dCError(process->errorString());

        return -1;
    }

    loop.exec();

    if (process->state() != QProcess::NotRunning) {
        dCDebug("The \"%s\" timeout, timeout: %d", qPrintable(command), timeout);

        if (QFile::exists(QString("/proc/%1").arg(process->pid()))) {
            process->terminate();
            process->waitForFinished();
        } else {
            dCDebug("The \"%s\" is quit, but the QProcess object state is not NotRunning");
        }
    }

    m_processStandardOutput = process->readAllStandardOutput();
    m_processStandardError = process->readAllStandardError();

    if (Global::debugLevel > 1)
        dCDebug("Done: \"%s\", exit code: %d", qPrintable(command), process->exitCode());

    return process->exitCode();
}

int Helper::processExec(const QString &command, int timeout)
{
    QProcess process;

    return processExec(&process, command, timeout);
}

QByteArray Helper::lastProcessStandardOutput()
{
    return m_processStandardOutput;
}

QByteArray Helper::lastProcessStandardError()
{
    return m_processStandardError;
}

const QLoggingCategory &Helper::loggerCategory()
{
    return lcDeepinGhost();
}

void Helper::warning(const QString &message)
{
    m_warningString = message;

    emit newWarning(message);
}

void Helper::error(const QString &message)
{
    m_errorString = message;

    emit newError(message);
}

QString Helper::lastWarningString()
{
    return m_warningString;
}

QString Helper::lastErrorString()
{
    return m_errorString;
}

QString Helper::sizeDisplay(qint64 size)
{
    constexpr qreal kb = 1024;
    constexpr qreal mb = kb * 1024;
    constexpr qreal gb = mb * 1024;
    constexpr qreal tb = gb * 1024;

    if (size > tb)
        return QString::asprintf("%.2f TB", size / tb);

    if (size > gb)
        return QString::asprintf("%.2f GB", size / gb);

    if (size > mb)
        return QString::asprintf("%.2f MB", size / mb);

    if (size > kb)
        return QString::asprintf("%.2f KB", size / kb);

    return QString("%1 B").arg(size);
}

bool Helper::refreshSystemPartList(const QString &device)
{
    int code = device.isEmpty() ? processExec("partprobe") : processExec(QString("partprobe %1").arg(device));

    if (code != 0)
        return false;

    QThread::sleep(1);

    return true;
}

QString Helper::getPartcloneExecuter(const DPartInfo &info)
{
    QString executor;

    switch (info.fileSystemType()) {
    case DPartInfo::Invalid:
        break;
    case DPartInfo::Btrfs:
        executor = "btrfs";
        break;
    case DPartInfo::EXT2:
    case DPartInfo::EXT3:
    case DPartInfo::EXT4:
        executor = "extfs";
        break;
    case DPartInfo::F2FS:
        executor = "f2fs";
        break;
    case DPartInfo::FAT12:
    case DPartInfo::FAT16:
    case DPartInfo::FAT32:
        executor = "fat";
        break;
    case DPartInfo::HFS_Plus:
        executor = "hfsplus";
        break;
    case DPartInfo::Minix:
        executor = "minix";
        break;
    case DPartInfo::Nilfs2:
        executor = "nilfs2";
        break;
    case DPartInfo::NTFS:
        executor = "ntfs";
        break;
    case DPartInfo::Reiser4:
        executor = "reiser4";
        break;
    case DPartInfo::VFAT:
        executor = "vfat";
        break;
    default:
        break;
    }

    if (executor.isEmpty())
        return "partclone.imager";

    return "partclone." + executor;
}

bool Helper::getPartitionSizeInfo(const DPartInfo &info, qint64 *used, qint64 *free, int *blockSize)
{
    QProcess process;
    QStringList env_list = QProcess::systemEnvironment();

    env_list.append("LANG=C");
    process.setEnvironment(env_list);

    if (info.isMounted()) {
        process.start(QString("df -B1 -P %1").arg(info.filePath()));
        process.waitForFinished();

        if (process.exitCode() != 0) {
            dCError("Call df failed: %s", qPrintable(process.readAllStandardError()));

            return false;
        }

        QByteArray output = process.readAll();
        const QByteArrayList &lines = output.trimmed().split('\n');

        if (lines.count() != 2)
            return false;

        output = lines.last().simplified();

        const QByteArrayList &values = output.split(' ');

        if (values.count() != 6)
            return false;

        bool ok = false;

        if (used)
            *used = values.at(2).toLongLong(&ok);

        if (!ok)
            return false;

        if (free)
            *free = values.at(3).toLongLong(&ok);

        if (!ok)
            return false;

        return true;
    } else {
        process.start(QString("%1 -s %2 -c -q -C -L /tmp/partclone.log").arg(getPartcloneExecuter(info)).arg(info.filePath()));
        process.setStandardOutputFile("/dev/null");
        process.setReadChannel(QProcess::StandardError);
        process.waitForStarted();

        qint64 used_block = -1;
        qint64 free_block = -1;

        while (process.waitForReadyRead(-1)) {
            const QByteArray &data = process.readAll();

            for (QByteArray line : data.split('\n')) {
                line = line.simplified();

                if (QString::fromLatin1(line).contains(QRegularExpression("\\berror\\b"))) {
                    dCError("Call \"%s %s\" failed: \"%s\"", qPrintable(process.program()), qPrintable(process.arguments().join(' ')), line.constData());
                }

                if (line.startsWith("Space in use:")) {
                    bool ok = false;
                    const QByteArray &value = line.split(' ').value(6, "-1");

                    used_block = value.toLongLong(&ok);

                    if (!ok) {
                        dCError("String to LongLong failed, String: %s", value.constData());

                        return false;
                    }
                } else if (line.startsWith("Free Space:")) {
                    bool ok = false;
                    const QByteArray &value = line.split(' ').value(5, "-1");

                    free_block = value.toLongLong(&ok);

                    if (!ok) {
                        dCError("String to LongLong failed, String: %s", value.constData());

                        return false;
                    }
                } else if (line.startsWith("Block size:")) {
                    bool ok = false;
                    const QByteArray &value = line.split(' ').value(2, "-1");

                    int block_size = value.toInt(&ok);

                    if (!ok) {
                        dCError("String to Int failed, String: %s", value.constData());

                        return false;
                    }

                    if (used_block < 0 || free_block < 0 || block_size < 0)
                        return false;

                    if (used)
                        *used = used_block * block_size;

                    if (free)
                        *free = free_block * block_size;

                    if (blockSize)
                        *blockSize = block_size;

                    process.terminate();
                    process.waitForFinished();

                    return true;
                }
            }
        }
    }

    return false;
}

QByteArray Helper::callLsblk(const QString &extraArg)
{
    processExec(COMMAND_LSBLK.arg(extraArg));

    return lastProcessStandardOutput();
}

QJsonArray Helper::getBlockDevices(const QString &commandExtraArg)
{
    const QByteArray &array = Helper::callLsblk(commandExtraArg);

    QJsonParseError error;

    const QJsonDocument &jd = QJsonDocument::fromJson(array, &error);

    if (error.error != QJsonParseError::NoError) {
        dCError(error.errorString());
    }

    return jd.object().value("blockdevices").toArray();
}

QString Helper::mountPoint(const QString &device)
{
    const QJsonArray &array = getBlockDevices(device);

    if (array.isEmpty())
        return QString();

    return array.first().toObject().value("mountpoint").toString();
}

bool Helper::isMounted(const QString &device)
{
    const QJsonArray &array = getBlockDevices("-l " + device);

    for (const QJsonValue &part : array) {
        const QJsonObject &obj = part.toObject();

        if (!obj.value("mountpoint").isNull())
            return true;
    }

    return false;
}

bool Helper::umountDevice(const QString &device)
{
    const QJsonArray &array = getBlockDevices("-l " + device);

    for (const QJsonValue &device : array) {
        const QJsonObject &obj = device.toObject();

        if (!obj.value("mountpoint").isNull()) {
            if (processExec(QString("umount -d %1").arg(obj.value("name").toString())) != 0)
                return false;
        }
    }

    return true;
}

bool Helper::tryUmountDevice(const QString &device)
{
    const QJsonArray &array = getBlockDevices("-l " + device);

    for (const QJsonValue &device : array) {
        const QJsonObject &obj = device.toObject();

        if (!obj.value("mountpoint").isNull()) {
            if (processExec(QString("umount -d %1 --fake").arg(obj.value("name").toString())) != 0)
                return false;
        }
    }

    return true;
}

bool Helper::mountDevice(const QString &device, const QString &path)
{
    return processExec(QString("mount %1 %2").arg(device, path)) == 0;
}

QString Helper::findDiskBySerialNumber(const QString &serialNumber, int partIndex)
{
    const QJsonArray &array = getBlockDevices("-x NAME");

    for (const QJsonValue &disk : array) {
        const QJsonObject &obj = disk.toObject();

        if (obj.value("serial").toString().compare(serialNumber, Qt::CaseInsensitive) != 0) {
            continue;
        }

        if (partIndex < 0)
            return obj.value("name").toString();

        const QJsonArray &children = obj.value("children").toArray();

        if (partIndex >= children.count())
            return QString();

        return children.at(partIndex).toObject().value("name").toString();
    }

    return QString();
}

int Helper::partitionIndex(const QString &partDevice)
{
    const QJsonArray &array = getBlockDevices(partDevice);

    if (array.isEmpty())
        return -1;

    const QJsonArray &p_array = getBlockDevices(array.first().toObject().value("pkname").toString() + " -x NAME");

    if (p_array.isEmpty())
        return -1;

    const QJsonArray &part_list = p_array.first().toObject().value("children").toArray();

    for (int i = 0; i < part_list.count(); ++i) {
        const QJsonObject &obj = part_list.at(i).toObject();

        if (obj.value("name").toString() == partDevice || obj.value("kname").toString() == partDevice)
            return i;
    }

    return -1;
}

QByteArray Helper::getPartitionTable(const QString &devicePath)
{
    processExec(QStringLiteral("/sbin/sfdisk -d %1").arg(devicePath));

    return lastProcessStandardOutput();
}

bool Helper::setPartitionTable(const QString &devicePath, const QString &ptFile)
{
    QProcess process;

    process.setStandardInputFile(ptFile);

    if (processExec(&process, QStringLiteral("/sbin/sfdisk %1").arg(devicePath)) != 0)
        return false;

    int code = processExec(QStringLiteral("/sbin/partprobe %1").arg(devicePath));
    processExec("sleep 1");

    return code == 0;
}

bool Helper::saveToFile(const QString &fileName, const QByteArray &data, bool override)
{
    if (!override && QFile::exists(fileName))
        return false;

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        dCError(file.errorString());

        return false;
    }

    qint64 size = file.write(data);
    file.flush();
    file.close();

    return size == data.size();
}

bool Helper::isBlockSpecialFile(const QString &fileName)
{
    if (fileName.startsWith("/dev/"))
        return true;

    processExec(QStringLiteral("env LANG=C stat -c %F %1").arg(fileName));

    return lastProcessStandardOutput() == "block special file\n";
}

bool Helper::isPartcloneFile(const QString &fileName)
{
    return processExec(QStringLiteral("partclone.info %1").arg(fileName)) == 0;
}

bool Helper::isDiskDevice(const QString &devicePath)
{
    const QJsonArray &blocks = getBlockDevices(devicePath);

    if (blocks.isEmpty())
        return false;

    if (!blocks.first().isObject())
        return false;

    return blocks.first().toObject().value("pkname").isNull();
}

bool Helper::isPartitionDevice(const QString &devicePath)
{
    const QJsonArray &blocks = getBlockDevices(devicePath);

    if (blocks.isEmpty())
        return false;

    if (!blocks.first().isObject())
        return false;

    return !blocks.first().toObject().value("pkname").isString();
}

QString Helper::parentDevice(const QString &device)
{
    const QJsonArray &blocks = getBlockDevices(device);

    if (blocks.isEmpty())
        return device;

    const QString &parent = blocks.first().toObject().value("pkname").toString();

    if (parent.isEmpty())
        return device;

    return parent;
}

bool Helper::deviceHaveKinship(const QString &device1, const QString &device2)
{
    return device1 == device2 || parentDevice(device1) == parentDevice(device2);
}

int Helper::clonePartition(const DPartInfo &part, const QString &to, bool override)
{
    QString executor;

    switch (part.fileSystemType()) {
    case DPartInfo::Invalid:
        break;
    case DPartInfo::Btrfs:
        executor = "btrfs";
        break;
    case DPartInfo::EXT2:
    case DPartInfo::EXT3:
    case DPartInfo::EXT4:
        executor = "extfs";
        break;
    case DPartInfo::F2FS:
        executor = "f2fs";
        break;
    case DPartInfo::FAT12:
    case DPartInfo::FAT16:
    case DPartInfo::FAT32:
        executor = "fat";
        break;
    case DPartInfo::HFS_Plus:
        executor = "hfsplus";
        break;
    case DPartInfo::Minix:
        executor = "minix";
        break;
    case DPartInfo::Nilfs2:
        executor = "nilfs2";
        break;
    case DPartInfo::NTFS:
        executor = "ntfs";
        break;
    case DPartInfo::Reiser4:
        executor = "reiser4";
        break;
    case DPartInfo::VFAT:
        executor = "vfat";
        break;
    default:
        return -1;
    }

    QString command;

    if (executor.isEmpty()) {
        if (part.guidType() == DPartInfo::InvalidGUID)
            return -1;

        command = QStringLiteral("dd if=%1 of=%2 status=none conv=fsync").arg(part.filePath()).arg(to);
    } else if (isBlockSpecialFile(to)) {
        command = QStringLiteral("/usr/sbin/partclone.%1 -b -c -s %2 -%3 %4").arg(executor).arg(part.filePath()).arg(override ? "O" : "o").arg(to);
    } else {
        command = QStringLiteral("/usr/sbin/partclone.%1 -c -s %2 -%3 %4").arg(executor).arg(part.filePath()).arg(override ? "O" : "o").arg(to);
    }

    int code = processExec(command);

    if (code != 0)
        qDebug() << command << QString::fromUtf8(lastProcessStandardOutput());

    return code;
}

int Helper::restorePartition(const QString &from, const DPartInfo &to)
{
    QString command;

    if (isPartcloneFile(from)) {
        command = QStringLiteral("/usr/sbin/partclone.restore -s %1 -o %2").arg(from).arg(to.filePath());
    } else {
        command = QStringLiteral("dd if=%1 of=%2 status=none conv=fsync").arg(from).arg(to.filePath());
    }

    int code = processExec(command);

    if (code != 0)
        qDebug() << command << QString::fromUtf8(lastProcessStandardOutput());

    return code;
}

bool Helper::existLiveSystem()
{
    QFile file("/boot/grub/grub.cfg");

    if (file.open(QIODevice::ReadOnly)) {
        while (!file.atEnd())
            if (file.readLine().contains("Deepin Recovery"))
                return true;
    }

    return false;
}

bool Helper::restartToLiveSystem(const QByteArray &autoStart)
{
    if (!existLiveSystem())
        return false;

    QFile file("/boot/deepin/autostart/deepin-clone.sh");

    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QByteArray data = "/bin/sh\n" + autoStart;

    file.write(data);

    return processExec("grub-reboot 'Deepin Recovery'") == 0;
}
