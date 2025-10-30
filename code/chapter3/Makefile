.SUFFIXES:

QEMU        = qemu-system-riscv32
CC          = riscv-none-elf-gcc
OBJDUMP     = riscv-none-elf-objdump
OBJCOPY     = riscv-none-elf-objcopy
NM          = riscv-none-elf-nm
YELLOW      = \033[1;33m
END         = \033[0m

LDFLAGS     = -nostdlib
CFLAGS      = -march=rv32ima_zicsr -mabi=ilp32 -Wl,--gc-sections -ffunction-sections -fdata-sections -fdiagnostics-show-option -fno-builtin
DEBUG_FLAGS = --source --all-headers --demangle --line-numbers --wide

all:
	@printf "$(YELLOW)-------- Compile Hello, World! --------$(END)\n"
	$(CC) $(CFLAGS) -Ishared -I. start.s trap.s hello.c frame.c kprintf.c io.c screen.c interrupt.c apps/splash.c -Thello.lds $(LDFLAGS) -o hello.elf
	$(OBJDUMP) $(DEBUG_FLAGS) hello.elf > hello.lst
	$(OBJCOPY) -O binary hello.elf hello.bin

qemu: all
	@printf "$(YELLOW)-------- Run Hello-World on QEMU --------$(END)\n"
	$(QEMU) -nographic -machine virt -smp 2 -bios default -kernel hello.bin
	@printf "$(END)\033[?25h\n"

clean:
	@printf "$(YELLOW)-------- clean --------$(END)\n"
	rm -f hello.bin hello.lst hello.elf
