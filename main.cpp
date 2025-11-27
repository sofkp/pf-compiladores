#include <iostream>
#include <fstream>
#include <string>
#if defined(_WIN32)
#include <direct.h>
#include <errno.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#define MKDIR(path) mkdir(path, 0755)
#endif
#include "scanner.h"
#include "parser.h"
#include "ast.h"
#include "visitor.h"

using namespace std;

int main(int argc, const char* argv[]) {
    std::cerr << "[DBG] main start\n" << std::flush;

    if (argc != 2) {
        cout << "Uso: " << argv[0] << " <archivo_de_entrada>\n";
        return 1;
    }

    string inputPath = argv[1];

    cout << "[DEBUG] inputPath='" << inputPath << "'\n";

    // Crear outputs si no existe 
    if (MKDIR("outputs") != 0) {
        if (errno != EEXIST) {
            cerr << "No se pudo crear el directorio outputs" << endl;
            return 1;
        }
    }
    cout << "[DEBUG] outputs dir ensured\n";

    // Leer archivo 
    ifstream infile(inputPath);
    if (!infile.is_open()) {
        cerr << "No se pudo abrir: " << inputPath << endl;
        return 1;
    }
    cout << "[DEBUG] input file opened OK\n";

    string input, line;
    while (getline(infile, line))
        input += line + "\n";
    infile.close();

    // BaseName 
    string base = inputPath;
    size_t pos = base.find_last_of("/\\");
    if (pos != string::npos) base = base.substr(pos + 1);
    pos = base.find_last_of(".");
    if (pos != string::npos) base = base.substr(0, pos);

    Scanner scanner1(input.c_str());
    Scanner scanner2(input.c_str());

    ejecutar_scanner(&scanner1, inputPath);

    Parser parser(&scanner2);
    Program* ast = nullptr;

    try {
        ast = parser.parseProgram();
    } catch (const exception& e) {
        cerr << "Error al parsear: " << e.what() << endl;
        return 1;
    }

    //  GENCODE 
    {
        string asmFile = "outputs/" + base + ".s";
        ofstream aout(asmFile);
        if (!aout.is_open()) {
            cerr << "No se pudo crear el archivo ASM: " << asmFile << "\n";
            return 1;
        }

        try {
            GenCodeVisitor gc(aout);
            gc.generate(ast);
            cout << "ASM generado en: " << asmFile << "\n";
        } catch (const std::exception& e) {
            cerr << "ERROR en typecheck/codegen: " << e.what() << "\n";
            return 1;
        }
    }

    return 0;
}
