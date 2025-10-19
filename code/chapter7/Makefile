QEMU        = qemu-system-riscv32
RISCV_CC    = riscv-none-elf-gcc
OBJDUMP     = riscv-none-elf-objdump
OBJCOPY     = riscv-none-elf-objcopy
YELLOW      = \033[1;33m
END         = \033[0m

LDFLAGS     = -nostdlib
CFLAGS      = -march=rv32ima_zicsr -mabi=ilp32 -Wl,--gc-sections -ffunction-sections -fdata-sections -fdiagnostics-show-option -fno-builtin
DEBUG_FLAGS = --source --all-headers --demangle --line-numbers --wide

all: user.o
	@printf "$(YELLOW)-------- Compile Hello, World! --------$(END)\n"
	$(RISCV_CC) $(CFLAGS) start.s ctx.s trap.s hello.c frame.c stdio.c sched.c screen.c process.c interrupt.c plic.c syscall.c mtime.c mmu.c uart.c user.o -Thello.lds $(LDFLAGS) -o hello.elf
	$(OBJDUMP) $(DEBUG_FLAGS) hello.elf > hello.lst
	$(OBJCOPY) -O binary hello.elf hello.bin

qemu: all
	@printf "$(YELLOW)-------- Run Hello-World on QEMU --------$(END)\n"
	$(QEMU) -nographic -machine sifive_u -smp 2 -bios hello.bin

# --- Build user binary ---
USER_SRC    = user.c
USER_ELF    = user.elf
USER_BIN    = user.bin
USER_OBJ    = user.o
USER_LDS    = user.lds

$(USER_ELF): $(USER_SRC) $(USER_LDS)
	$(RISCV_CC) $(CFLAGS) -nostdlib -T $(USER_LDS) -o $@ crt0.s $(USER_SRC)

$(USER_BIN): $(USER_ELF)
	$(OBJCOPY) -O binary $< $@

$(USER_OBJ): $(USER_BIN)
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@

# Link user object into kernel
OBJS += $(USER_OBJ)

clean:
	rm -f hello.bin hello.lst hello.elf
