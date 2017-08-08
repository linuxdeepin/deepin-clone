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
        Loop,
        Disk
    };

    // partition table type
    enum PTType {
        Unknow,
        MBR,
        GPT
    };

    explicit DDiskInfo();
    explicit DDiskInfo(const QString &name);
    explicit DDiskInfo(const DDiskInfo &other);
    ~DDiskInfo();

    DDiskInfo &operator=(const DDiskInfo &other);
#ifdef Q_COMPILER_RVALUE_REFS
    DDiskInfo &operator=(DDiskInfo &&other) Q_DECL_NOTHROW { swap(other); return *this; }
#endif

    inline void swap(DDiskInfo &other) Q_DECL_NOTHROW
    { qSwap(d, other.d); }

    QString device() const;
    // device name
    QString name() const;
    // internal kernal device name
    QString kname() const;
    // total size of device
    qint64 totalSize() const;
    // display size of device
    QString displaySize() const;
    // type of device
    QString typeName() const;
    Type type() const;
    PTType ptType() const;

    QList<DPartInfo> childrenPartList() const;

    static QList<DDiskInfo> localeDiskList();

    void refresh();

private:
    friend class DDiskInfoPrivate;
    friend bool operator==(const DDiskInfo &first, const DDiskInfo &second);
    QExplicitlySharedDataPointer<DDiskInfoPrivate> d;
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
