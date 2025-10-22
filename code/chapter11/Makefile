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

APP_OBJS = $(addprefix $(APPS_DIR)/,$(addsuffix .bin.o,$(APPS)))
APP_ELFS = $(addprefix $(APPS_DIR)/,$(addsuffix .elf,$(APPS)))

all: apps_gen.c $(APP_OBJS)
	@printf "$(YELLOW)-------- Compile Hello, World! --------$(END)\n"
	$(CC) $(CFLAGS) start.s ctx.s trap.s enter_user.s hello.c apps.c frame.c stdio.c string.c sched.c screen.c process.c proc_clean.c interrupt.c flat.c bd_ramdisk.c bd_simple.c bd_simple_ops.c plic.c syscall.c mtime.c pmp.c uart.c apps_gen.c $(APP_OBJS) -Thello.lds $(LDFLAGS) -o hello.elf
	$(OBJDUMP) $(DEBUG_FLAGS) hello.elf > hello.lst
	$(OBJCOPY) -O binary hello.elf hello.bin

qemu: all
	@printf "$(YELLOW)-------- Run Hello-World on QEMU --------$(END)\n"
	$(QEMU) -nographic -machine sifive_u -smp 2 -bios hello.bin

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
	@echo "#include <stdint.h>" >> $@
	@echo "extern void run_user(const char *start, const char *end, uintptr_t gp_offset);" >> $@
	@echo "" >> $@
	@for f in $^; do \
	    name=$$(basename $$f .elf); \
	    func=$$(echo $$name | sed 's/^/task_/'); \
	    off=$$($(NM) $$f | awk '/ __global_pointer\$$/ {print $$1}'); \
	    echo "void $${func}(void) {" >> $@; \
	    echo "    extern char _binary_$${name}_bin_start[], _binary_$${name}_bin_end[];" >> $@; \
	    echo "    run_user(_binary_$${name}_bin_start, _binary_$${name}_bin_end, 0x$${off});" >> $@; \
	    echo "}" >> $@; \
	    echo "" >> $@; \
	done; \
	echo "void (*applications[])() = {" >> $@; \
	for f in $^; do \
	    name=$$(basename $$f .elf); \
	    func=$$(echo $$name | sed 's/^/task_/'); \
	    echo "    $${func}," >> $@; \
	done; \
	echo "};" >> $@; \
	echo "int n_applications = sizeof(applications)/sizeof(applications[0]);" >> $@; \
	echo "Generated $@"

clean:
	@printf "$(YELLOW)-------- clean --------$(END)\n"
	rm -f hello.bin hello.lst hello.elf
