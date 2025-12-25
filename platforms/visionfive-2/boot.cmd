echo ===== TRYING TO LOAD KERNEL =====

fatls mmc 1:1

fatload mmc 1:1 0x40200000 visionfive.uImage
fatload mmc 1:1 ${fdt_addr_r} jh7110-starfive-visionfive-2-v1.3b.dtb
bootm 0x40200000 - ${fdt_addr_r}
