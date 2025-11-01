include apps.mk

CC        = riscv-none-elf-gcc
OBJCOPY   = riscv-none-elf-objcopy
OBJDUMP   = riscv-none-elf-objdump
NM        = riscv-none-elf-nm

DEBUG_FLAGS = --source --all-headers --demangle --line-numbers --wide

ARCH ?= rv32   # or rv64

# ----------------------------------------------------------------------
# Architecture-dependent flags
# ----------------------------------------------------------------------
ifeq ($(ARCH),rv64)
    MARCH     = rv64im
    MABI      = lp64
    ELFCLASS  = elf64-littleriscv
else
    MARCH     = rv32im
    MABI      = ilp32
    ELFCLASS  = elf32-littleriscv
endif

# ----------------------------------------------------------------------
# Compiler / Linker Flags
# ----------------------------------------------------------------------
CFLAGS  = -g -I../shared -march=$(MARCH) -mabi=$(MABI) \
           -ffreestanding -nostdlib -nostartfiles -fpie -mno-relax -msmall-data-limit=0
LDFLAGS = -Wl,--no-relax -T user.lds

APP_ELFS = $(addsuffix .elf,$(APPS))
APP_BINS = $(addsuffix .bin,$(APPS))
APP_OBJS = $(addsuffix .bin.o,$(APPS))

# ----------------------------------------------------------------------
# Rules
# ----------------------------------------------------------------------
all: $(APP_ELFS) $(APP_OBJS)

%.elf: %.c crt0.s crt1.c user.lds ../shared/string.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ crt0.s crt1.c ../shared/string.c $<
	$(OBJDUMP) $(DEBUG_FLAGS) $@ > $@.lst

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.bin.o: %.bin
	$(OBJCOPY) -I binary -O $(ELFCLASS) -B riscv $< $@

clean:
	rm -f $(APP_ELFS) $(APP_BINS) $(APP_OBJS) *.lst
