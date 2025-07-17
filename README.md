# Simple Assembler and Emulator

This project implements a full toolchain (assembler + emulator) for a custom 32-bit instruction set architecture, including a Streamlit UI for interaction and visualization.

## Assemble a Program

```bash
g++ assembler/assembler.cpp -o assembler
./assembler test_programs/example.s
```

## Run the Emulator

```bash
g++ emulator/emulator.cpp -o emulator
./emulator example.o -run        # Run the full program
./emulator example.o -dump       # Memory dump
./emulator example.o -reg        # Show registers
./emulator example.o -read       # Show all memory reads
./emulator example.o -write      # Show all memory writes
```

## Streamlit UI (Optional)

```bash
streamlit run ui/run_ui.py
```

## Project Structure

```
project_root/
│
├── assembler/             # Two-pass assembler  
│   └── assembler.cpp      # Parses, validates, and emits .o, .lst, .log files
│
├── emulator/              # Emulator / virtual CPU implementation  
│   └── emulator.cpp       # Executes .o files with register/memory simulation
│
├── test_programs/         # Sample assembly programs  
│   └── example.s
│
├── ui/                    # Streamlit UI (optional)  
│   └── run_ui.py
│
├── README.md
└── .gitignore
```

## Example Program

```asm
ldc 5
stl 0
ldc 10
stl 1
ldl 0
stl 5
ldl 1
stl 0
ldl 5
stl 1
HALT
```

## Output Files

- `example.o`: Binary object file (32-bit instructions)
- `example.lst`: Instruction listing with addresses
- `example.log`: Any warnings/errors encountered

## Features

- Two-pass assembler with support for 18+ instructions, labels, constants
- Supports decimal, octal, and hex numbers with 6+ types of error detection
- Emulator simulates register state, stack operations, memory access, branching
- Logs all memory read and write operations with detailed info
- Streamlit UI for uploading `.asm` files, assembling, running, and viewing output interactively

## Requirements

- C++17 compiler
- Python 3.8+ (for Streamlit UI)

## License

MIT License
