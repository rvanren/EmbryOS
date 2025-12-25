#include "libfdt.h"
#include "embryos.h"

static void indent(int depth) {
    for (int i = 0; i < depth; i++)
        kprintf("  ");
}

// Try to print a property as one or more ASCII strings if possible.
static void print_prop_data(const void *data, int len) {
    const unsigned char *d = data;

    // Check if this is a valid sequence of printable strings
    bool all_printable = true;
    int strings = 0, i = 0;

    while (i < len) {
        while (i < len && d[i] != '\0') {
            if (d[i] < 0x20 || d[i] > 0x7E) {  // non-printable
                all_printable = false;
                break;
            }
            i++;
        }
        if (i == len) { all_printable = false; break; } // missing null terminator
        strings++;
        i++; // skip null
        if (!all_printable) break;
    }

    if (all_printable && strings > 0 && d[len-1] == '\0') {
        bool all_empty = true;
        i = 0;
        for (int s = 0; s < strings; s++) {
            if (strlen((const char *)(d + i)) != 0)
                all_empty = false;
            i += strlen((const char *)(d + i)) + 1;
        }
        if (!all_empty) {
            kprintf(" ");
            i = 0;
            for (int s = 0; s < strings; s++) {
                const char *str = (const char *)(d + i);
                if (s > 0) kprintf(", ");
                kprintf("\"%s\"", str);
                i += strlen(str) + 1;
            }
            return;
        }
    }
    if (len == 4) kprintf(" 0x%x", (unsigned)fdt32_to_cpu(*(fdt32_t *)d));
    else if (len == 8) {
        const fdt64_t *val = (const fdt64_t *)d;
        kprintf(" 0x%X", (uword_t)fdt64_to_cpu(*val));
    }
    else {
        for (int i = 0; i < len; i++)
            kprintf(" %x", d[i]);
    }
}

static void dump_node(const void *fdt, int offset, int depth) {
    const char *name = fdt_get_name(fdt, offset, NULL);
    indent(depth);
    kprintf("%s {\n", name);

    // Print all properties in this node
    int prop_off;
    fdt_for_each_property_offset(prop_off, fdt, offset) {
        const struct fdt_property *prop = fdt_get_property_by_offset(fdt, prop_off, NULL);
        const char *pname = fdt_string(fdt, fdt32_to_cpu(prop->nameoff));
        int len = fdt32_to_cpu(prop->len);
        indent(depth + 1);
        kprintf("%s =", pname);
        const unsigned char *d = (const unsigned char *)prop->data;
        print_prop_data(d, len);
        kprintf("\n");
    }


    // Recurse into subnodes
    int sub;
    fdt_for_each_subnode(sub, fdt, offset)
        dump_node(fdt, sub, depth + 1);

    indent(depth);
    kprintf("}\n");
}

void fdt_dump(void *fdt) {
    if (fdt_check_header(fdt)) {
        kprintf("Bad FDT header!\n");
        return;
    }

    kprintf("Flattened Device Tree @ %X, size=%u bytes\n",
            (uword_t) (uintptr_t) fdt, (unsigned int) fdt_totalsize(fdt));

    int root = fdt_path_offset(fdt, "/");
    dump_node(fdt, root, 0);
}

void fdt_memory_range(const void *fdt, uintptr_t *base_out, uintptr_t *end_out)
{
    int root = fdt_path_offset(fdt, "/");
    if (root < 0) die("bad FDT");

    int addr_cells = fdt_address_cells(fdt, root);
    int size_cells = fdt_size_cells(fdt, root);
    if (addr_cells <= 0 || addr_cells > 2) addr_cells = 2;
    if (size_cells <= 0 || size_cells > 2) size_cells = 2;

    // Find a node whose device_type == "memory" or compatible == "memory"
    int off;
    fdt_for_each_subnode(off, fdt, 0) {
        const char *dtype = fdt_getprop(fdt, off, "device_type", NULL);
        const char *comp  = fdt_getprop(fdt, off, "compatible", NULL);
        if ((dtype && !strcmp(dtype, "memory")) || (comp && !strcmp(comp, "memory")))
            break;
    }
    if (off < 0)
        die("no /memory node found");
    int len;
    const fdt32_t *reg = fdt_getprop(fdt, off, "reg", &len);
    if (!reg) die("no reg property in /memory");
    if (len < (addr_cells + size_cells) * 4)
        die("short reg property in /memory");

    uint64_t base64 = 0, size64 = 0;
    for (int i = 0; i < addr_cells; i++)
        base64 = (base64 << 32) | fdt32_to_cpu(reg[i]);
    for (int i = 0; i < size_cells; i++)
        size64 = (size64 << 32) | fdt32_to_cpu(reg[addr_cells + i]);

    *base_out = (uintptr_t) base64;
    *end_out  = (uintptr_t) (base64 + size64);
}

static uintptr_t fdt_read_reg_base(const void *fdt, int node) {
    int parent = fdt_parent_offset(fdt, node), len, ac = 1;
    const fdt32_t *prop;
    prop = fdt_getprop(fdt, parent, "#address-cells", &len);
    if (prop && len == 4) ac = fdt32_to_cpu(*prop);
    prop = fdt_getprop(fdt, node, "reg", &len);
    if (!prop) return 0;
    const fdt32_t *reg = prop;
    uint64_t addr = 0;
    for (int i = 0; i < ac; i++)
        addr = (addr << 32) | fdt32_to_cpu(reg[i]);
    return (uintptr_t)addr;
}

static int fdt_get_stdout_node(const void *fdt) {
    int chosen = fdt_path_offset(fdt, "/chosen");
    if (chosen < 0) return -1;
    int len;
    const char *path = fdt_getprop(fdt, chosen, "stdout-path", &len);
    if (!path)
        path = fdt_getprop(fdt, chosen, "linux,stdout-path", &len);
    if (!path) return -1;
    char buf[128];
    int i = 0;
    while (i < sizeof(buf)-1 && path[i] && path[i] != ':') {
        buf[i] = path[i];
        i++;
    }
    buf[i] = 0;
    int node = fdt_path_offset(fdt, buf);
    if (node >= 0) return node;
    int aliases = fdt_path_offset(fdt, "/aliases");
    if (aliases >= 0) {
        const char *alias = fdt_getprop(fdt, aliases, buf, NULL);
        if (alias) return fdt_path_offset(fdt, alias);
    }
    return -1;
}

const char *fdt_uart_info(const void *fdt, int *plen, uintptr_t *pbase) {
    int node = fdt_get_stdout_node(fdt);
    if (node < 0) return 0;
    *pbase = fdt_read_reg_base(fdt, node);
    return fdt_getprop(fdt, node, "compatible", plen);
}

uint32_t fdt_get_timebase(void *fdt) {
    int cpus = fdt_path_offset(fdt, "/cpus");
    if (cpus < 0) {
        kprintf("FDT: no /cpus node, defaulting to 10 MHz");
        return 10000000;   // fallback
    }

    int len;
    const uint32_t *prop = fdt_getprop(fdt, cpus, "timebase-frequency", &len);
    if (!prop || len < 4) {
        kprintf("FDT: no timebase-frequency, defaulting to 10 MHz");
        return 10000000;
    }

    // Property values in FDT are big-endian
    return fdt32_to_cpu(*prop);
}
