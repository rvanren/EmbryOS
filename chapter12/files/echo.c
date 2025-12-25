int main(int argc, uint64_t *argv) {
  int i;
  i = 1;
  while (i < argc) {
      if (i > 1) write(1, " ", 1);
      write(1, (uint64_t *) *(argv + i), 8);
      i = i + 1;
  }
  write(1, "\n", 1);
}
