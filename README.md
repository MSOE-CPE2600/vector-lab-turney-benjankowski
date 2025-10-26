# Vector Calculator
A Matlab style command line interface to perform vector operations.

## Build
Makefile included. Use `make build` to build and `make clean` to cleanup after build

### Version 1
- Does in memory operations
- Store
- Print
- Add
- Subtract
- Multiply
- Dot Product
- Cross Product

### Version 2
- Adds storing and loading vectors from CSV
- Supports unlimited* vectors
  - Done by making a automatically sized array of varaibles using realloc
