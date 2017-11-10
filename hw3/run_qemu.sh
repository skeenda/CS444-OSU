#!/bin/bash

qemu-system-i386 -nographic -kernel arch/x86/boot/bzImage -drive file=core-image-lsb-sdk-qemux86.ext4 -enable-kvm -usb -localtime --no-reboot --append "root=/dev/hda rw console=ttyS0 debug" 
