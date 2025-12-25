# Platforms

This directory contains a subdirectory for each platform on which you might
want to run EmbryOS.  While EmbryOS tries to find out various configuration
details on its own, some need to be used during compilation, such as where
the kernel is loaded.

Each subdirectory contains the following files:

| File | Purpose |
|------|---------|
| `Makefile` | Platform-specific part that `make` needs |
| `kernel.lds` | Memory layout |
| `config.c` | Platform-parameters that cannot be gleaned automatically |

## 📄 License

Not yet
