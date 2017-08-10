#ifndef DPARTINFO_H
#define DPARTINFO_H

#include "dpartinfo.h"

#include <QList>
#include <QExplicitlySharedDataPointer>

QT_BEGIN_NAMESPACE
class QJsonObject;
QT_END_NAMESPACE

class DPartInfoPrivate;
class DPartInfo
{
public:
    enum Type {
        Unknow,
        EXT2,
        EXT3,
        EXT4,
        FAT12,
        FAT16,
        FAT32,
        Btrfs,
        F2FS,
        HFS_Plus,
        Minix,
        Nilfs2,
        NTFS,
        Reiser4,
        VFAT
    };

    explicit DPartInfo();
    explicit DPartInfo(const QString &name);
    explicit DPartInfo(const DPartInfo &other);
    ~DPartInfo();

    DPartInfo &operator=(const DPartInfo &other);
#ifdef Q_COMPILER_RVALUE_REFS
    DPartInfo &operator=(DPartInfo &&other) Q_DECL_NOTHROW { swap(other); return *this; }
#endif

    inline void swap(DPartInfo &other) Q_DECL_NOTHROW
    { qSwap(d, other.d); }

    QString device() const;
    // device name
    QString name() const;
    // internal kernal device name
    QString kname() const;
    // total size of partition
    qint64 totalSize() const;
    // display size of partition
    QString displaySize() const;
    // file system type of partition
    QString typeName() const;
    Type type() const;
    // mount path of partition
    QString mountPoint() const;
    bool isMounted() const;
    // file system label of partition
    QString label() const;

    // unit: bytes
    quint64 sizeStart() const;
    quint64 sizeEnd() const;
    quint64 size() const;

    void refresh();

    static QList<DPartInfo> localePartList();

private:
    void init(const QJsonObject &obj);

    friend class DPartInfoPrivate;
    friend class DDiskInfoPrivate;
    friend bool operator==(const DPartInfo &first, const DPartInfo &second);
    QExplicitlySharedDataPointer<DPartInfoPrivate> d;
};

inline bool operator==(const DPartInfo &first, const DPartInfo &second)
{
    if (first.d == second.d)
        return true;
    return first.name() == second.name();
}

QT_BEGIN_NAMESPACE
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug deg, const DPartInfo &info);
#endif
QT_END_NAMESPACE

#endif // DPARTINFO_H
