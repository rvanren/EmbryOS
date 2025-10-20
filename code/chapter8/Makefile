QEMU        = qemu-system-riscv32
RISCV_CC    = riscv-none-elf-gcc
OBJDUMP     = riscv-none-elf-objdump
OBJCOPY     = riscv-none-elf-objcopy
NM          = riscv-none-elf-nm
YELLOW      = \033[1;33m
END         = \033[0m

LDFLAGS     = -nostdlib
CFLAGS      = -march=rv32ima_zicsr -mabi=ilp32 -Wl,--gc-sections -ffunction-sections -fdata-sections -fdiagnostics-show-option -fno-builtin
DEBUG_FLAGS = --source --all-headers --demangle --line-numbers --wide

all: user.o
	@printf "$(YELLOW)-------- Compile Hello, World! --------$(END)\n"
	$(RISCV_CC) $(CFLAGS) start.s ctx.s trap.s hello.c frame.c stdio.c sched.c screen.c process.c proc_clean.c interrupt.c plic.c syscall.c mtime.c uart.c user.o -Thello.lds $(LDFLAGS) -o hello.elf
	$(OBJDUMP) $(DEBUG_FLAGS) hello.elf > hello.lst
	$(OBJCOPY) -O binary hello.elf hello.bin

qemu: all
	@printf "$(YELLOW)-------- Run Hello-World on QEMU --------$(END)\n"
	$(QEMU) -nographic -machine sifive_u -smp 2 -bios hello.bin

# --- Build user binary ---

user.elf: user.c user.lds
	$(RISCV_CC) $(CFLAGS) -fpie -ffreestanding -nostdlib -nostartfiles -march=rv32i -nostdlib -T user.lds -o $@ crt0.s user.c

user.bin: user.elf
	$(OBJCOPY) -O binary $< $@

user.o: user.bin
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@

# user_gp.h contains:  #define USER_GP_OFFSET 0x<hex>
user_gp.h: user.elf
	@gp_off=$$($(NM) user.elf | awk '/ __global_pointer\$$/ {print $$1}'); \
	echo "#pragma once"              >  $@; \
	echo "#define USER_GP_OFFSET 0x$${gp_off}" >> $@

# make kernel objects recompile if gp offset changes
$(KERNEL_COBJS): user_gp.h

clean:
	rm -f hello.bin hello.lst hello.elf
