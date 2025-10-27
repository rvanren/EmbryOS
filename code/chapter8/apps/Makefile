include apps.mk

CC        = riscv-none-elf-gcc
OBJCOPY   = riscv-none-elf-objcopy
OBJDUMP   = riscv-none-elf-objdump
NM        = riscv-none-elf-nm

DEBUG_FLAGS = --source --all-headers --demangle --line-numbers --wide

CFLAGS = -g -I../shared -I.. -march=rv32im -mabi=ilp32 -ffreestanding -nostdlib -nostartfiles -fpie -mno-relax -msmall-data-limit=0
LDFLAGS = -Wl,--no-relax -T user.lds

APP_ELFS  = $(addsuffix .elf,$(APPS))
APP_BINS  = $(addsuffix .bin,$(APPS))
APP_OBJS  = $(addsuffix .bin.o,$(APPS))   # <-- note .bin.o suffix

# build both .elf and .o so that top-level can nm the .elf
all: $(APP_ELFS) $(APP_OBJS)

init.elf: init.c ../shared/string.c ../shared/string.h dir.c dir.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ crt0.s crt1.c init.c ../shared/string.c dir.c
	$(OBJDUMP) $(DEBUG_FLAGS) $@ > $@.lst

%.elf: %.c crt0.s crt1.c user.lds ../shared/string.h ../shared/string.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ crt0.s crt1.c ../shared/string.c $<
	$(OBJDUMP) $(DEBUG_FLAGS) $@ > $@.lst

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.bin.o: %.bin
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@

clean:
	rm -f $(APP_ELFS) $(APP_BINS) $(APP_OBJS)
