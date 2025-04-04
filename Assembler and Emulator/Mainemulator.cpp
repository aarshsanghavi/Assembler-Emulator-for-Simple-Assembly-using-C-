#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <sstream>
#include <iomanip>
using namespace std;

constexpr int MEMORY_SIZE = 1 << 24;
int Memory[MEMORY_SIZE];    // Main memory array
vector<string> code;        // Machine code instructions

int A = 0, B = 0, PC = 0, SP = 0, execCount = 0;
array<int, 2> memChange;
bool execFlag;

vector<string> mnemonics = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", 
                            "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", 
                            "brz", "brlz", "br", "HALT"};

// Converts integer to a hex string with padding
string intToHex(int num, int width = 8) 
{
    stringstream ss;
    ss << uppercase << hex << setw(width) << setfill('0') << (num & 0xFFFFFFFF);
    return ss.str();
    
}

// Handles memory access with boundary check
bool isValidMemoryAddress(int addr) {
    return addr >= 0 && addr < MEMORY_SIZE;
}

// Bound-safe increment for PC after instruction
void incrementPC() {
    PC++;
    if (PC >= code.size()) {
        cout << "PC out of bounds - terminating execution.\n";
        exit(1);
    }
}

// Functions to emulate instructions
void ldc(int value) { B = A; A = value; }
void adc(int value) { A += value; }
void ldl(int offset) { B = A; A = Memory[SP + offset]; memChange = {SP + offset, 0}; execFlag = true; }
void stl(int offset) { memChange = {Memory[SP + offset], A}; Memory[SP + offset] = A; execFlag = true; A = B; }
void ldnl(int offset) { A = Memory[A + offset]; memChange = {A + offset, 0}; execFlag = true; }
void stnl(int offset) { memChange = {Memory[A + offset], B}; Memory[A + offset] = B; execFlag = true; }
void add(int) { A += B; }
void sub(int) { A = B - A; }
void shl(int) { A = B << A; }
void shr(int) { A = B >> A; }
void adj(int value) { SP += value; }
void a2sp(int) { SP = A; A = B; }
void sp2a(int) { B = A; A = SP; }
void call(int offset) { B = A; A = PC; PC += offset; if (PC < 0 || PC >= code.size()) exit(1); }
void ret(int) { PC = A; A = B; }
void brz(int offset) { if (A == 0) PC += offset; if (PC < 0 || PC >= code.size()) exit(1); }
void brlz(int offset) { if (A < 0) PC += offset; if (PC < 0 || PC >= code.size()) exit(1); }
void br(int offset) { PC += offset; if (PC < 0 || PC >= code.size()) exit(1); }

using InstructionFunc = void (*)(int);
InstructionFunc executeInstruction[] = {ldc, adc, ldl, stl, ldnl, stnl, add, sub, 
                                        shl, shr, adj, a2sp, sp2a, call, ret, brz, brlz, br};

// Reads machine code from a file into memory
void loadMachineCode() {
    cout << "Enter file name (e.g., machineCode.o): ";
    string fileName;
    cin >> fileName;
    ifstream file(fileName, ios::in | ios::binary);
    unsigned int read;
    int pos = 0;
    while (file.read(reinterpret_cast<char*>(&read), sizeof(read))) {
        Memory[pos++] = read;
        code.push_back(intToHex(read));
    }
    file.close();
}

// Display commands available in the emulator
void displayInstructions() {
    cout << "Emulator Options:\n"
         << "1. Memory Dump: -dump\n"
         << "2. Step-by-Step Execution: -t\n"
         << "3. Run Until Halt: -run\n"
         << "4. Show Registers: -reg\n"
         << "5. Show Instruction Set: -isa\n"
         << "6. Show Read Operations: -read\n"
         << "7. Show Write Operations: -write\n";
}

// Dumps memory contents in 4-byte chunks
void memoryDump() {
    for (int i = 0; i < code.size(); i += 4) {
        cout << intToHex(i) << " ";
        for (int j = i; j < min(i + 4, (int)code.size()); ++j) {
            cout << intToHex(Memory[j]) << " ";
        }
        cout << endl;
    }
}

// Displays register and SP values
void displayRegisters() {
    cout << "A: " << intToHex(A) << "  B: " << intToHex(B) 
         << "  SP: " << intToHex(SP) << "  PC: " << intToHex(PC + 1) 
         << endl;
}

// Shows memory read operations
void showReadOperation() {
    cout << "Read from memory[" << intToHex(memChange[0]) 
         << "] with value: " << intToHex(Memory[memChange[0]]) << endl;
}

// Shows memory write operations
void showWriteOperation() {
    cout << "Write to memory[" << intToHex(memChange[0]) << "] changed to " 
         << intToHex(memChange[1]) << endl;
}

// Lists all instruction opcodes and mnemonics
void listInstructions() {
    cout << "Opcode    Mnemonic\n";
    for (int i = 0; i < mnemonics.size(); ++i) {
        cout << i << "   " << mnemonics[i] << '\n';
    }
}

// Runs the loaded machine code
bool executeCode(int mode, int limit = (1 << 25)) 
{
    while (limit-- && PC < code.size()) {
        execCount++;
        if (execCount > 3e7) {
            cout << "Segmentation Fault: Instruction limit exceeded.\n";
            return false;
        }
        
        string instruction = code[PC];
        int opcode = stoi(instruction.substr(6, 2), nullptr, 16);
        int operand = stoi(instruction.substr(0, 6), nullptr, 16);
        if (operand >= (1 << 23)) operand -= (1 << 24);

        if (opcode == 18) { // HALT
            cout << "Execution Halted.\n";
            return true;
        }

        executeInstruction[opcode](operand);
        
        if (mode == 1 && execFlag) showReadOperation();
        else if (mode == 2 && execFlag) showWriteOperation();
        
        execFlag = false;
        incrementPC();
    }
    displayRegisters();
    return true;
}

// Handles command input for the emulator
bool runEmulator() 
{
    cout << "Enter command or 0 to exit:\n";
    string command;
    cin >> command;
    if (command == "0") return false;
    else if (command == "-dump") memoryDump();
    else if (command == "-reg") displayRegisters();
    else if (command == "-t") executeCode(0, 1);
    else if (command == "-run") executeCode(0);
    else if (command == "-isa") listInstructions();
    else if (command == "-read") executeCode(1);
    else if (command == "-write") executeCode(2);
    else cout << "Invalid command. Try again.\n";
    return true;
}

signed main() {
    loadMachineCode();
    displayInstructions();
    while (runEmulator());
    return 0;
}
