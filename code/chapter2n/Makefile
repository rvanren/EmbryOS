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

APPS_DIR   = apps
include $(APPS_DIR)/apps.mk          # import APPS from one place

APP_SRCS = $(addprefix $(APPS_DIR)/,$(addsuffix .c,$(APPS)))

all: $(APP_OBJS)
	@printf "$(YELLOW)-------- Compile Hello, World! --------$(END)\n"
	$(CC) $(CFLAGS) -Ishared -I. start.s ctx.s hello.c kprintf.c shared/string.c screen.c uart2.c $(APP_SRCS) -Thello.lds $(LDFLAGS) -o hello.elf
	$(OBJDUMP) $(DEBUG_FLAGS) hello.elf > hello.lst
	$(OBJCOPY) -O binary hello.elf hello.bin

qemu: all
	@printf "$(YELLOW)-------- Run Hello-World on QEMU --------$(END)\n"
	$(QEMU) -nographic -machine sifive_u -smp 2 -bios hello.bin

clean:
	@printf "$(YELLOW)-------- clean --------$(END)\n"
	rm -f hello.bin hello.lst hello.elf
