#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app_info.h"

static void emit_array(FILE *out, const char *sym, const unsigned char *buf, size_t len) {
    fprintf(out, "const unsigned char %s[] = {", sym);

    for (size_t i = 0; i < len; i++) {
        if (i % 12 == 0) fprintf(out, "\n    ");
        fprintf(out, "0x%02x,", buf[i]);
    }
    if (len > 0) fprintf(out, "\n");
    fprintf(out, "};\n\n");
}

static char *make_symbol(const char *filename) {
    // Symbol: file_<basename>_<hexlen>
    const char *base = strrchr(filename, '/');
    base = base ? base + 1 : filename;

    char *sym = malloc(strlen(base) + 16);
    for (size_t i = 0; ; i++) {
        if      (base[i] == '\0') { sym[i] = '\0'; break; }
        else if ( (base[i] >= 'a' && base[i] <= 'z') ||
                  (base[i] >= 'A' && base[i] <= 'Z') ||
                  (base[i] >= '0' && base[i] <= '9') )
            sym[i] = base[i];
        else
            sym[i] = '_';
    }

    return sym;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s file1 file2 ... > out.c\n", argv[0]);
        return 1;
    }

    printf("#include \"embryos.h\"\n\n");
    for (int i = 1; i < argc; i++) {
        const char *fname = argv[i];
        FILE *f = fopen(fname, "rb");
        if (!f) { perror(fname); continue; }

        fseek(f, 0, SEEK_END);
        long len = ftell(f);
        fseek(f, 0, SEEK_SET);

        unsigned char *buf = malloc(len);
        if (fread(buf, 1, len, f) != (size_t)len) {
            perror(fname);
            fclose(f);
            free(buf);
            continue;
        }
        fclose(f);

        char *sym = make_symbol(fname);
        printf("// File: %s (%ld bytes)\n", fname, len);
        emit_array(stdout, sym, buf, len);
        free(buf);
        free(sym);
    }

    printf("const struct embedded_file embedded_files[] = {\n");
    for (int i = 1; i < argc; i++) {
        char *sym = make_symbol(argv[i]);
        FILE *f = fopen(argv[i], "rb");
        fseek(f, 0, SEEK_END);
        long len = ftell(f);
        fclose(f);
        printf("    { \"%s\", %s, %ld },\n", argv[i], sym, len);
        free(sym);
    }
    printf("    { NULL, NULL, 0 }\n");
    printf("};\n");

    return 0;
}
