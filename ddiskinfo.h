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
        Partition
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

    inline void swap(DDiskInfo &other) Q_DECL_NOTHROW
    { qSwap(d, other.d); }

    DataScope currentScope() const;
    bool hasScope(DataScope scope, ScopeMode mode = Read) const;
    bool beginScope(DataScope scope, ScopeMode mode = Read, int index = 0);
    void endScope();

    quint64 totalReadableDataSize() const;
    quint64 totalWritableDataSize() const;

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
    quint64 totalSize() const;
    // type of device
    QString typeName() const;
    Type type() const;
    PTType ptType() const;

    QList<DPartInfo> childrenPartList() const;

    void refresh();

    inline operator bool() const
    { return d;}

protected:
    explicit DDiskInfo(DDiskInfoPrivate *dd);

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
