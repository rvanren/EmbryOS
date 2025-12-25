echo ===== TRYING TO LOAD KERNEL =====

setenv kernel_addr_r 0x02000000
setenv fdt_addr_r    0x04000000

usb start

fatls usb 0:1

echo Loading kernel...
fatload usb 0:1 ${kernel_addr_r} orangepi.uImage

echo Loading DTB...
fatload usb 0:1 ${fdt_addr_r} kyx1-orangepi-r2s.dtb

echo ===== JUMPING TO KERNEL =====
bootm ${kernel_addr_r} - ${fdt_addr_r}
