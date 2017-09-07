#ifndef HELPER_H
#define HELPER_H

#include <QByteArray>
#include <QJsonArray>
#include <QObject>
#include <QLoggingCategory>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

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

    static QString sizeDisplay(qint64 size);

    static bool refreshSystemPartList(const QString &device = QString());
    static QString getPartcloneExecuter(const DPartInfo &info);
    static bool getPartitionSizeInfo(const DPartInfo &info, qint64 *used, qint64 *free, int *blockSize);

    static QByteArray callLsblk(const QString &extraArg = QString());
    static QJsonArray getBlockDevices(const QString &commandExtraArg = QString());

    static bool isMounted(const QString &device);
    static bool umountDevice(const QString &device);

    static QByteArray getPartitionTable(const QString &devicePath);
    static bool setPartitionTable(const QString &devicePath, const QString &ptFile);
    static bool saveToFile(const QString &fileName, const QByteArray &data, bool override = true);
    static bool isBlockSpecialFile(const QString &fileName);
    static bool isPartcloneFile(const QString &fileName);
    static bool isDiskDevice(const QString &devicePath);
    static bool isPartitionDevice(const QString &devicePath);

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
    return QString::asprintf(qPrintable(string), std::forward<Args>(args)...);
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

#define dCDebug(...) qCDebug(Helper::loggerCategory, __VA_ARGS__)

#define dCWarning(format, ...) { \
    const QString &__m = __d_asprintf__(format, ##__VA_ARGS__); \
    Helper::instance()->warning(__m); \
    qCWarning(Helper::loggerCategory, qPrintable(__m));}
#define dCError(format, ...) { \
    const QString &__m = __d_asprintf__(format, ##__VA_ARGS__); \
    Helper::instance()->warning(__m); \
    qCCritical(Helper::loggerCategory, qPrintable(__m));}

namespace DThreadUtil {
template <typename ReturnType>
class _TMP
{
public:
    template <typename Fun, typename... Args>
    static ReturnType runInNewThread(Fun fun, Args&&... args)
    {
        QFutureWatcher<ReturnType> watcher;
        QEventLoop loop;

        QObject::connect(&watcher, &QFutureWatcherBase::finished, &loop, &QEventLoop::quit);
        watcher.setFuture(QtConcurrent::run(fun, std::forward<Args>(args)...));

        loop.exec();

        return watcher.result();
    }
};

template <typename Fun, typename... Args>
auto runInNewThread(Fun fun, Args&&... args) -> decltype(fun(args...))
{
    return _TMP<decltype(fun(args...))>::runInNewThread(fun, std::forward<Args>(args)...);
}
template <typename Fun, typename... Args>
typename QtPrivate::FunctionPointer<Fun>::ReturnType runInNewThread(typename QtPrivate::FunctionPointer<Fun>::Object *obj, Fun fun, Args&&... args)
{
    return _TMP<typename QtPrivate::FunctionPointer<Fun>::ReturnType>::runInNewThread([&] {
        return (obj->*fun)(std::forward<Args>(args)...);
    });
}

}

#endif // HELPER_H
