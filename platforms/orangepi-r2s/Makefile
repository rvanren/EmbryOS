OP2 = ../platforms/orangepi-r2s

orangepi.elf: start.S sbi.S $(OP2)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS)
	$(CC) $(CFLAGS) -Ishared -I. -I../log start.S sbi.S $(QV)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS) -T$(QV)/kernel.lds $(LDFLAGS) -o orangepi.elf

orangepi.bin: orangepi.elf
	$(OBJDUMP) $(DUMP_FLAGS) orangepi.elf > orangepi.lst
	$(OBJCOPY) -O binary orangepi.elf orangepi.bin

orangepi.uImage: orangepi.bin
	mkimage -A riscv -O linux -T kernel -C none -a 0x02000000 -e 0x02000000 -n "EmbryOS" -d orangepi.bin orangepi.uImage

orangepi.scr: $(OP2)/boot.cmd orangepi.uImage
	mkimage -A riscv -O linux -T script -C none -n "EmbryOS Boot Script" -d $(OP2)/boot.cmd orangepi.scr

orangepi: orangepi.uImage orangepi.scr
	@echo 'screen -L /dev/tty.usbserial-BG02SIFL 115200'
	cp orangepi.scr $(OP2)/boot.scr
	cp orangepi.uImage $(OP2)
