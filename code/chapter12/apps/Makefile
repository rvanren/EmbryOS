.SUFFIXES:

QEMU        = qemu-system-riscv32
CC          = riscv-none-elf-gcc
OBJDUMP     = riscv-none-elf-objdump
OBJCOPY     = riscv-none-elf-objcopy
NM          = riscv-none-elf-nm
YELLOW      = \033[1;33m
END         = \033[0m

LDFLAGS     = -nostdlib
CFLAGS      = -march=rv32ima_zicsr -mabi=ilp32 -Wl,--gc-sections -ffunction-sections -fdata-sections -fdiagnostics-show-option -fno-builtin -DVIRT
DEBUG_FLAGS = --source --all-headers --demangle --line-numbers --wide

APPS_DIR   = apps
include $(APPS_DIR)/apps.mk          # import APPS from one place

APP_OBJS = $(addprefix $(APPS_DIR)/,$(addsuffix .bin.o,$(APPS)))
APP_ELFS = $(addprefix $(APPS_DIR)/,$(addsuffix .elf,$(APPS)))

all: apps_gen.c $(APP_OBJS)
	@printf "$(YELLOW)-------- Compile Hello, World! --------$(END)\n"
	$(CC) $(CFLAGS) -Ishared -I. start.s ctx.s trap.s enter_user.s hello.c die.c apps.c frame.c kprintf.c shared/string.c sched.c sched2.c screen.c process.c procsys.c interrupt.c plic.c syscall.c vm.c io.c io2.c uart_sifive.c uart_16550.c files.c bd_ramdisk.c bd_simple.c bd_simple_ops.c flat1.c flat2.c apps_gen.c $(APP_OBJS) -Thello.lds $(LDFLAGS) -o hello.elf
	$(OBJDUMP) $(DEBUG_FLAGS) hello.elf > hello.lst
	$(OBJCOPY) -O binary hello.elf hello.bin

qemu: all
	@printf "$(YELLOW)-------- Run Hello-World on QEMU --------$(END)\n"
	$(QEMU) -nographic -machine virt -smp 2 -bios default -kernel hello.bin
	@printf "$(END)\033[?25h\n"

# --- Build user binary ---

###############################################################################
# Delegate user apps build to apps/Makefile
###############################################################################

RISCV_CC   = riscv-none-elf-gcc
OBJCOPY    = riscv-none-elf-objcopy
NM         = riscv-none-elf-nm

APP_OBJS = $(addprefix $(APPS_DIR)/,$(addsuffix .bin.o,$(APPS)))
APP_ELFS = $(addprefix $(APPS_DIR)/,$(addsuffix .elf,$(APPS)))

# Build user apps first
$(APP_OBJS) $(APP_ELFS):
	$(MAKE) -C $(APPS_DIR)

# --- auto-generate apps_gen.c ---
apps_gen.c: $(APP_ELFS)
	@echo "// auto-generated" > $@
	@echo '#include "app_info.h"' >> $@
	@echo "" >> $@
	@for f in $^; do \
	    name=$$(basename $$f .elf); \
	    off=$$($(NM) $$f | awk '/ __global_pointer\$$/ {print $$1}'); \
	    echo "extern char _binary_$${name}_bin_start[], _binary_$${name}_bin_end[];" >> $@; \
	done; \
	echo "" >> $@; \
	echo "struct app_info app_table[] = {" >> $@; \
	for f in $^; do \
	    name=$$(basename $$f .elf); \
	    off=$$($(NM) $$f | awk '/ __global_pointer\$$/ {print $$1}'); \
	    echo "    { _binary_$${name}_bin_start, _binary_$${name}_bin_end, 0x$${off} }," >> $@; \
	done; \
	echo "};" >> $@; \
	echo "int n_applications = sizeof(app_table)/sizeof(app_table[0]);" >> $@; \
	echo "Generated $@"

clean:
	@printf "$(YELLOW)-------- clean --------$(END)\n"
	rm -f hello.bin hello.lst hello.elf
