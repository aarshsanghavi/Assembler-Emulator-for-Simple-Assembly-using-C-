#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <sstream>
#include <iomanip>
using namespace std;

constexpr int MEMORY_SIZE = 1 << 24;
int Memory[MEMORY_SIZE];  // Main memory
vector<uint32_t> instructions;

int A = 0, B = 0, PC = 0, SP = 0, execCount = 0;
vector<int> readLog;  // <- NEW: store all read addresses
array<int, 2> memChange;
bool execFlag = false;

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

void incrementPC() {
    PC++;
    if (PC >= instructions.size()) {
        cout << "PC out of bounds - terminating execution.\n";
        exit(1);
    }
}

// Instructions
void ldc(int value) { B = A; A = value; }
void adc(int value) { A += value; }
void ldl(int offset) {
    B = A;
    int addr = SP + offset;
    A = Memory[addr];
    readLog.push_back(addr);  // <- LOGGING READ
    execFlag = true;
}
void stl(int offset) {
    int addr = SP + offset;
    memChange = {addr, Memory[addr]};
    Memory[addr] = A;
    execFlag = true;
    A = B;
}
void ldnl(int offset) {
    int addr = A + offset;
    A = Memory[addr];
    readLog.push_back(addr);  // <- LOGGING READ
    execFlag = true;
}
void stnl(int offset) {
    int addr = A + offset;
    memChange = {addr, Memory[addr]};
    Memory[addr] = B;
    execFlag = true;
}
void add(int) { A += B; }
void sub(int) { A = B - A; }
void shl(int) { A = B << A; }
void shr(int) { A = B >> A; }
void adj(int value) { SP += value; }
void a2sp(int) { SP = A; A = B; }
void sp2a(int) { B = A; A = SP; }
void call(int offset) {
    B = A;
    A = PC;
    PC += offset;
    if (PC < 0 || PC >= instructions.size()) exit(1);
}
void ret(int) { PC = A; A = B; }
void brz(int offset) {
    if (A == 0) PC += offset;
    if (PC < 0 || PC >= instructions.size()) exit(1);
}
void brlz(int offset) {
    if (A < 0) PC += offset;
    if (PC < 0 || PC >= instructions.size()) exit(1);
}
void br(int offset) {
    PC += offset;
    if (PC < 0 || PC >= instructions.size()) exit(1);
}

using InstructionFunc = void (*)(int);
InstructionFunc executeInstruction[] = {
    ldc, adc, ldl, stl, ldnl, stnl, add, sub,
    shl, shr, adj, a2sp, sp2a, call, ret, brz, brlz, br
};

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

void showReadLog() {
    for (int addr : readLog) {
        cout << "Read from memory[" << intToHex(addr)
             << "] = " << intToHex(Memory[addr]) << endl;
    }
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

bool executeCode(int mode, int limit = (1 << 25)) {
    readLog.clear();  // <- RESET
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
            break;
        }

        executeInstruction[opcode](operand);

        if (mode == 2 && execFlag) showWriteOperation();
        execFlag = false;
        incrementPC();
    }

    if (mode == 1) showReadLog();  // print all reads at once
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
            executeCode(1);  // show all reads
        }
        else if (command == "-write") {
            executeCode(2);  // show writes
        }
        else {
            cerr << "Invalid command: " << command << endl;
            return 1;
        }
    } else {
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
