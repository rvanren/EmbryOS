MVD = ../platforms/milkv-duo-256m

milkv-duo.elf: start.S sbi.S hello.c kprintf.c io.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS)
	$(CC) $(CFLAGS) -Ishared -I. start.S sbi.S hello.c kprintf.c io.c $(KERNEL_SRCS) $(APP_SRCS) $(FDT_SRCS) -T$(MVD)/kernel.lds $(LDFLAGS) -o milkv-duo.elf
	$(OBJDUMP) $(DUMP_FLAGS) milkv-duo.elf > milkv-duo.lst

milkv-duo.bin: milkv-duo.elf
	$(OBJCOPY) -O binary milkv-duo.elf milkv-duo.bin
