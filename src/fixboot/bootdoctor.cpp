/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     zccrs <zccrs@live.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "bootdoctor.h"
#include "helper.h"
#include "ddevicediskinfo.h"

#include <QTemporaryDir>

QString BootDoctor::m_lastErrorString;

bool BootDoctor::fix(const QString &partDevice)
{
    m_lastErrorString.clear();

    const QString &mount_root = Helper::temporaryMountDevice(partDevice, QFileInfo(partDevice).fileName());

    if (mount_root.isEmpty()) {
        m_lastErrorString = QObject::tr("Failed to mount partition \"%1\"").arg(partDevice);

        return false;
    }

    const QStringList &tmp_paths = QStandardPaths::standardLocations(QStandardPaths::TempLocation);
    const QString tmp_dir = (tmp_paths.isEmpty() ? "/tmp" : tmp_paths.first()) + "/.deepin-clone";

    {
        if (!QDir::current().mkpath(tmp_dir)) {
            dCError("mkpath \"%s\" failed", qPrintable(tmp_dir));
            goto failed;
        }

        const QString &repo_path = tmp_dir + "/repo.iso";

        if (!QFile::exists(repo_path)
                && !QFile::copy(QString(":/repo_%1.iso").arg(HOST_ARCH), repo_path)) {
            dCError("copy file failed, new name: %s", qPrintable(repo_path));
            goto failed;
        }

        bool ok = false;

        const QString &repo_mount_point = mount_root + "/deepin-clone";
        QFile file_boot_fix(mount_root + "/boot_fix.sh");

        do {
            if (!QDir(mount_root).exists("deepin-clone") && !QDir(mount_root).mkdir("deepin-clone")) {
                dCError("Create \"deepin-clone\" dir failed(\"%s\")", qPrintable(mount_root));
                break;
            }

            if (!Helper::mountDevice(repo_path, repo_mount_point, true)) {
                m_lastErrorString = QObject::tr("Failed to mount partition \"%1\"").arg(repo_path);
                break;
            }

            if (file_boot_fix.exists()) {
                dCError("File \"%s\" is exists", qPrintable(file_boot_fix.fileName()));
                break;
            }

            if (!QFile::copy(QString(":/scripts/boot_fix_%1.sh").arg(
                     #if defined(HOST_ARCH_x86_64) || defined(HOST_ARCH_i386)
                                 "x86"
                     #elif defined(HOST_ARCH_mips64) || defined(HOST_ARCH_mips32)
                                 "mips"
                     #elif defined(HOST_ARCH_sw_64)
                                 "sw_64"
                     #else
                     #error Unknow machine
                     #endif
                                 ), file_boot_fix.fileName())) {
                dCError("copy file failed, new name: %s", qPrintable(file_boot_fix.fileName()));
                break;
            }

            if (!file_boot_fix.setPermissions(file_boot_fix.permissions() | QFile::ExeUser)) {
                dCError("Set \"%s\" permissions failed", qPrintable(file_boot_fix.fileName()));
                break;
            }

            if (Helper::processExec(QString("mount --bind -v --bind /dev/ /%1/dev").arg(mount_root)) != 0) {
                dCError("Failed to bind /dev");
                break;
            }

            if (Helper::processExec(QString("mount --bind -v --bind /dev/pts/ /%1/dev/pts").arg(mount_root)) != 0) {
                dCError("Failed to bind /dev/pts");
                break;
            }

            if (Helper::processExec(QString("mount --bind -v --bind /proc/ /%1/proc").arg(mount_root)) != 0) {
                dCError("Failed to bind /proc");
                break;
            }

            if (Helper::processExec(QString("mount --bind -v --bind /sys/ /%1/sys").arg(mount_root)) != 0) {
                dCError("Failed to bind /sys");
                break;
            }

            ok = true;
        } while (0);

        QProcess process;

        if (ok) {
            const QString &parent_device = Helper::parentDevice(partDevice);

            bool is_efi = false;

            if (!parent_device.isEmpty()) {
                DDeviceDiskInfo info(parent_device);

                if (info.ptType() == DDeviceDiskInfo::GPT) {
                    for (const DPartInfo &part : info.childrenPartList()) {
                        if (part.guidType() == DPartInfo::EFI_SP_None) {
                            if (Helper::processExec(QString("mount %1 /%2/boot/efi").arg(part.filePath()).arg(mount_root)) != 0) {
                                dCError("Failed to mount EFI partition");
                                m_lastErrorString = QObject::tr("Failed to mount partition \"%1\"").arg(part.filePath());
                                ok = false;
                                break;
                            }

                            is_efi = true;

                            break;
                        }
                    }

                    if (!is_efi && m_lastErrorString.isEmpty()) {
                        m_lastErrorString = QObject::tr("Not found EFI partition");
                        ok = false;
                    }
                }
            }

            if (ok) {
                process.start(QString("chroot %1 ./boot_fix.sh %2 %3 /deepin-clone").arg(mount_root).arg(partDevice).arg(is_efi ? "true" : "false"));
                process.waitForFinished(-1);

                switch (process.exitCode()) {
                case 1:
                    m_lastErrorString = QObject::tr("grub-install failed");
                    break;
                case 2:
                    m_lastErrorString = QObject::tr("update-grub failed");
                    break;
                default:
                    break;
                }

                dCDebug("Exec boot_fix.sh log:\n%s", process.readAll().constData());
            }
        }

        // clear
        Helper::processExec("umount " + repo_mount_point);
        QDir(mount_root).rmdir("deepin-clone");
        file_boot_fix.remove();
        Helper::processExec("umount " + mount_root + "/dev");
        Helper::processExec("umount " + mount_root + "/dev/pts");
        Helper::processExec("umount " + mount_root + "/proc");
        Helper::processExec("umount " + mount_root + "/sys");
        Helper::processExec("umount " + mount_root + "/boot/efi");

        return ok && process.exitCode() == 0;
    }

failed:
    if (m_lastErrorString.isEmpty())
        m_lastErrorString = QObject::tr("Failed to fix boot loader");

    return false;
}

QString BootDoctor::errorString()
{
    return m_lastErrorString;
}
