#ifndef HELPER_H
#define HELPER_H

#include <QByteArray>
#include <QJsonArray>
#include <QObject>
#include <QLoggingCategory>

#include <typeinfo>

QT_BEGIN_NAMESPACE
class QProcess;
QT_END_NAMESPACE

class DPartInfo;
class Helper : public QObject
{
    Q_OBJECT

public:
    static Helper *instance();

    static int processExec(QProcess *process, const QString &command, int timeout = -1);
    static int processExec(const QString &command, int timeout = -1);
    static QByteArray lastProcessStandardOutput();
    static QByteArray lastProcessStandardError();

    static const QLoggingCategory &loggerCategory();

    void warning(const QString &message);
    void error(const QString &message);
    QString lastWarningString();
    QString lastErrorString();

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

signals:
    void newWarning(const QString &message);
    void newError(const QString &message);

private:
    static QByteArray m_processStandardOutput;
    static QByteArray m_processStandardError;

    QString m_warningString;
    QString m_errorString;
};

template<typename... Args>
static QString __d_asprintf__(const QString &string, Args&&... args)
{
    return QString::asprintf(string.toUtf8().constData(), std::forward<Args>(args)...);
}
template<typename... Args>
static QString __d_asprintf__(const QByteArray &array, Args&&... args)
{
    return QString::asprintf(array.constData(), std::forward<Args>(args)...);
}
template<typename... Args>
static QString __d_asprintf__(const char *format, Args&&... args)
{
    return QString::asprintf(format, std::forward<Args>(args)...);
}

#define dCWarning(format, ...) { \
    const QString &__m = __d_asprintf__(format, ##__VA_ARGS__); \
    Helper::instance()->warning(__m); \
    qCWarning(Helper::loggerCategory, __m.toUtf8().constData());}
#define dCError(format, ...) { \
    const QString &__m = __d_asprintf__(format, ##__VA_ARGS__); \
    Helper::instance()->warning(__m); \
    qCCritical(Helper::loggerCategory, __m.toUtf8().constData());}

#endif // HELPER_H
