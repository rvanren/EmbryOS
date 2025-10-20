# --------------------------------------------------------------------
#  Toolchain
# --------------------------------------------------------------------
CC      = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
OBJDUMP = riscv64-unknown-elf-objdump

# --------------------------------------------------------------------
#  Kernel build configuration
# --------------------------------------------------------------------
KERNEL_CFLAGS = -Wall -O2 -g \
	-march=rv32i -mabi=ilp32 \
	-ffreestanding -nostdlib -nostartfiles

KERNEL_LDFLAGS = -T kernel.lds -nostdlib -nostartfiles

KERNEL_SRCS = main.c uart.c trap.c sched.c process.c frame.c \
	       interrupt.c plic.c mtime.c ctx.s
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o)

# --------------------------------------------------------------------
#  User program build configuration (position-independent)
# --------------------------------------------------------------------
USER_CFLAGS = -Wall -O2 -g \
	-march=rv32i -mabi=ilp32 \
	-fpie -ffreestanding -nostdlib -nostartfiles

USER_LDFLAGS = -pie -nostdlib -nostartfiles -T user.lds

USER_SRCS = user.c
USER_OBJS = $(USER_SRCS:.c=.o)

# --------------------------------------------------------------------
#  Targets
# --------------------------------------------------------------------
.PHONY: all clean dump

all: kernel.elf user.bin

# ------------------ Kernel build ------------------------------------
kernel.elf: $(KERNEL_OBJS)
	$(CC) $(KERNEL_CFLAGS) $(KERNEL_LDFLAGS) -o $@ $^

# ------------------ User program build -------------------------------
user.elf: $(USER_OBJS)
	$(CC) $(USER_CFLAGS) $(USER_LDFLAGS) -o $@ $^

user.bin: user.elf
	$(OBJCOPY) -O binary $< $@

# ------------------ Helpers -----------------------------------------
dump: kernel.elf
	$(OBJDUMP) -d $< > kernel.dump
	$(OBJDUMP) -d user.elf > user.dump

clean:
	rm -f $(KERNEL_OBJS) $(USER_OBJS) kernel.elf user.elf user.bin *.dump

# --------------------------------------------------------------------
#  Per-source compile rules
# --------------------------------------------------------------------
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Default rule uses whichever flags are appropriate
# Override per group:
$(KERNEL_OBJS): %.o: %.c
	$(CC) -c $(KERNEL_CFLAGS) $< -o $@

$(USER_OBJS): %.o: %.c
	$(CC) -c $(USER_CFLAGS) $< -o $@
