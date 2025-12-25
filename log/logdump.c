/*
 * logdump.c
 *
 * Usage:
 *   logdump kernel.elf mem.dump
 *
 * Assumes mem.dump is a pmemsave-style dump whose offset 0 corresponds
 * to the lowest PT_LOAD physical address (kernel base).
 *
 * Prints one integrated log sorted by vector timestamp.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <inttypes.h>
#include "log_defs.h"
#include "../chapter12/shared/syscall.h"

struct log_event_desc {
    const char *name;
    const char *arg_names;
    enum log_arg_kind args[LOG_MAX_PAYLOAD];
};

static const struct log_event_desc log_event_desc[] = {
#define LOG_EVENT(name, arg_names, ...) \
    [L_##name] = { #name, arg_names, { __VA_ARGS__ } },
#include "log_events.def"
#undef LOG_EVENT
};

_Static_assert(sizeof(struct log_entry) == 64, "log_entry size mismatch");

/* ============================================================
 * Minimal ELF parsing (no <elf.h>)
 * ============================================================ */

#define EI_NIDENT   16
#define EI_CLASS    4
#define ELFCLASS32  1
#define ELFCLASS64  2

#define ELFMAG0     0x7f
#define ELFMAG1     'E'
#define ELFMAG2     'L'
#define ELFMAG3     'F'

#define PT_LOAD     1
#define SHT_SYMTAB  2

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;

typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Xword;

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off  e_phoff;
    Elf32_Off  e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off  e_phoff;
    Elf64_Off  e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

typedef struct {
    Elf64_Word  p_type;
    Elf64_Word  p_flags;
    Elf64_Off   p_offset;
    Elf64_Addr  p_vaddr;
    Elf64_Addr  p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;

/* === Missing before: section headers === */
typedef struct {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off  sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {
    Elf64_Word  sh_name;
    Elf64_Word  sh_type;
    Elf64_Xword sh_flags;
    Elf64_Addr  sh_addr;
    Elf64_Off   sh_offset;
    Elf64_Xword sh_size;
    Elf64_Word  sh_link;
    Elf64_Word  sh_info;
    Elf64_Xword sh_addralign;
    Elf64_Xword sh_entsize;
} Elf64_Shdr;

typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

typedef struct {
    Elf64_Word st_name;
    unsigned char st_info;
    unsigned char st_other;
    Elf64_Half st_shndx;
    Elf64_Addr st_value;
    Elf64_Xword st_size;
} Elf64_Sym;

struct elf_info {
    uint64_t kernel_base;
    uint64_t log_header_addr;
    uint64_t log_addr;
};

/* ============================================================
 * Helpers
 * ============================================================ */

static void die(const char *msg)
{
    perror(msg);
    exit(1);
}

static void die_msg(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

/* ============================================================
 * ELF parsing: find kernel base, log_header, log[]
 * ============================================================ */

int is64;

static void parse_elf(const char *path, struct elf_info *info) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) die(path);

    unsigned char ident[EI_NIDENT];
    if (read(fd, ident, EI_NIDENT) != EI_NIDENT)
        die(path);

    if (ident[0] != ELFMAG0 || ident[1] != ELFMAG1 ||
        ident[2] != ELFMAG2 || ident[3] != ELFMAG3)
        die_msg("not an ELF file");

    is64 = (ident[EI_CLASS] == ELFCLASS64);

    uint64_t kernel_base = UINT64_MAX;
    uint64_t log_header_addr = 0;
    uint64_t log_addr = 0;

    if (is64) {
        Elf64_Ehdr eh;
        pread(fd, &eh, sizeof(eh), 0);

        for (int i = 0; i < eh.e_phnum; i++) {
            Elf64_Phdr ph;
            pread(fd, &ph, sizeof(ph), eh.e_phoff + (Elf64_Off)i * sizeof(ph));
            if (ph.p_type == PT_LOAD && ph.p_paddr < kernel_base)
                kernel_base = ph.p_paddr;
        }

        for (int i = 0; i < eh.e_shnum; i++) {
            Elf64_Shdr sh;
            pread(fd, &sh, sizeof(sh), eh.e_shoff + (Elf64_Off)i * sizeof(sh));
            if (sh.sh_type != SHT_SYMTAB) continue;

            Elf64_Shdr str;
            pread(fd, &str, sizeof(str),
                  eh.e_shoff + (Elf64_Off)sh.sh_link * sizeof(str));

            char *strtab = malloc((size_t)str.sh_size);
            if (!strtab) die("malloc strtab");
            pread(fd, strtab, (size_t)str.sh_size, str.sh_offset);

            for (Elf64_Off off = 0; off < sh.sh_size; off += sizeof(Elf64_Sym)) {
                Elf64_Sym sym;
                pread(fd, &sym, sizeof(sym), sh.sh_offset + off);
                const char *name = strtab + sym.st_name;
                if (!strcmp(name, "log_header"))
                    log_header_addr = sym.st_value;
                else if (!strcmp(name, "log"))
                    log_addr = sym.st_value;
            }

            free(strtab);
            break;
        }
    } else {
        Elf32_Ehdr eh;
        pread(fd, &eh, sizeof(eh), 0);

        for (int i = 0; i < eh.e_phnum; i++) {
            Elf32_Phdr ph;
            pread(fd, &ph, sizeof(ph), eh.e_phoff + (Elf32_Off)i * sizeof(ph));
            if (ph.p_type == PT_LOAD && ph.p_paddr < kernel_base)
                kernel_base = ph.p_paddr;
        }

        for (int i = 0; i < eh.e_shnum; i++) {
            Elf32_Shdr sh;
            pread(fd, &sh, sizeof(sh), eh.e_shoff + (Elf32_Off)i * sizeof(sh));
            if (sh.sh_type != SHT_SYMTAB) continue;

            Elf32_Shdr str;
            pread(fd, &str, sizeof(str),
                  eh.e_shoff + (Elf32_Off)sh.sh_link * sizeof(str));

            char *strtab = malloc((size_t)str.sh_size);
            if (!strtab) die("malloc strtab");
            pread(fd, strtab, (size_t)str.sh_size, str.sh_offset);

            for (Elf32_Off off = 0; off < sh.sh_size; off += sizeof(Elf32_Sym)) {
                Elf32_Sym sym;
                pread(fd, &sym, sizeof(sym), sh.sh_offset + off);
                const char *name = strtab + sym.st_name;
                if (!strcmp(name, "log_header"))
                    log_header_addr = sym.st_value;
                else if (!strcmp(name, "log"))
                    log_addr = sym.st_value;
            }

            free(strtab);
            break;
        }
    }

    close(fd);

    if (kernel_base == UINT64_MAX)
        die_msg("could not determine kernel_base from PT_LOAD segments");
    if (!log_header_addr)
        die_msg("symbol log_header not found (need .symtab, not stripped)");
    if (!log_addr)
        die_msg("symbol log not found (need .symtab, not stripped)");

    info->kernel_base = kernel_base;
    info->log_header_addr = log_header_addr;
    info->log_addr = log_addr;
}

/* ============================================================
 * Event collection + sorting
 * ============================================================ */

struct event {
    struct log_entry le;
};

static int cmp_event(const void *a, const void *b)
{
    const struct event *x = a;
    const struct event *y = b;

    for (int i = 0; i < LOG_N_LEVELS; i++) {
        if (x->le.ts[i] < y->le.ts[i]) return -1;
        if (x->le.ts[i] > y->le.ts[i]) return  1;
    }
    return 0;
}

/* ============================================================
 * Main
 * ============================================================ */

static void print_cell(uint64_t x) {
    static char *types[] = { "ASCII", "BLOCK", "BRAILLE" };
    static char *colors[] = { "black", "red", "green", "yellow",
                              "blue", "magenta", "cyan", "white" };
    cell_t c = x;
    uint8_t t  = CELL_T(c);
    uint8_t ch = CELL_CH(c);
    uint8_t fg = CELL_FG(c);
    uint8_t bg = CELL_BG(c);
    printf("CELL(%s,%s,%s,", types[t], colors[fg], colors[bg]);
    if (t == CELL_ASCII && ch < 128 && ch >= 32)
        printf("'%c')", (char) ch);
    else printf("0x%02x)", ch);
}

static char *scause_str(uint64_t scause) {
    static char *unknown_int  = "Unknown interrupt";

    uint64_t mask = is64 ? 63 : 31;
    int is_interrupt = (scause >> mask) & 1;
    uint64_t code = scause & ((1ULL << mask) - 1);

    if (!is_interrupt) {
        switch (code) {
        case 0:  return "Instruction address misaligned";
        case 1:  return "Instruction access fault";
        case 2:  return "Illegal instruction";
        case 3:  return "Breakpoint";
        case 4:  return "Load address misaligned";
        case 5:  return "Load access fault";
        case 6:  return "Store/AMO address misaligned";
        case 7:  return "Store/AMO access fault";
        case 8:  return "System call";
        case 9:  return "Environment call from S-mode";
        case 11: return "Environment call from M-mode";
        case 12: return "Instruction page fault";
        case 13: return "Load page fault";
        case 15: return "Store/AMO page fault";
        default: return 0;
        }
    } else {
        switch (code) {
        case 1:  return "Supervisor software interrupt";
        case 3:  return "Machine software interrupt";
        case 5:  return "Supervisor timer interrupt";
        case 7:  return "Machine timer interrupt";
        case 9:  return "Supervisor external interrupt";
        case 11: return "Machine external interrupt";
        default: return unknown_int;
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s kernel.elf mem.dump\n", argv[0]);
        return 1;
    }

    struct elf_info info;
    parse_elf(argv[1], &info);

    int fd = open(argv[2], O_RDONLY);
    if (fd < 0) die(argv[2]);

    struct stat st;
    if (fstat(fd, &st) < 0) die(argv[2]);

    uint8_t *dump = mmap(NULL, (size_t)st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (dump == MAP_FAILED) die("mmap dump");

    uint64_t dump_base = info.kernel_base;

    uint64_t lh_off  = info.log_header_addr - dump_base;
    uint64_t log_off = info.log_addr        - dump_base;

    if (lh_off + sizeof(struct log_header) > (uint64_t)st.st_size)
        die_msg("log_header is outside dump (wrong dump base or wrong dump file)");
    if (log_off + sizeof(struct log_entry) > (uint64_t)st.st_size)
        die_msg("log[] is outside dump (wrong dump base or wrong dump file)");

    struct log_header *lh = (struct log_header *)(dump + lh_off);
    struct log_entry  *log = (struct log_entry *)(dump + log_off);

    /* Compute total entries to size the array conservatively */
    uint64_t total_entries = 0;
    for (int i = 0; i < LOG_N_LEVELS; i++)
        total_entries += lh->sizes[i];

    size_t cap = (total_entries > 0 && total_entries < (1u<<26)) ? (size_t)total_entries : 1024;
    size_t n = 0;
    struct event *events = malloc(cap * sizeof(*events));
    if (!events) die("malloc events");

    uint32_t offset = 0;
    for (int lvl = 0; lvl < LOG_N_LEVELS; lvl++) {
        uint32_t size = lh->sizes[lvl];
        uint32_t ts   = lh->ts[lvl];

        if (size == 0) {
            continue;
        }

        uint32_t start = (ts > size) ? (ts - size) : 0;

        for (uint32_t seq = start; seq < ts; seq++) {
            uint32_t idx = seq % size;
            /* bounds check within dump: log[offset+idx] */
            uint64_t ent_off = log_off + (uint64_t)(offset + idx) * sizeof(struct log_entry);
            if (ent_off + sizeof(struct log_entry) > (uint64_t)st.st_size)
                continue;

            if (n == cap) {
                cap *= 2;
                events = realloc(events, cap * sizeof(*events));
                if (!events) die("realloc events");
            }
            events[n++].le = log[offset + idx];
        }

        offset += size;
    }

    qsort(events, n, sizeof(*events), cmp_event);

    for (size_t i = 0; i < n; i++) {
        struct log_entry *le = &events[i].le;

        const struct log_event_desc *d = &log_event_desc[le->type];

        printf("[%u,%u,%u] %s self:%p",
               le->ts[0], le->ts[1], le->ts[2], d->name, (void *) le->self);

        const char *args[LOG_MAX_PAYLOAD];
        int lens[LOG_MAX_PAYLOAD];
        memset(args, 0, sizeof(args));
        if (d->arg_names != 0) {
            int index = 0;
            args[index] = d->arg_names;
            for (const char *p = d->arg_names;; p++) {
                if (*p == 0 || *p == ':') {
                    lens[index] = p - args[index];
                    index++;
                    if (*p == 0) break;
                    args[index] = p + 1;
                }
            }
        }

        for (unsigned j = 0; j < le->arity; j++) {
            uint64_t v = le->payload[j];
            enum log_arg_kind kind;
            if (args[j] == 0) kind = ARG_PTR;
            else {
                printf(" %.*s:", lens[j], args[j]);
                kind = d->args[j];
            }

            switch (kind) {
            case ARG_CHAR:
                if (v < 128 && v >= 32)
                    printf("'%c'", (char) v);
                else
                    switch (v) {
                    case USER_GET_NO_INPUT:   printf("NO_INPUT");   break;
                    case USER_GET_GOT_FOCUS:  printf("GOT_FOCUS");  break;
                    case USER_GET_LOST_FOCUS: printf("LOST_FOCUS"); break;
                    case '\t':                printf("<TAB>");      break;
                    case '\n':                printf("<NEWLINE>");  break;
                    case '\r':                printf("<RETURN>");   break;
                    case 0177:                printf("<DELETE>");   break;
                    default:
                        printf("0x%02" PRIx64, v);
                    }
                break;

            case ARG_INT:
                printf("%lld", (long long) v);
                break;

            case ARG_UINT:
                printf("%llu", (unsigned long long) v);
                break;

            case ARG_PTR:
                printf("0x%" PRIx64, v);
                break;

            case ARG_CELL:
                print_cell(v);
                break;

            case ARG_CAUSE:
                {
                    char *scause = scause_str(v);
                    if (scause == 0) printf("0x%" PRIx64, v);
                    else printf("'%s'", scause);
                }
                break;
            }
        }

        printf("\n");
    }


    return 0;
}
