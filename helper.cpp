#include "helper.h"
#include "dpartinfo.h"

#include <QProcess>
#include <QEventLoop>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QLoggingCategory>

#define COMMAND_LSBLK QStringLiteral("/bin/lsblk -J -b -o NAME,KNAME,FSTYPE,MOUNTPOINT,LABEL,SIZE,TYPE,PARTTYPE,PARTLABEL %1")

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

    process->start(command, QIODevice::ReadOnly);
    process->waitForStarted();

    if (process->error() != QProcess::UnknownError) {
        dCError(process->errorString());

        return -1;
    }

    loop.exec();

    m_processStandardOutput = process->readAllStandardOutput();
    m_processStandardError = process->readAllStandardError();

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

QString Helper::sizeDisplay(quint64 size)
{
    return QString("%1 bytes").arg(size);
}

bool Helper::refreshSystemPartList(const QString &device)
{
    int code = device.isEmpty() ? processExec("partprobe") : processExec(QString("partprobe %1").arg(device));

    if (code != 0)
        return false;

    processExec("sleep 1");

    return true;
}

QString Helper::getPartcloneExecuter(const DPartInfo &info)
{
    QString executor;

    switch (info.type()) {
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

QString Helper::getDeviceByName(const QString &name)
{
    if (name.startsWith("/") || isBlockSpecialFile(name))
        return name;

    return QStringLiteral("/dev/%1").arg(name);
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
    processExec(QStringLiteral("env LANG=C stat -c %F %1").arg(fileName));

    return lastProcessStandardOutput() == "block special file\n";
}

bool Helper::isPartcloneFile(const QString &fileName)
{
    return processExec(QStringLiteral("partclone.info %1").arg(fileName)) == 0;
 }

int Helper::clonePartition(const DPartInfo &part, const QString &to, bool override)
{
    QString executor;

    switch (part.type()) {
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
