#ifndef DPARTINFO_H
#define DPARTINFO_H

#include "dpartinfo.h"

#include <QList>
#include <QExplicitlySharedDataPointer>

class DPartInfoPrivate;
class DPartInfo
{
public:
    enum Type {
        Invalid,
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

    enum GUIDType {
        InvalidGUID = 0,
        GUIDTypeBegin = 1,
        // None
        Unused_None = GUIDTypeBegin,
        MBR_PS_None = 2,
        EFI_SP_None = 3,
        BIOS_BP_None = 4,
        iFFS_None = 5,
        Sony_BP_None = 6,
        Lenove_BP_None = 7,
        // Windows
        MSR_Win = 8,
        BasicData_Win = 9,
        LDM_Win = 10,
        LDM_DP_Win = 11,
        WRE_Win = 12,
        IBM_GPFS_Win = 13,
        SSP_Win = 14,
        // HP-UX
        DP_HPUX = 15,
        SP_HPUX = 16,
        // Linux
        LFD_Linux = 17,
        RAID_P_Linux = 18,
        RP_x86_Linux = 19,
        RP_x86_64_Linux = 20,
        RP_32bit_ARM_Linux = 21,
        RP_64bit_ARM_Linux = 22,
        SP_Linux = 23,
        LVM_P_Linux = 24,
        Home_P_Linux = 25,
        Srv_P_Linux = 26,
        Plain_DC_P_Linux = 27,
        LUKS_P_Linux = 28,
        Reserved_Linux = 29,
        // FreeBSD
        BP_FreeBSD = 30,
        DP_FreeBSD = 31,
        SP_FreeBSD = 32,
        UFS_P_FreeBSD = 33,
        VVM_P_FreeBSD = 34,
        ZFS_P_FreeBSD = 35,
        // macOS Darwin
        HFS_PLUS_P_Mac = 36,
        UFS_Mac = 37,
        ZFS_Mac = 38,
        RAID_P_Mac = 39,
        RAID_P_Offline_Mac = 40,
        BP_Mac = 41,
        Label_Mac = 42,
        TV_RP_Mac = 43,
        CS_P_Mac = 44,
        SoftRAID_Status_Mac = 45,
        SoftRAID_Scratch_Mac = 46,
        SoftRAID_Volume_Mac = 47,
        SoftRAID_Cache_Mac = 48,
        // Solaris illumos
        BP_Solaris = 49,
        RP_Solaris = 50,
        SP_Solaris = 51,
        Backup_P_Solaris = 52,
        Usr_P_Solaris = ZFS_Mac,
        Var_P_Solaris = 53,
        Home_P_Solaris = 54,
        AS_Solaris = 55,
        Reserved_Solaris = 56,
        // NetBSD
        SP_NetBSD = 57,
        FFS_P_NetBSD = 58,
        LFS_P_NetBSD = 59,
        RAID_P_NetBSD = 60,
        CP_NetBSD = 61,
        EP_NetBSD = 62,
        // ChromeOS
        Kernel_ChromeOS = 63,
        Rootfs_ChromeOS = 64,
        FU_ChromeOS = 65,
        // Haiku
        BFS_Haiku = 66,
        // MidnightBSD
        BP_MidnightBSD = 67,
        DP_MidnightBSD = 68,
        SP_MidnightBSD = 69,
        UFS_P_MidnightBSD = 70,
        VVM_P_MidnightBSD = 71,
        ZFS_P_MidnightBSD = 72,
        // Ceph
        Journal_Ceph = 73,
        DC_EJ_Ceph = 74,
        OSD_Ceph = 75,
        DC_OSD_Ceph = 76,
        DIC_Ceph = 77,
        DC_DIC_Ceph = 78,
        // OpenBSD
        DP_OpenBSD = 79,
        // QNX
        PAFS_QNX = 80,
        // Plan9
        Partition_Plan9 = 81,
        // VMware ESX
        Vmkcore_VMware = 82,
        VMFS_VMware = 83,
        Reserved_VMware = 84,
        // Android-IA
        Bootloader_Android = 85,
        Bottloader2_Android = 86,
        Boot_Android = 87,
        Recovery_Android = 88,
        Misc_Android = 89,
        Metadata_Android = 90,
        System_Android = 91,
        Cache_Android = 92,
        Data_Android = 93,
        Persistent_Android = 94,
        Factory_Android = 95,
        Fastboot_Android = 96,
        OEM_Android = 97,
        // Open Network Install Environment (ONIE)
        Boot_ONIE = 98,
        Config_ONIE = 99,
        // PowerPC
        Boot_PowerPC = 100,
        // freedesktop.org OSes (Linux, etc.)
        SBLC_OSes = 101,
        // Atari TOS
        BD_P_Atari = 102,
        GUIDTypeEnd
    };

    explicit DPartInfo();
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
    // partition label
    QString partLabel() const;
    GUIDType guidType() const;

    // unit: bytes
    quint64 sizeStart() const;
    quint64 sizeEnd() const;
    quint64 size() const;

    void refresh();

    static GUIDType guidType(const QByteArray &guid);
    static QString guidTypeDescription(GUIDType type);

protected:
    DPartInfo(DPartInfoPrivate *dd);

    QExplicitlySharedDataPointer<DPartInfoPrivate> d;

private:
    friend class DPartInfoPrivate;
    friend bool operator==(const DPartInfo &first, const DPartInfo &second);
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
