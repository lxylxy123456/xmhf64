#!/bin/bash

# Run QEMU (download debian11efi.qcow2 from Google Drive, put it to /PATH/TO/)
# /usr/share/OVMF/OVMF_CODE.fd is UEFI firmware image
# /usr/share/OVMF/UefiShell.iso contains UEFI shell
# You will see a line like "Image base: 0x7F9FE000" in UEFI console. This line
#  will be used for GDB
# [NOTE] Qemu does not need to load UefiShell.iso in Ubuntu, so we load fat.img in two disks to keep the FS* sequence.
qemu-system-x86_64 \
	-m 2G \
	-gdb tcp::1234 \
	-smp 4 \
	-cpu Haswell,vmx=yes \
	-enable-kvm \
	-bios /usr/share/ovmf/OVMF.fd \
	-net none \
	-serial stdio \
	-drive media=cdrom,file=../uefi_flushdrive_img/grub/fat.img,index=0 \
	-drive media=cdrom,file=../uefi_flushdrive_img/grub/fat.img,index=1 \
	-drive media=disk,file=debian11efi.qcow2,index=2