include apps.mk

CC        = riscv-none-elf-gcc
OBJCOPY   = riscv-none-elf-objcopy
NM        = riscv-none-elf-nm

CFLAGS = -I.. -march=rv32im -mabi=ilp32 -ffreestanding -nostdlib -nostartfiles -fpie -T user.lds

APP_ELFS  = $(addsuffix .elf,$(APPS))
APP_BINS  = $(addsuffix .bin,$(APPS))
APP_OBJS  = $(addsuffix .bin.o,$(APPS))   # <-- note .bin.o suffix

# build both .elf and .o so that top-level can nm the .elf
all: $(APP_ELFS) $(APP_OBJS)

%.elf: %.c crt0.s user.lds
	$(CC) $(CFLAGS) -o $@ crt0.s $<

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@

%.bin.o: %.bin
	$(OBJCOPY) -I binary -O elf32-littleriscv -B riscv $< $@


clean:
	rm -f $(APP_ELFS) $(APP_BINS) $(APP_OBJS)
