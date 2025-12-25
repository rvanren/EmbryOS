VF2 = ../platforms/visionfive-2

visionfive.elf: start.S sbi.S $(VF2)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS)
	$(CC) $(CFLAGS) -Ishared -I. -I../log start.S sbi.S $(VF2)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS) -T$(VF2)/kernel.lds $(LDFLAGS) -o visionfive.elf

visionfive.bin: visionfive.elf
	$(OBJDUMP) $(DUMP_FLAGS) visionfive.elf > visionfive.lst
	$(OBJCOPY) -O binary visionfive.elf visionfive.bin

visionfive.uImage: visionfive.bin
	mkimage -A riscv -O linux -T kernel -C none -a 0x40200000 -e 0x40200000 -n "EmbryOS" -d visionfive.bin visionfive.uImage
	sum visionfive.uImage

visionfive.scr: $(VF2)/boot.cmd visionfive.uImage
	mkimage -A riscv -O linux -T script -C none -n "EmbryOS Boot Script" -d $(VF2)/boot.cmd visionfive.scr

visionfive: visionfive.uImage visionfive.scr
	@echo 'screen -L /dev/tty.usbserial-BG02SIFL 115200'
	@echo 'fatload mmc 1:1 0x40200000 visionfive.uImage; cp.b $$fdtcontroladdr $$fdt_addr_r 0x10000; fdt addr $$fdt_addr_r; bootm 0x40200000 - $$fdt_addr_r'
	cp visionfive.uImage $(VF2)
	cp visionfive.scr $(VF2)/boot.scr
	cp visionfive.uImage /Volumes/P3
	cp visionfive.scr /Volumes/P3
	sum /Volumes/P3/visionfive.uImage
	sync
	sleep 1
	diskutil eject /dev/disk4
