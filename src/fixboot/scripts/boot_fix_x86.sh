#!/bin/bash

error () {
    echo $1
    exit $2
}

DI_BOOTLOADER=$1
disk_is_efi=$2
repo_path=$3

if [[ disk_is_efi == "true" ]]; then
    echo "disk is efi mode"
    disk_is_efi=1
else
    echo "disk is legacy mode"
    disk_is_efi=0
fi

dpkg-query -l grub-pc|egrep -i ii\\s+grub-pc
system_is_efi=$?

if [[ system_is_efi == 0 ]]; then
    echo "system is legacy mode"
    system_is_efi=0
else
    echo "system is efi mode"
    system_is_efi=1
fi

set -e

if [[ $disk_is_efi != $system_is_efi ]]; then
    cp /etc/apt/sources.list /tmp/sources.list.bak
    echo "deb file://${repo_path} unstable main contrib non-free" > /etc/apt/sources.list
    apt-get update

    export DEBIAN_FRONTEND="noninteractive"

    if [[ $disk_is_efi == 0 ]]; then
        echo "INFO: Detected legacy machine, installing grub to ${DI_BOOTLOADER}"
        apt-get -y -o Dpkg::Options::="--force-confdef" \
            -o Dpkg::Options::="--force-confold" --no-install-recommends \
            --allow-unauthenticated install grub-pc

        grub-install --no-floppy ${DI_BOOTLOADER} --target=i386-pc --force|| \
            error "grub-install failed! ${DI_BOOTLOADER}" 1
    else
        if [[ `uname -m` == "x86_64" ]]; then
            # Clover efi loader cannot use grub.efi correctly,
            # so we may patch grub or use grub.efi.signed.
            apt-get install --no-install-recommends -y --allow-unauthenticated \
                shim-signed grub-efi-amd64-signed efibootmgr

            # uefi-secure-boot options is enabled by default
            grub-install --target=x86_64-efi --uefi-secure-boot \
                --efi-directory=/boot/efi --bootloader-id="${BOOTLOADER_ID}" \
                --recheck || error "grub-install failed with efi64 ${BOOTLOADER_ID}" 1

            # Copy signed grub efi file.
            [ -d /boot/efi/EFI/ubuntu ] || mkdir -p /boot/efi/EFI/ubuntu
            cp /boot/efi/EFI/${BOOTLOADER_ID}/grub* /boot/efi/EFI/ubuntu/
            [ -d /boot/efi/EFI/boot ] || mkdir -p /boot/efi/EFI/boot
            cp -f /boot/efi/EFI/${BOOTLOADER_ID}/grub* /boot/efi/EFI/boot/

            # Backup fallback efi first.
            fallback_efi=/boot/efi/EFI/boot/bootx64.efi
            fallback_efi_bak="${fallback_efi}-$(date +%s).bak"
            [ -f "${fallback_efi}" ] && cp "${fallback_efi}" "${fallback_efi_bak}"
            # Override fallback efi with shim.
            cp /boot/efi/EFI/${BOOTLOADER_ID}/shim*.efi "${fallback_efi}"
        else
            apt-get install --no-install-recommends -y --allow-unauthenticated \
                grub-efi-ia32
            grub-install --target=i386-efi --efi-directory=/boot/efi \
                --bootloader-id="${BOOTLOADER_ID}" --recheck || \
                error "grub-install failed with efi! ${BOOTLOADER_ID}" 1

            # Copy signed grub efi file.
            [ -d /boot/efi/EFI/ubuntu ] || mkdir -p /boot/efi/EFI/ubuntu
            cp /boot/efi/EFI/${BOOTLOADER_ID}/grub* /boot/efi/EFI/ubuntu/
            [ -d /boot/efi/EFI/boot ] || mkdir -p /boot/efi/EFI/boot
            cp -f /boot/efi/EFI/${BOOTLOADER_ID}/grub* /boot/efi/EFI/boot/
            # Backup fallback efi first.
            fallback_efi=/boot/efi/EFI/boot/bootia32.efi
            fallback_efi_bak="${fallback_efi}-$(date +%s).bak"
            [ -f "${fallback_efi}" ] && cp "${fallback_efi}" "${fallback_efi_bak}"
            # Override fallback efi with shim.
            cp /boot/efi/EFI/${BOOTLOADER_ID}/shim*.efi "${fallback_efi}"
        fi
    fi

    mv -f /tmp/sources.list.bak /etc/apt/sources.list
fi

update-grub || error "Failed update-grub" 2

exit 0
