#ifndef DDISKINFO_H
#define DDISKINFO_H

#include "dpartinfo.h"

#include <QList>
#include <QExplicitlySharedDataPointer>

class DDiskInfoPrivate;
class DDiskInfo
{
public:
    enum Type {
        Part,
        Disk
    };

    // partition table type
    enum PTType {
        Unknow,
        MBR,
        GPT
    };

    enum DataScope {
        NullScope,
        Headgear,
        PartitionTable,
        Partition,
        JsonInfo
    };

    enum ScopeMode {
        Read,
        Write
    };

    explicit DDiskInfo();
    DDiskInfo(const DDiskInfo &other);
    ~DDiskInfo();

    DDiskInfo &operator=(const DDiskInfo &other);
#ifdef Q_COMPILER_RVALUE_REFS
    DDiskInfo &operator=(DDiskInfo &&other) Q_DECL_NOTHROW { swap(other); return *this; }
#endif

    void swap(DDiskInfo &other);

    DataScope currentScope() const;
    bool hasScope(DataScope scope, ScopeMode mode = Read) const;
    bool beginScope(DataScope scope, ScopeMode mode = Read, int index = 0);
    void endScope();
    qint64 readableDataSize(DataScope scope) const;

    qint64 totalReadableDataSize() const;
    qint64 maxReadableDataSize() const;
    qint64 totalWritableDataSize() const;
    bool setTotalWritableDataSize(qint64 size);

    qint64 read(char *data, qint64 maxSize);

    qint64 write(const char *data, qint64 maxSize);
    qint64 write(const char *data);

    bool atEnd() const;

    QString filePath() const;
    // device name
    QString name() const;
    // internal kernal device name
    QString kname() const;
    // total size of device
    qint64 totalSize() const;
    // type of device
    QString typeName() const;
    Type type() const;
    PTType ptType() const;

    QList<DPartInfo> childrenPartList() const;

    void refresh();

    QByteArray toJson() const;

    QString errorString() const;

    inline operator bool() const
    { return d;}

    static DDiskInfo getInfo(const QString &file);

protected:
    explicit DDiskInfo(DDiskInfoPrivate *dd);
    static void fromJson(const QByteArray &json, DDiskInfoPrivate *dd);

    QExplicitlySharedDataPointer<DDiskInfoPrivate> d;

private:
    friend class DDiskInfoPrivate;
    friend bool operator==(const DDiskInfo &first, const DDiskInfo &second);
};

inline bool operator==(const DDiskInfo &first, const DDiskInfo &second)
{
    if (first.d == second.d)
        return true;
    return first.name() == second.name();
}

QT_BEGIN_NAMESPACE
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug deg, const DDiskInfo &info);
#endif
QT_END_NAMESPACE

#endif // DDISKINFO_H
