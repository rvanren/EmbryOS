# ============================================================
# RISC-V Toolchain
# ============================================================
CC      = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
OBJDUMP = riscv64-unknown-elf-objdump

# ============================================================
# Kernel build configuration
# ============================================================
KERNEL_CSRC   = hello.c uart.c trap.c sched.c process.c frame.c interrupt.c \
                plic.c mtime.c proc_clean.c
KERNEL_ASMSRC = start.s ctx.s                    # kernel assembly sources

KERNEL_CFLAGS = -Wall -O2 -g \
  -march=rv32i -mabi=ilp32 \
  -ffreestanding -nostdlib -nostartfiles

KERNEL_ASFLAGS = -march=rv32i -mabi=ilp32

KERNEL_LDFLAGS = -T kernel.lds -nostdlib -nostartfiles

KERNEL_COBJS   = $(KERNEL_CSRC:.c=.o)
KERNEL_AOBJS   = $(KERNEL_ASMSRC:.s=.o)
KERNEL_OBJS    = $(KERNEL_COBJS) $(KERNEL_AOBJS)

# ============================================================
# User build configuration (position-independent binary)
# ============================================================
USER_CSRC   = user.c
USER_ASMSRC = crt0.s

USER_CFLAGS = -Wall -O2 -g \
  -march=rv32i -mabi=ilp32 \
  -fpie -ffreestanding -nostdlib -nostartfiles

USER_ASFLAGS = -march=rv32i -mabi=ilp32

USER_LDFLAGS = -pie -nostdlib -nostartfiles -T user.lds

USER_COBJS   = $(USER_CSRC:.c=.o)
USER_AOBJS   = $(USER_ASMSRC:.s=.o)
USER_OBJS    = $(USER_COBJS) $(USER_AOBJS)

# ============================================================
# Targets
# ============================================================
.PHONY: all clean dump

all: kernel.elf user.bin

# ------------------------------------------------------------
# Kernel build
# ------------------------------------------------------------
kernel.elf: $(KERNEL_OBJS)
	$(CC) $(KERNEL_CFLAGS) $(KERNEL_LDFLAGS) -o $@ $^

# ------------------------------------------------------------
# User build (PIC)
# ------------------------------------------------------------
user.elf: $(USER_OBJS)
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -o $@ $^

user.bin: user.elf
	$(OBJCOPY) -O binary $< $@

# ------------------------------------------------------------
# Utility targets
# ------------------------------------------------------------
dump: kernel.elf user.elf
	$(OBJDUMP) -d kernel.elf > kernel.dump
	$(OBJDUMP) -d user.elf   > user.dump

clean:
	rm -f $(KERNEL_OBJS) $(USER_OBJS) kernel.elf user.elf user.bin *.dump

# ============================================================
# Compile rules
# ============================================================
# Kernel C
$(KERNEL_COBJS): %.o: %.c
	$(CC) -c $(KERNEL_CFLAGS) $< -o $@

# Kernel assembly
$(KERNEL_AOBJS): %.o: %.s
	$(CC) -c $(KERNEL_ASFLAGS) $< -o $@

# User C
$(USER_COBJS): %.o: %.c
	$(CC) -c $(USER_CFLAGS) $< -o $@

# User assembly
$(USER_AOBJS): %.o: %.s
	$(CC) -c $(USER_ASFLAGS) $< -o $@
