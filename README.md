# Assembler and Emulator for a Simple Assembly Language

This project implements a custom two-pass assembler and an emulator (virtual CPU) for a toy assembly language with 18+ unique instructions, supporting branching, stack operations, and arithmetic.

---

##  Features

-  Two-pass assembler with label resolution and symbol table generation
-  Supports decimal, octal, and hexadecimal numbers
-  Emits `.o` (binary), `.lst` (listing), and `.log` (errors/warnings)
-  Emulator executes real instruction flow with branching, arithmetic, and stack support
-  Read and write memory operations are traced and visualized
-  Streamlit UI for uploading `.s` files and seeing assembled output, decoded instructions, and final memory/register state

---

## Usage

Assemble a Program

g++ assembler/assembler.cpp -o assembler
./assembler test_programs/example.s


Run the Emulator
g++ emulator/emulator.cpp -o emulator
./emulator example.o -run        # run the full program
./emulator example.o -dump       # memory dump
./emulator example.o -reg        # show registers
./emulator example.o -read       # show all memory reads
./emulator example.o -write      # show all memory writes
UI (optional)
bash
Copy
Edit
streamlit run ui/run_ui.py
Project Structure
graphql
Copy
Edit
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
Example Program
asm
Copy
Edit
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
Output Files
example.o: Binary object file (32-bit instructions)

example.lst: Instruction listing with addresses

example.log: Any warnings/errors encountered

Requirements
C++17 compiler

Python 3.8+ (for Streamlit UI)

License
MIT License

