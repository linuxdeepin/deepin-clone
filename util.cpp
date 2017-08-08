#include "util.h"
#include "dpartinfo.h"

#include <QProcess>
#include <QEventLoop>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>

#define COMMAND_LSBLK QStringLiteral("/bin/lsblk -J -o NAME,KNAME,FSTYPE,MOUNTPOINT,LABEL,SIZE,TYPE %1")

QByteArray Util::processExec(QProcess *process, const QString &command, int timeout, int *exitCode)
{
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    timer.setInterval(timeout);

    timer.connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    loop.connect(process, SIGNAL(finished(int)), &loop, SLOT(quit()));

    if (timeout > 0) {
        timer.start();
    }

    process->start(command, QIODevice::ReadOnly);

    loop.exec();

    if (exitCode)
        *exitCode = process->exitCode();

    return process->readAll();
}

QByteArray Util::processExec(const QString &command, int timeout, int *exitCode)
{
    QProcess process;

    return processExec(&process, command, timeout, exitCode);
}

QByteArray Util::callLsblk(const QString &extraArg)
{
    return processExec(COMMAND_LSBLK.arg(extraArg));
}

QJsonArray Util::getBlockDevices(const QString &commandExtraArg)
{
    const QByteArray &array = Util::callLsblk(commandExtraArg);

    QJsonParseError error;

    const QJsonDocument &jd = QJsonDocument::fromJson(array, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << array;
        qWarning() << error.errorString();
    }

    return jd.object().value("blockdevices").toArray();
}

QString Util::getDeviceByName(const QString &name)
{
    if (name.startsWith("/") || isBlockSpecialFile(name))
        return name;

    return QStringLiteral("/dev/%1").arg(name);
}

QByteArray Util::getPartitionTable(const QString &devicePath)
{
    return processExec(QStringLiteral("/sbin/sfdisk -d %1").arg(devicePath));
}

bool Util::setPartitionTable(const QString &devicePath, const QString &ptFile)
{
    int code = -1;

    QProcess process;

    process.setStandardInputFile(ptFile);
    processExec(&process, QStringLiteral("/sbin/sfdisk %1").arg(devicePath), 30000, &code);

    if (code != 0)
        return false;

    processExec(QStringLiteral("/sbin/partprobe %1").arg(devicePath), 30000, &code);

    return code == 0;
}

bool Util::saveToFile(const QString &fileName, const QByteArray &data, bool override)
{
    if (!override && QFile::exists(fileName))
        return false;

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << file.errorString();

        return false;
    }

    qint64 size = file.write(data);
    file.flush();
    file.close();

    return size == data.size();
}

bool Util::isBlockSpecialFile(const QString &fileName)
{
    return processExec(QStringLiteral("env LANG=C stat -c %F %1").arg(fileName)) == "block special file\n";
}

int Util::clonePartition(const DPartInfo &part, const QString &to, bool override)
{
    QString executor;

    switch (part.type()) {
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

    if (isBlockSpecialFile(to)) {
        command = QStringLiteral("/usr/sbin/partclone.%1 -b -c -s %2 -%3 %4").arg(executor).arg(part.device()).arg(override ? "O" : "o").arg(to);
    } else {
        command = QStringLiteral("/usr/sbin/partclone.%1 -c -s %2 -%3 %4").arg(executor).arg(part.device()).arg(override ? "O" : "o").arg(to);
    }

    int code = -1;

    processExec(command, -1, &code);

    return code;
}

int Util::restorePartition(const QString &from, const QString &to)
{
    int code = -1;

    processExec(QStringLiteral("/usr/sbin/partclone.restore -s %1 -o %2").arg(from).arg(to), -1, &code);

    return code;
}
