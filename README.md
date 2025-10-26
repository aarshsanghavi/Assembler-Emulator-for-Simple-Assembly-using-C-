# Assembler and Emulator for Simple Assembly Language

A comprehensive two-pass assembler and emulator implementation for a custom 32-bit instruction set architecture (ISA), complete with an interactive Streamlit web interface for visualization and debugging.

## 📋 Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Features](#features)
- [Project Structure](#project-structure)
- [Installation](#installation)
- [Usage](#usage)
  - [Using the Assembler](#using-the-assembler)
  - [Using the Emulator](#using-the-emulator)
  - [Using the Streamlit UI](#using-the-streamlit-ui)
- [Instruction Set Architecture](#instruction-set-architecture)
- [Assembly Language Syntax](#assembly-language-syntax)
- [Examples](#examples)
- [Output Files](#output-files)
- [Error Detection](#error-detection)
- [Requirements](#requirements)
- [Author](#author)

---

## 🎯 Overview

This project provides a complete toolchain for writing, assembling, and executing programs in a custom assembly language. It simulates a simple stack-based architecture with support for arithmetic operations, memory manipulation, branching, and function calls.

**Key Components:**
- **Two-Pass Assembler** (`Mainassembler.cpp`): Converts assembly code (.asm) into machine code (.o)
- **Emulator** (`Mainemulator.cpp`): Executes machine code and simulates CPU behavior
- **Streamlit UI** (`App.py`): Web-based interface for easy interaction and visualization

---

## 🏗️ Architecture

The emulator simulates a **32-bit stack-based architecture** with the following components:

### Registers
- **A (Accumulator)**: Primary register for arithmetic and logic operations
- **B**: Secondary register for two-operand instructions
- **PC (Program Counter)**: Points to the current instruction
- **SP (Stack Pointer)**: Points to the top of the stack

### Memory
- **24-bit address space** (16,777,216 memory locations)
- Each memory location stores a 32-bit word
- Supports both stack-based and direct memory access

### Instruction Format
```
┌─────────────────────────────┬──────────┐
│      Operand (24 bits)      │ Opcode   │
│                             │ (8 bits) │
└─────────────────────────────┴──────────┘
 31                         8  7        0
```

---

## ✨ Features

### Assembler Features
- ✅ **Two-pass assembly** for label resolution
- ✅ **Label support** with colon syntax (`label:`)
- ✅ **Multiple number formats**: Decimal, Octal (0...), Hexadecimal (0x...)
- ✅ **Comprehensive error detection** (6+ error types)
- ✅ **Comment support** using semicolon (`;`)
- ✅ **SET directive** for assigning values to labels
- ✅ **data directive** for storing constants
- ✅ **Generates three output files**: .o (object), .lst (listing), .log (error log)

### Emulator Features
- ✅ **Complete ISA implementation** (19 instructions)
- ✅ **Multiple execution modes**: full run, step-by-step, read/write tracking
- ✅ **Memory dump** capabilities
- ✅ **Register state inspection**
- ✅ **Memory read/write logging** for debugging
- ✅ **Stack operations** simulation
- ✅ **Branch and function call** support
- ✅ **Overflow protection** (instruction limit: 30 million)

### UI Features
- 🎨 **Interactive web interface** using Streamlit
- 📁 **File upload** for .asm and .o files
- 📊 **Real-time assembly** and execution
- 📝 **Output visualization** (listing, logs, memory dumps)
- 🔍 **Parsed output display** (initial/final memory, registers, read operations)

---

## 📁 Project Structure

```
Assembler-Emulator-for-Simple-Assembly-using-C-/
│
├── Assembler and Emulator/
│   ├── App.py                    # Streamlit web interface
│   ├── Mainassembler.cpp         # Two-pass assembler implementation
│   ├── Mainemulator.cpp          # CPU emulator implementation
│   ├── Mainassembler.exe         # Compiled assembler (Windows)
│   └── Mainemulator.exe          # Compiled emulator (Windows)
│
├── Test Files/                   # Example assembly programs
│   ├── simple.asm                # Simple addition example
│   ├── bubblesort.asm            # Bubble sort implementation
│   ├── Finding_max.asm           # Find maximum value
│   ├── Swap.asm                  # Swap two numbers
│   ├── sub.asm                   # Subtraction example
│   ├── Mysampletest1.asm         # Sample test 1
│   ├── Mysampletest2.asm         # Sample test 2
│   ├── test1.asm - test8.asm     # Various test programs
│   └── Test5.asm
│
└── README.md                     # This file
```

---

## 🚀 Installation

### Prerequisites
- **C++ Compiler**: GCC/G++ with C++17 support or MSVC (Visual Studio)
- **Python 3.8+** (for Streamlit UI)

### Step 1: Clone the Repository
```bash
git clone https://github.com/yourusername/Assembler-Emulator-for-Simple-Assembly-using-C-.git
cd Assembler-Emulator-for-Simple-Assembly-using-C-
```

### Step 2: Compile the Assembler and Emulator

**On Windows (PowerShell/CMD):**
```bash
cd "Assembler and Emulator"
g++ Mainassembler.cpp -o Mainassembler.exe
g++ Mainemulator.cpp -o Mainemulator.exe
```

**On Linux/macOS:**
```bash
cd "Assembler and Emulator"
g++ Mainassembler.cpp -o Mainassembler
g++ Mainemulator.cpp -o Mainemulator
```

### Step 3: Install Python Dependencies (Optional - for UI)
```bash
pip install streamlit
```

---

## 💻 Usage

### Using the Assembler

**Basic Usage:**
```bash
./Mainassembler.exe <filename.asm>
```

**Example:**
```bash
cd "Assembler and Emulator"
./Mainassembler.exe "../Test Files/simple.asm"
```

**Output:**
- `simple.o` - Binary object file
- `simple.lst` - Listing file with addresses and machine code
- `simple.log` - Compilation log (errors or success message)

### Using the Emulator

**Syntax:**
```bash
./Mainemulator.exe <objectfile.o> <command>
```

**Available Commands:**
- `-run`: Execute the entire program and show final state
- `-stepfull`: Execute step-by-step with memory dump after each instruction
- `-read`: Execute and display all memory read operations

**Examples:**
```bash
# Run the entire program
./Mainemulator.exe simple.o -run

# Step through with detailed output
./Mainemulator.exe simple.o -stepfull

# Track memory reads
./Mainemulator.exe simple.o -read
```

### Using the Streamlit UI

**Start the UI:**
```bash
cd "Assembler and Emulator"
streamlit run App.py
```

**Then:**
1. Open your browser to the displayed URL (usually `http://localhost:8501`)
2. Upload an `.asm` file or use a pre-assembled `.o` file
3. Click "Assemble" to compile the code
4. Select an emulator command and click "Run Emulator"
5. View the results in organized sections

---

## 🔧 Instruction Set Architecture

The ISA consists of **19 instructions** with three operand types:

### Instruction Types

| **Type** | **Description** | **Operand** |
|----------|----------------|-------------|
| Type 0 | No operand required | - |
| Type 1 | Value operand (immediate) | 24-bit signed integer |
| Type 2 | Offset operand (for branches) | Relative offset |

### Complete Instruction Set

| **Opcode** | **Mnemonic** | **Type** | **Description** |
|------------|--------------|----------|-----------------|
| 0 | `ldc value` | 1 | Load constant: B ← A, A ← value |
| 1 | `adc value` | 1 | Add constant: A ← A + value |
| 2 | `ldl offset` | 2 | Load local: B ← A, A ← Memory[SP + offset] |
| 3 | `stl offset` | 2 | Store local: Memory[SP + offset] ← A, A ← B |
| 4 | `ldnl offset` | 2 | Load non-local: A ← Memory[A + offset] |
| 5 | `stnl offset` | 2 | Store non-local: Memory[A + offset] ← B |
| 6 | `add` | 0 | Add: A ← B + A |
| 7 | `sub` | 0 | Subtract: A ← B - A |
| 8 | `shl` | 0 | Shift left: A ← B << A |
| 9 | `shr` | 0 | Shift right: A ← B >> A |
| 10 | `adj value` | 1 | Adjust SP: SP ← SP + value |
| 11 | `a2sp` | 0 | A to SP: SP ← A, A ← B |
| 12 | `sp2a` | 0 | SP to A: B ← A, A ← SP |
| 13 | `call offset` | 2 | Call subroutine: B ← A, A ← PC, PC ← PC + offset |
| 14 | `return` | 0 | Return: PC ← A, A ← B |
| 15 | `brz offset` | 2 | Branch if zero: if A == 0 then PC ← PC + offset |
| 16 | `brlz offset` | 2 | Branch if less than zero: if A < 0 then PC ← PC + offset |
| 17 | `br offset` | 2 | Branch unconditional: PC ← PC + offset |
| 18 | `HALT` | 0 | Stop execution |

### Special Directives

- **`data value`**: Store a 32-bit value in memory at the current location
- **`SET value`**: Assign a value to a label (does not generate code)

---

## 📝 Assembly Language Syntax

### Basic Syntax Rules

1. **Instructions**: One instruction per line
2. **Labels**: Must start with a letter, followed by alphanumeric characters or underscores, ending with `:`
3. **Comments**: Start with `;` and continue to end of line
4. **Number Formats**:
   - Decimal: `123`, `-45`
   - Octal: `0177`, `0755`
   - Hexadecimal: `0xFF`, `0x1A2B`

### Example Syntax

```asm
; This is a comment
loop:                ; Label definition
    ldc 10           ; Load constant 10 into A
    stl 0            ; Store A at Memory[SP + 0]
    ldl 0            ; Load from Memory[SP + 0] into A
    adc -1           ; Add -1 to A
    brz end          ; Branch to 'end' if A is zero
    br loop          ; Unconditional branch to 'loop'
end:
    HALT             ; Stop execution
```

### Using the SET Directive

```asm
MAX: SET 100         ; Define MAX as 100
    ldc MAX          ; Load 100 into A
    stl 0            ; Store at Memory[SP + 0]
    HALT
```

---

## 📚 Examples

### Example 1: Simple Addition
```asm
; Add two numbers and store the result
    ldc 5            ; Load 5 into A
    stl 0            ; Store at [SP + 0]
    ldc 10           ; Load 10 into A
    ldl 0            ; Load [SP + 0] (5) into A, B ← A (10)
    add              ; A ← B + A = 10 + 5 = 15
    stl 1            ; Store result at [SP + 1]
    HALT
```

### Example 2: Sum of Array
```asm
; Sum 5 numbers stored in memory
    ldc 6            ; First number
    stl 0
    ldc 9            ; Second number
    stl 1
    ldc 4
    stl 2
    ldc 9
    stl 3
    ldc 2
    stl 4
    
    ldc 5            ; Counter
    stl 5
    ldc 0            ; Initialize sum
    stl 6
    ldc 0            ; Base pointer
    stl 7

add_loop:
    ldl 5            ; Load counter
    brz end_add      ; If zero, exit
    
    ldl 7            ; Load base pointer
    ldnl 0           ; Load value at pointer
    ldl 6            ; Load current sum
    add              ; Add to sum
    stl 6            ; Store result
    
    ldl 7
    adc 1            ; Increment pointer
    stl 7
    
    ldl 5
    adc -1           ; Decrement counter
    stl 5
    
    br add_loop

end_add:
    HALT
```

### Example 3: Swap Two Numbers
```asm
; Swap values at [SP + 0] and [SP + 1]
    ldc 42
    stl 0            ; Store 42 at [SP + 0]
    ldc 17
    stl 1            ; Store 17 at [SP + 1]
    
    ldl 0            ; Load [SP + 0]
    stl 2            ; Temp = [SP + 0]
    ldl 1
    stl 0            ; [SP + 0] = [SP + 1]
    ldl 2
    stl 1            ; [SP + 1] = Temp
    HALT
```

---

## 📄 Output Files

### 1. Object File (.o)
- Binary file containing 32-bit machine code instructions
- Directly executable by the emulator
- Format: Sequence of 32-bit words

### 2. Listing File (.lst)
Shows each line with address and machine code:
```
00000000 00000600 ldc 6
00000001 03000000 stl 0
00000002 00000900 ldc 9
...
```
Format: `[Address] [MachineCode] [SourceCode]`

### 3. Log File (.log)
Contains compilation status:
```
Compiled without any error
```
Or lists all errors found:
```
ERROR: Duplicate label definition at line number 5
ERROR: Invalid operand at line number 12
```

---

## ⚠️ Error Detection

The assembler detects **6 types of errors**:

| **Error Code** | **Description** | **Example** |
|----------------|-----------------|-------------|
| 0 | Invalid file format | Non-.asm file extension |
| 1 | Extraneous data at line end | `ldc 5 10` (too many operands) |
| 2 | Invalid label name | `123label:` (starts with digit) |
| 3 | Duplicate label definition | Defining `loop:` twice |
| 4 | Unrecognized mnemonic | `xyz 5` (unknown instruction) |
| 5 | Invalid operand | `ldc abc` (non-existent label) |

---

## 📦 Requirements

### For Compilation
- **C++17 Compatible Compiler**
  - GCC 7.0+ / Clang 5.0+ / MSVC 2017+
- **Standard Template Library (STL)**

### For Streamlit UI
- **Python 3.8 or higher**
- **Streamlit**: `pip install streamlit`

### Tested On
- Windows 10/11 (PowerShell, CMD)
- Linux (Ubuntu 20.04+)
- macOS (Big Sur+)

---

## 👨‍💻 Author

**Aarsh Sanghavi**  
Roll No: 2301CS01

---

## 📝 License

This project is available for educational purposes. Feel free to use, modify, and distribute with attribution.

---

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to check the issues page.

---

## 🙏 Acknowledgments

This project was developed as part of a computer architecture and systems programming course, demonstrating the fundamentals of:
- Assembly language design
- Two-pass assembler implementation
- Virtual machine/emulator development
- ISA design and implementation

---

**Happy Coding! 🚀**
