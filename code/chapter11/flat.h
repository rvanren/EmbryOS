struct flat;

int  flat_create(struct flat *fs);              // allocate new file, return file #
int  flat_read(struct flat *fs, int file, int off, void *dst, int n);
int  flat_write(struct flat *fs, int file, int off, const void *src, int n);
int  flat_size(struct flat *fs, int file);
void flat_delete(struct flat *fs, int file);
