BPIF3 = ../platforms/bananapi-f3

bananapi.elf: start.S sbi.S $(BPIF3)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS)
	$(CC) $(CFLAGS) -Ishared -I. -I../log start.S sbi.S $(QV)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS) -T$(QV)/kernel.lds $(LDFLAGS) -o bananapi.elf

bananapi.bin: bananapi.elf
	$(OBJDUMP) $(DUMP_FLAGS) bananapi.elf > bananapi.lst
	$(OBJCOPY) -O binary bananapi.elf bananapi.bin

bananapi.uImage: bananapi.bin
	mkimage -A riscv -O linux -T kernel -C none -a 0x600000 -e 0x600000 -n "EmbryOS" -d bananapi.bin bananapi.uImage

bananapi.scr: $(BPIF3)/boot.cmd bananapi.uImage
	mkimage -A riscv -O linux -T script -C none -n "EmbryOS Boot Script" -d $(BPIF3)/boot.cmd bananapi.scr

bananapi: bananapi.uImage bananapi.scr
	@echo 'screen -L /dev/tty.usbserial-BG02SIFL 115200'
	@echo 'fatload mmc 1:1 0x40200000 bananapi.uImage; cp.b $$fdtcontroladdr $$fdt_addr_r 0x10000; fdt addr $$fdt_addr_r; bootm 0x40200000 - $$fdt_addr_r'
	cp bananapi.uImage $(BPIF3)
	cp bananapi.scr $(BPIF3)/boot.scr
	cp bananapi.uImage /Volumes/P3
	cp bananapi.scr /Volumes/P3
	sync
	sleep 1
	diskutil eject /dev/disk4
