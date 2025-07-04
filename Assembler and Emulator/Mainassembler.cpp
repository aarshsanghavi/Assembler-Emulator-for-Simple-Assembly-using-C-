//Author - Aarsh Sanghavi                       Roll No. - 2301CS01

#include <bits/stdc++.h>
using namespace std;
#define pb push_back

// Data structure for storing label details

int program_count = 0;                  // Tracks the number of instructions processed
int current_line = 0;                   // Tracks the line currently being processed
int line_length;                        // Temporary variable to store line length
string line_content, aux_string;        // Auxiliary strings
map<string, pair<int,int>> instruction_set;  // Map for storing instruction details
map<int, string> error_descriptions;    // Map for storing error messages
map<string, int> label_positions;       // Map for storing label positions
vector<string> source_code;             // Stores original code from file
vector<string> error_messages;          // Stores error messages for output
vector<string> listing_col1(1000);      // First column for listing file
vector<string> listing_col2(1000);      // Second column for listing file
vector<string> label_operand;
vector<int> label_operand_pos;          // Tracks the current_line for a operand which is a label
vector<int> label_opcode;               // Tracks the opcode of label (which is the program count of that label in most cases)
vector<int> label_operand_type;         
vector<int> label_operand_program_count;// Tracks the program count for a operand which is a label
int number_type;                        // Helper to track number format
int max_hex_value = 16777215;           // Maximum value for hex (24 bits)
unsigned long long max_hex_value_8bit = 4294967295;
int error_flags[100005];
string current_label="@temp";
string last_label="@temp";
int label_flag=0;
string templabel;

void initialize() {
    instruction_set["data"]   = {-1, 1};
    instruction_set["SET"]    = {-2, 1};
    instruction_set["ldc"]    = {0, 1};
    instruction_set["adc"]    = {1, 1};
    instruction_set["ldl"]    = {2, 2};
    instruction_set["stl"]    = {3, 2};
    instruction_set["ldnl"]   = {4, 2};
    instruction_set["stnl"]   = {5, 2};
    instruction_set["add"]    = {6, 0};
    instruction_set["sub"]    = {7, 0};
    instruction_set["shl"]    = {8, 0};
    instruction_set["shr"]    = {9, 0};
    instruction_set["adj"]    = {10, 1};
    instruction_set["a2sp"]   = {11, 0};
    instruction_set["sp2a"]   = {12, 0};
    instruction_set["call"]   = {13, 2};
    instruction_set["return"] = {14, 0};
    instruction_set["brz"]    = {15, 2};
    instruction_set["brlz"]   = {16, 2};
    instruction_set["br"]     = {17, 2};
    instruction_set["HALT"]   = {18, 0};
    
    error_descriptions[0] = "Invalid file format";
    error_descriptions[1] = "Extraneous data at line end";
    error_descriptions[2] = "Invalid label name ";
    error_descriptions[3] = "Duplicate label definition";
    error_descriptions[4] = "Unrecognized mnemonic";
    error_descriptions[5] = "Invalid operand";
}

string trimSpaces(string temp) {
    stringstream ss(temp);
    vector<string> tokens;
    while (ss >> aux_string)
        tokens.pb(aux_string);
    
    string result;
    for (int i = 0; i < tokens.size(); ++i) {
        result += tokens[i];
        if (i != tokens.size() - 1)
            result += " ";
    }
    return result;
}

//This function helps seperating all words from line and stores them in tokens.
vector<string> tokenize(const string &str) {
    vector<string> tokens;
    stringstream ss(str);
    string word;
    while (ss >> word) {
        tokens.push_back(word);
    }
    return tokens;
}

string stripComments(string line) {  
    string result = line;
    for (int i = 0; i < line_length; i++) {
        if (line[i] == ';') {
            result = line.substr(0, i);
            break;
        }
    }
    return result;
}

//Stores all the errors in the vector error_messages
void logError(int code, int line_num) {
    error_flags[line_num] = 1;
    string error_message = error_descriptions[code];
    error_message += " at line number ";
    error_message += to_string(line_num + 1);
    error_messages.pb(error_message);
}

//Checks if a label is valid or not
bool validLabel(string label) {
    for (int i = 0; i < label.size(); i++) {
        if (label[i] == '_')
            continue;
        if (i == 0) {
            if (isalpha(label[i]))
                continue;
            return false;
        } else {
            if (isalnum(label[i]) || label[i] == '_')
                continue;
            return false;
        }
    }
    return true;
}

// Converts a integer value to hexadecimal form of 8 bits
string intToHex(int num, int width=8 ) {
    std::stringstream ss;
    ss << std::uppercase << std::hex << std::setw(width) << std::setfill('0') 
       << static_cast<uint32_t>(num); // Convert to uint32_t for proper two's complement representation
    return ss.str().substr(0, width); // Limit to 8 characters
}

//The function below is to convert 8 bit hex into 6 bit hex
string intTo24BitHex(int num,int w=6) {
    
    int maskedNum = num & 0xFFFFFF; // Masking ensures a 24-bit result
    std::stringstream ss;
    ss << std::uppercase << std::hex << std::setw(6) << std::setfill('0') << maskedNum;
    return ss.str();
}


bool isValidDecimal(string num) {
    for (char ch : num) {
        if (!isdigit(ch))
            return false;
    }
    return true;
}

bool isValidOctal(string num) {
    for (char ch : num) {
        if (ch < '0' || ch > '7')
            return false;
    }
    return true;
}

bool isValidHex(string num) {
    for (char ch : num) {
        if (!isxdigit(ch))
            return false;
    }
    return true;
}

// this function validates if a operand value is correct or not. It assigns number_type to the operand.
bool validateValue(const string& value) {
    if (value.empty()) return false;

    int idx = 0;
    if (value[0] == '-' || value[0] == '+') {
        idx++;
        if (value.size() == 1) return false;
    }

    // Check if it's a valid hexadecimal, octal, or decimal number
    if (isdigit(value[idx]) || (value.size() > 1 && value[0] == '0' && 
       (value[1] == 'x' || value[1] == 'X'))) {
        
        // Hexadecimal check
        if (value.size() > idx + 2 && (value[idx] == '0' && 
           (value[idx + 1] == 'x' || value[idx + 1] == 'X'))) {
            if (isValidHex(value.substr(idx + 2))) {
                number_type = 2; // Hexadecimal
                return true;
            }
        } 
        // Octal check
        else if (value.size() > idx && value[idx] == '0') {
            if (isValidOctal(value.substr(idx))) {
                number_type = 1; // Octal
                return true;
            }
        } 
        // Decimal check
        else {
            if (isValidDecimal(value.substr(idx))) {
                number_type = 0; // Decimal
                return true;
            }
        }
        return false; // Invalid number format
    }

    // If it's not a number, check if it’s a valid label name (alphanumeric and underscores)
    number_type = -1; // Reset number_type for label names
    return validLabel(value);
}

bool isMnemonic(string mnemonic) {
    return instruction_set.find(mnemonic) != instruction_set.end();
}

int convertToInt(const string& value, int base) 
{
    bool isNegative = (value[0] == '-');
    int startIdx = (isNegative || value[0] == '+') ? 1 : 0;

    // Adjust start index to skip "0x" or "0X" prefix for hexadecimal values
    if (base == 16 && value.size() > startIdx + 1 && value[startIdx] == '0' &&
        (value[startIdx + 1] == 'x' || value[startIdx + 1] == 'X')) {
        startIdx += 2;
    }

    int result = 0;
    for (int i = startIdx; i < value.size(); ++i) {
        int curr = (isdigit(value[i])) ? (value[i] - '0') : (toupper(value[i]) - 'A' + 10);
        result = result * base + curr;
    }

    return isNegative ? -result : result;
}

//This is the core processor of my assembler. It processes all the instruction like finding out errors and creating the listing_columns for the lst file
void processInstruction(vector<string> &tokens) {
    string mnemonic = tokens[0];
    if (isMnemonic(mnemonic)) {
        int opcode = instruction_set[mnemonic].first;
        int operand_type = instruction_set[mnemonic].second;
        // Case for instructions with no operand required
        if (operand_type == 0) {
            if (tokens.size() > 1) {  // If there's an unexpected operand, log an error
                logError(1, current_line);
            } else {
                listing_col2[current_line] = intTo24BitHex(0, 6) + intToHex(opcode, 2);  // No operand part, only opcode
            }
            return;  // Skip further processing for no-operand instructions
        }

        // Handle instructions that do require an operand
        string operand = tokens.size() == 2 ? tokens[1] : "";

        if (mnemonic == "SET") {
            if (validateValue(operand)) {
                int setValue = convertToInt(operand, number_type == 2 ? 16 : number_type == 1 ? 8 : 10);
                if(label_flag==1)
                label_positions[templabel] = setValue;  // Assign value to label
                listing_col2[current_line] = intToHex(setValue, 8);
            } else {
                logError(5, current_line);
            }
        } 
        else if (mnemonic == "data") {
            if (validateValue(operand)) { 
                // Convert the operand to an integer based on the detected number type (hex, octal, decimal)
                int dataValue = convertToInt(operand, number_type == 2 ? 16 : number_type == 1 ? 8 : 10);
                
                // Store the 8-digit hex representation in listing_col2
                listing_col2[current_line] = intToHex(dataValue, 8);
            } else {
                logError(5, current_line); // Log an invalid operand error if `data` operand is not valid
            }
        }
        else if (operand_type == 1) {
                if (validateValue(operand)) {
                int operandValue;
                //************************************************************/
                if (number_type==-1)
                {
                    if(label_positions.count(operand))
                    {
                        listing_col2[current_line]= intTo24BitHex(label_positions[operand], 6) + intToHex(opcode, 2);
                        return;
                    }
                    listing_col2[current_line]=-1;
                    label_operand.pb(operand);
                    label_operand_type.pb(1);
                    label_operand_program_count.pb(program_count);
                    label_operand_pos.pb(current_line);
                    label_opcode.pb(opcode);
                    return;
                }
                 else {
                    // If operand is a numeric literal, convert it to an integer based on the specified base
                    if (number_type == 2) {
                        operandValue = convertToInt(operand, 16);  // Hexadecimal
                    } else if (number_type == 1) {
                        operandValue = convertToInt(operand, 8);   // Octal
                    } else {
                        operandValue = convertToInt(operand, 10);  // Decimal
                    }
                }

                // Format operandValue and opcode into an 8-character hex string for the listing file
                listing_col2[current_line] = intTo24BitHex(operandValue, 6) + intToHex(opcode, 2);
            }
            else
            {
                logError(5, current_line);
            }

        } 
        else if (operand_type == 2) {
            // Check if the operand is a valid number
            validateValue(operand);
            if(number_type==-1)
            {
                if(label_positions.count(operand))
                    {
                        int offset = label_positions[operand] - program_count -1 ;
                        listing_col2[current_line]= intTo24BitHex(offset, 6) + intToHex(opcode, 2);
                        return;
                    }
                listing_col2[current_line]=-1;
                label_operand.pb(operand);
                label_operand_type.pb(2);
                label_operand_program_count.pb(program_count);
                label_operand_pos.pb(current_line);
                label_opcode.pb(opcode);
                return;
            }
            else
            {
                // If the operand is a number, convert it to an integer and use it directly
                int offset = convertToInt(operand, number_type == 2 ? 16 : number_type == 1 ? 8 : 10);
                listing_col2[current_line] = intTo24BitHex(offset, 6) + intToHex(opcode, 2);
            }
            
        }
         else {
            logError(5, current_line);
        }
    } 
    else 
    {
        logError(4, current_line);
    }
}



void FirstPass(char** argv) {
    string file = argv[1];
    ifstream input(file);
    
    while (getline(input, line_content)) {
        label_flag=0;
        source_code.pb(line_content);
        line_length = line_content.size();
        
        line_content = trimSpaces(stripComments(line_content));
        if (line_content.empty()) {
            listing_col1[current_line] = "        ";
            listing_col2[current_line] = "        ";
            current_line++;
            continue;
        }
        
        int colon_pos = line_content.find(':');
        if (colon_pos != string::npos) {
            string label = line_content.substr(0, colon_pos);
            templabel = label;
            label_flag=1;
            if (!validLabel(label)) {
                logError(2, current_line);
            } else {
                if (label_positions.find(label) != label_positions.end()) {
                    logError(3, current_line);
                } else {
                    label_positions[label] = program_count;
                }
            }
            vector<string> tokens = tokenize(line_content.substr(colon_pos + 1));
            if (tokens.empty()) {
                current_line++;
                continue;
            }
            listing_col1[current_line] = intToHex(program_count, 8); // Store address
            processInstruction(tokens);
            program_count++; // Increment program count after each instruction
        } else {
            vector<string> tokens = tokenize(line_content);
            if (tokens.empty()) {
                current_line++;
                continue;
            }
            listing_col1[current_line] = intToHex(program_count, 8); // Store address
            processInstruction(tokens);
            program_count++; // Increment program count after each instruction
        }
        current_line++;
    }
    for(int i=0;i<label_operand.size(); i++)
    {
        if(label_operand_type[i]==1)
        {
        if(label_positions.count(label_operand[i]))
        listing_col2[label_operand_pos[i]]= intTo24BitHex(label_positions[label_operand[i]], 6) + intToHex(label_opcode[i], 2);
        else
        logError(5,label_operand_pos[i]);
        }
        else
        {
            if(label_positions.count(label_operand[i]))
            {
            int offset = label_positions[label_operand[i]] -label_operand_program_count[i] - 1;
            listing_col2[label_operand_pos[i]] = intTo24BitHex(offset, 6) + intToHex(label_opcode[i], 2);
            }
            else
            logError(5,label_operand_pos[i]);

        }
        
    }
}


void writeLogFile(const string &fname) {
    string log_file = fname + ".log";
    ofstream log_out(log_file);
    if (error_messages.empty()) {
        log_out << "Compiled without any error" << endl;
    } else {
        for (const auto& error : error_messages) {
            log_out << "ERROR: " << error << endl;
        }
    }
    log_out.close();
}

void writeListFile(const string &fname) {
    string list_file = fname + ".lst";
    ofstream list_out(list_file);
    
    for (int i = 0; i < current_line; i++) {
        if (error_flags[i]) continue;
        
        string out = listing_col1[i] + " " + listing_col2[i] + " " + source_code[i];
        list_out << out << endl;
    }
    list_out.close();
}


void writeToFile(const string &fname) {
    string obj_file = fname + ".o";
    ofstream obj_out(obj_file, ios::binary);
    unsigned int machine_code[current_line];
    int idx = 0;
    for (int i = 0; i < current_line; i++) {
        if (!listing_col2[i].empty() && listing_col2[i][0] != ' ') {
            machine_code[idx++] = convertToInt(listing_col2[i], 16);
        }
    }

    obj_out.write(reinterpret_cast<const char*>(machine_code), idx * sizeof(int));
    obj_out.close();
}

void SecondPass(char** argv) {
   
    string file = argv[1];
    int length = file.size();
    string fname = file.substr(0, length - 4);  // Remove .asm extension

    writeLogFile(fname);
    writeListFile(fname);

    // Write object file only if there were no errors
    if (error_messages.empty()) {
        writeToFile(fname);
    }
}


int main(int argc, char** argv) {
    
    initialize();
    if (argc < 2) {
        cout << "Error -- Input file not provided\n";
        return 0;
    }
    
    FirstPass(argv);  
    SecondPass(argv);    
    if(error_messages.empty())
    {
        cout<<"Compiled without any error"<<endl;
    }  
    for(auto it:error_messages)
    {
        cout<<"ERROR: "<<it<<endl;
    }      
    
    return 0;
}
//***************End*******************//