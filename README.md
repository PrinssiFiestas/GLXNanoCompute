# GLX Nano Compute

Absolutely minimal (about 100 loc) OpenGL compute shader example using GLX. No external dependencies on Linux.

This demonstrates a method of doing arbitrary computations on GPU. There is no windows, event loops, or any other bloat, the program just assigns 7 to an integer on GPU and prints the result. It does have some basic error handling, but it is very much incomplete.

This is not meant to be a tutorial, just a minimal template, so comments are limited. However, the program is so short that anybody should be able to just read the docs for each function call.

## Build

Build:

```bash
make
```
