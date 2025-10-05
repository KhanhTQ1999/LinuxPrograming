# 📘 Linux Programming – Session 1

## 🛠️ 1. Build Process in C

A C compiler converts source files into executable binaries through **four main phases**:

| Phase           | Description                                                                 | Output File | Example Command                              |
|----------------|------------------------------------------------------------------------------|-------------|----------------------------------------------|
| Pre-processing  | Removes comments, expands macros, includes headers, handles conditional code | `.i`        | `gcc -E main.c -o main.i`                    |
| Compiling       | Converts preprocessed code into assembly instructions                        | `.s`        | `gcc -S main.c -o main.s`                    |
| Assembling      | Translates assembly code into machine code                                   | `.o`        | `gcc -c main.c -o main.o`                    |
| Linking         | Resolves function calls and combines all code into an executable             | executable  | `gcc main.o -o main`                         |

📌 To generate all intermediate files in one go, use:

```bash
gcc -Wall -save-temps source_file.c -o output_file
```

## 📚 2. Static / Shared Library
| Properties            | Static Library                                                                                   | Shared Library                                                                                                 |
|-----------------------|--------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------|
| Linking time          | Happens as the last step of the compilation process. After the program is placed in memory       | Added during linking process when executable file and libraries are added to memory                            |
| Means                 | Performed by linkers                                                                             | Performed by operating system                                                                                  |
| Size                  | Much bigger, because external programs are built into the executable file                        | Much smaller, only one copy of dynamic library is kept in memory                                               |
| External file changes | Executable must be recompiled if external files change                                           | No need to recompile the executable                                                                            |
| Time                  | Takes longer to execute, loading into memory happens every time                                  | Faster, shared library code is already in memory                                                               |
| Compatibility         | No compatibility issues, all code is in one executable module                                    | Dependent on having a compatible library; program will not work if library is removed from the system          |

## ⚙️ 3. Makefile
A Makefile is a script used by the make build automation tool to compile and link a program.

The make program reads the contents of the Makefile and executes it.

📌 Naming conventions: Makefile, makefile, or *.mk

📐 Structure of a Makefile
makefile
target: dependencies
    command
target: The file to be generated

dependencies: The files required to build the target

command: The shell command to build the target (must start with a tab character)

🔁 Automatic Variables
| Variable | Description                                 |
|----------|---------------------------------------------|
| $@       | The name of the target in the current rule  |
| $<       | The first prerequisite (dependency)         |
| $?       | Prerequisites newer than the target         |
| $^       | All prerequisites                           |