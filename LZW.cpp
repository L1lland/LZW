#include <iostream>
#include <sstream>
#include <fstream>
#include <bitset>
#include <string>
#include <unordered_map>
#include <utility>

using namespace std;
constexpr size_t MAX_DEF = 4096;

string convert_int_to_bin(int number)
{
    string result = bitset<12>(number).to_string();
    return result;
}

void compress(string input, int size, string filename) {
    unordered_map<string, int> compress_dictionary(MAX_DEF);
    //Инициализация словаря ASCII кодом
    for (int unsigned i = 0; i < 256; i++) {
        compress_dictionary[string(1, i)] = i;
    }
    string current_string;
    unsigned int code;
    unsigned int next_code = 256;
    //Выходной файл для сжатой информации
    ofstream outputFile;
    outputFile.open(filename + ".lzw");

    for (const char& c: input) {
        current_string = current_string + c;
        if (compress_dictionary.find(current_string) == compress_dictionary.end()) {
            if (next_code <= MAX_DEF)
                compress_dictionary.insert(make_pair(current_string, next_code++));
            current_string.erase(current_string.size() - 1);
            outputFile << convert_int_to_bin(compress_dictionary[current_string]);
            current_string = c;
        }
    }
    if (current_string.size())
        outputFile << convert_int_to_bin(compress_dictionary[current_string]);
    outputFile.close();
}



void decompress(string input, int size, string filename) {
    unordered_map<unsigned int, string> dictionary(MAX_DEF);
    //Инициализация словаря ASCII кодом
    for (int unsigned i = 0; i < 256; i++) {
        dictionary[i] = string(1, i);
    }
    string previous_string;
    unsigned int code;
    unsigned int next_code = 256;
    //Выходной файл для распакованной информации
    ofstream outputFile;
    outputFile.open(filename + "_uncompressed.txt");

    int i = 0;
    while (i < size) {
        //Ковертация бинарного кода в десятичный
        string subinput = input.substr(i, 12);
        bitset<12> binary(subinput);
        code = binary.to_ullong();
        i += 12;

        if (dictionary.find(code) == dictionary.end())
            dictionary.insert(make_pair(code, (previous_string + previous_string.substr(0, 1))));
        outputFile << dictionary[code];
        if (previous_string.size())
            dictionary.insert(make_pair(next_code++, previous_string + dictionary[code][0]));
        previous_string = dictionary[code];
    }
    outputFile.close();
}

string convert_char_to_string(const char* pCh, int arraySize) {
    string str;
    if (pCh[arraySize - 1] == '\0') str.append(pCh);
    else for (int i = 0; i < arraySize; i++) str.append(1, pCh[i]);
    return str;
}

static void show_usage()
{
    cerr << "Usage: \n"
        << "Specify the file that needs to be compressed or decompressed\n"
        << "lzw -c #input   #compress\n"
        << "lzw -d #input   #decompress\n"
        << "Compressed data will be found in a file with the same name but with a .lzw extension\n"
        << "Decompressed data can be found in a file with the same name and a _uncompressed.txt extension\n"
        << endl;
}


int main(int argc, char* argv[]) {
    streampos size;
    char* memblock;

    if (argc < 2)
    {
        show_usage();
        return 0;
    }
    ifstream file(argv[2], ios::in | ios::binary | ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        memblock = new char[size];
        file.seekg(0, ios::beg);
        file.read(memblock, size);
        file.close();
        string input = convert_char_to_string(memblock, size);
        if (string("-c") == argv[1])
            compress(input, size, argv[2]);
        else if (string("-d") == argv[1])
            decompress(input, size, argv[2]);
        else
            show_usage();
    }
    else {
        cout << "Unable to open file." << endl;
        show_usage();
    }
}
