QVB = ../platforms/qemu-bare
 
qemu-bare.elf: start.S sbi.S $(QVB)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS)
	$(CC) $(CFLAGS) -DNO_SBI -Ishared -I. -I../log start.S sbi.S $(QVB)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS) -T$(QVB)/kernel.lds $(LDFLAGS) -o qemu-bare.elf
	$(OBJDUMP) $(DUMP_FLAGS) qemu-bare.elf > qemu-bare.lst
	$(OBJCOPY) -O binary qemu-bare.elf qemu-bare.bin
