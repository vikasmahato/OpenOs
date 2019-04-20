#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/openos.kernel isodir/boot/openos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "OpenOs" {
  multiboot /boot/openos.kernel
}
EOF
grub-mkrescue -o openos.iso isodir
