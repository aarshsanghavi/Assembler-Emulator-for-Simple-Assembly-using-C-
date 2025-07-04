#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <sstream>
#include <iomanip>
using namespace std;

constexpr int MEMORY_SIZE = 1 << 24;
int Memory[MEMORY_SIZE];  // Main memory
vector<uint32_t> instructions;  // Raw 32-bit machine code

int A = 0, B = 0, PC = 0, SP = 0, execCount = 0;
array<int, 2> memChange;
bool execFlag;

vector<string> mnemonics = {
    "ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub",
    "shl", "shr", "adj", "a2sp", "sp2a", "call", "return",
    "brz", "brlz", "br", "HALT"
};

string intToHex(int num, int width = 8) {
    stringstream ss;
    ss << uppercase << hex << setw(width) << setfill('0') << (num & 0xFFFFFFFF);
    return ss.str();
}

bool isValidMemoryAddress(int addr) {
    return addr >= 0 && addr < MEMORY_SIZE;
}

void incrementPC() {
    PC++;
    if (PC >= instructions.size()) {
        cout << "PC out of bounds - terminating execution.\n";
        exit(1);
    }
}

// Instruction implementations
void ldc(int value) { B = A; A = value; }
void adc(int value) { A += value; }
void ldl(int offset) { B = A; A = Memory[SP + offset]; memChange = {SP + offset, 0}; execFlag = true; }
void stl(int offset) { memChange = {SP + offset, Memory[SP + offset]}; Memory[SP + offset] = A; execFlag = true; A = B; }
void ldnl(int offset) { memChange = {A + offset, 0}; A = Memory[A + offset]; execFlag = true; }
void stnl(int offset) { memChange = {A + offset, Memory[A + offset]}; Memory[A + offset] = B; execFlag = true; }
void add(int) { A += B; }
void sub(int) { A = B - A; }
void shl(int) { A = B << A; }
void shr(int) { A = B >> A; }
void adj(int value) { SP += value; }
void a2sp(int) { SP = A; A = B; }
void sp2a(int) { B = A; A = SP; }
void call(int offset) { B = A; A = PC; PC += offset; if (PC < 0 || PC >= instructions.size()) exit(1); }
void ret(int) { PC = A; A = B; }
void brz(int offset) { if (A == 0) PC += offset; if (PC < 0 || PC >= instructions.size()) exit(1); }
void brlz(int offset) { if (A < 0) PC += offset; if (PC < 0 || PC >= instructions.size()) exit(1); }
void br(int offset) { PC += offset; if (PC < 0 || PC >= instructions.size()) exit(1); }

using InstructionFunc = void (*)(int);
InstructionFunc executeInstruction[] = {
    ldc, adc, ldl, stl, ldnl, stnl, add, sub,
    shl, shr, adj, a2sp, sp2a, call, ret, brz, brlz, br
};

// Read object file into memory and instruction vector
void loadMachineCode(const string& fileName) {
    ifstream file(fileName, ios::binary);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << fileName << endl;
        exit(1);
    }

    unsigned int word;
    int pos = 0;
    while (file.read(reinterpret_cast<char*>(&word), sizeof(word))) {
        Memory[pos++] = word;
        instructions.push_back(word);
    }
    file.close();
}

// Print memory as hex
void memoryDump() {
    for (int i = 0; i < instructions.size(); i += 4) {
        cout << intToHex(i) << " ";
        for (int j = i; j < min(i + 4, (int)instructions.size()); ++j) {
            cout << intToHex(Memory[j]) << " ";
        }
        cout << endl;
    }
}

void displayRegisters() {
    cout << "A: " << intToHex(A)
         << "  B: " << intToHex(B)
         << "  SP: " << intToHex(SP)
         << "  PC: " << intToHex(PC + 1) << endl;
}

void showReadOperation() {
    cout << "Read from memory[" << intToHex(memChange[0])
         << "] = " << intToHex(Memory[memChange[0]]) << endl;
}

void showWriteOperation() {
    cout << "Write to memory[" << intToHex(memChange[0])
         << "] = " << intToHex(Memory[memChange[0]]) << endl;
}

void listInstructions() {
    cout << "Opcode    Mnemonic\n";
    for (int i = 0; i < mnemonics.size(); ++i) {
        cout << i << "   " << mnemonics[i] << '\n';
    }
}
void displayInstructions() {
    cout << "\nEmulator Commands:\n"
         << "  -run      Run the program until HALT\n"
         << "  -t        Execute one instruction (step)\n"
         << "  -dump     Dump memory\n"
         << "  -reg      Show register values\n"
         << "  -isa      List instruction set\n"
         << "  -read     Run and print all memory reads\n"
         << "  -write    Run and print all memory writes\n"
         << "  0         Exit (only in interactive mode)\n";
}

// Main executor loop
bool executeCode(int mode, int limit = (1 << 25)) {
    while (limit-- && PC < instructions.size()) {
        execCount++;
        if (execCount > 3e7) {
            cout << "Segmentation Fault: Instruction limit exceeded.\n";
            return false;
        }

        uint32_t raw = instructions[PC];
        int opcode = raw & 0xFF;
        int operand = (raw >> 8) & 0xFFFFFF;
        if (operand & (1 << 23)) operand -= (1 << 24);  // sign-extend

        if (opcode == 18) {
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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <file.o> [command]" << endl;
        return 1;
    }

    string fileName = argv[1];
    loadMachineCode(fileName);

    if (argc == 3) {
        string command = argv[2];

        if (command == "-dump") {
            cout << "\nInitial Memory Dump:\n";
            memoryDump();
        }
        else if (command == "-reg") {
            displayRegisters();
        }
        else if (command == "-t") {
            executeCode(0, 1);
        }
        else if (command == "-run") {
            cout << "\nInitial Memory Dump:\n";
            memoryDump();

            cout << "\n--- Executing Program ---\n";
            executeCode(0);

            cout << "\nFinal Memory Dump:\n";
            memoryDump();

            cout << "\nFinal Register State:\n";
            displayRegisters();
        }
        else if (command == "-isa") {
            listInstructions();
        }
        else if (command == "-read") {
            executeCode(1);
        }
        else if (command == "-write") {
            executeCode(2);
        }
        else {
            cerr << "Invalid command: " << command << endl;
            return 1;
        }
    } else {
        // Interactive REPL
        displayInstructions();
        while (true) {
            cout << "Enter command or 0 to exit:\n> ";
            string cmd;
            cin >> cmd;

            if (cmd == "0") break;
            else if (cmd == "-dump") memoryDump();
            else if (cmd == "-reg") displayRegisters();
            else if (cmd == "-t") executeCode(0, 1);
            else if (cmd == "-run") {
                cout << "\nInitial Memory Dump:\n";
                memoryDump();

                cout << "\n--- Executing Program ---\n";
                executeCode(0);

                cout << "\nFinal Memory Dump:\n";
                memoryDump();

                cout << "\nFinal Register State:\n";
                displayRegisters();
            }
            else if (cmd == "-isa") listInstructions();
            else if (cmd == "-read") executeCode(1);
            else if (cmd == "-write") executeCode(2);
            else cout << "Invalid command. Try again.\n";
        }
    }

    return 0;
}



