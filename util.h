#ifndef UTIL_H
#define UTIL_H

#include <QByteArray>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
class QProcess;
QT_END_NAMESPACE

class DPartInfo;
class Util
{
public:
    static QByteArray processExec(QProcess *process, const QString &command, int timeout = 30000, int *exitCode = 0);
    static QByteArray processExec(const QString &command, int timeout = 30000, int *exitCode = 0);
    static QByteArray callLsblk(const QString &extraArg = QString());
    static QJsonArray getBlockDevices(const QString &commandExtraArg = QString());
    static QString getDeviceByName(const QString &name);
    static QByteArray getPartitionTable(const QString &devicePath);
    static bool setPartitionTable(const QString &devicePath, const QString &ptFile);
    static bool saveToFile(const QString &fileName, const QByteArray &data, bool override = true);
    static bool isBlockSpecialFile(const QString &fileName);
    static bool isPartcloneFile(const QString &fileName);
    static int clonePartition(const DPartInfo &part, const QString &to, bool override = true);
    static int restorePartition(const QString &from, const DPartInfo &to);
};

#endif // UTIL_H
