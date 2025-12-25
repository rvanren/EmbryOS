include apps.mk

CC        = $(CPREFIX)gcc
OBJDUMP   = $(CPREFIX)objdump
STRIP     = $(CPREFIX)strip

CFLAGS = -g -I../shared -I.. $(CFLAGS_ARCH) \
         -ffreestanding -nostdlib -nostartfiles -mno-relax -msmall-data-limit=0
LDFLAGS = -Wl,--no-relax -T user.lds
DUMP_FLAGS = --source --all-headers --demangle --line-numbers --wide

all: $(APPS)

%: %.c crt0.s syslib.S crt1.c user.lds ../shared/string.c ../shared/string2.c stdio.c dir.c blockpixel.c window1.c window2.c putchar.c malloc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ crt0.s syslib.S crt1.c ../shared/string.c ../shared/string2.c stdio.c dir.c blockpixel.c window1.c window2.c putchar.c malloc.c $<
	$(OBJDUMP) $(DUMP_FLAGS) $@ > $@.lst
	$(STRIP) $@

clean:
	rm -f *.elf *.o *.lst *.bin $(APPS)
