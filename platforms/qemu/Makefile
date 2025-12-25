QV = ../platforms/qemu

qemu.elf: start.S sbi.S $(QV)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS)
	$(CC) $(CFLAGS) -Ishared -I. -I../log start.S sbi.S $(QV)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS) -T$(QV)/kernel.lds $(LDFLAGS) -o qemu.elf
	$(OBJDUMP) $(DUMP_FLAGS) qemu.elf > qemu.lst
	$(OBJCOPY) -O binary qemu.elf qemu.bin
