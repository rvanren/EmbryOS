LITEX = ../platforms/litex

litex.elf: start.S sbi.S $(LITEX)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS)
	$(CC) $(CFLAGS) -DNO_SBI -Ishared -I. -I../log start.S sbi.S $(LITEX)/config.c log.c shared/string.c hello.c kprintf.c io.c uart.c uart_16550.c uart_sifive.c uart_pxa.c uart_litex.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS) -T$(LITEX)/kernel.lds $(LDFLAGS) -o litex.elf
	$(OBJDUMP) $(DUMP_FLAGS) litex.elf > litex.lst
	$(OBJCOPY) -O binary litex.elf litex.bin
