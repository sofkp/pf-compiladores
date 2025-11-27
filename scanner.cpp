#include <iostream>
#include <cstring>
#include <fstream>
#include "token.h"
#include "scanner.h"

using namespace std;


// Constructor
Scanner::Scanner(const char* s): input(s), first(0), current(0) { 
}

// auxiliar
bool is_white_space(char c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

Token* Scanner::nextToken() {
    Token* token;

    while (current < input.length() && is_white_space(input[current])) current++;

    if (current >= input.length()) return new Token(Token::END);

    char c = input[current];

    first = current;

    // nums
    if (isdigit(c)) {
        current++;
        while (current < input.length() && isdigit(input[current])) current++;
        token = new Token(Token::NUM, input, first, current - first);
    }

    else if (c == '&') {
        if (current + 3 < input.length() && input[current+1] == 's' && input[current+2] == 't' && input[current+3] == 'r') {
            current += 4;
            return new Token(Token::STRINGTYPE, input, first, current - first);
        }
    }

    // ID
    else if (isalpha(c)) {
        current++;
        while (current < input.length() && isalnum(input[current])) current++;
        
        if (current < input.length() && input[current] == '!') {
            current++;
            string lexema = input.substr(first, current - first);
            if (lexema == "println!") return new Token(Token::PRINT, input, first, current - first);
        }
        string lexema = input.substr(first, current - first);
        if (lexema=="if") return new Token(Token::IF, input, first, current - first);
        else if (lexema=="else") return new Token(Token::ELSE, input, first, current - first);
        else if (lexema=="while") return new Token(Token::WHILE, input, first, current - first);
        else if (lexema=="true") return new Token(Token::TRUE, input, first, current - first);
        else if (lexema=="false") return new Token(Token::FALSE, input, first, current - first);
        else if (lexema=="let") return new Token(Token::LET, input, first, current - first);
        else if (lexema=="mut") return new Token(Token::MUT, input, first, current - first);
        else if (lexema=="for") return new Token(Token::FOR, input, first, current - first);
        else if (lexema=="in") return new Token(Token::IN, input, first, current - first);
        else if (lexema=="fn") return new Token(Token::FN, input, first, current - first);
        else if (lexema=="i32") return new Token(Token::I32, input, first, current - first);
        else if (lexema=="i64") return new Token(Token::I64, input, first, current - first);
        else if (lexema=="bool") return new Token(Token::BOOL, input, first, current - first);
        else if (lexema=="static") return new Token(Token::STATIC, input, first, current - first);
        else if (lexema=="return") return new Token(Token::RETURN, input, first, current - first);
        else if (lexema=="struct") return new Token(Token::STRUCT, input, first, current - first);
        else return new Token(Token::ID, input, first, current - first);
    }
    // Operadores
    else if (c=='+' || c=='-' || c=='*' || c=='/' || c=='(' || c==')' || c=='{' || c=='}' || c==';' || c==',' || c==':' || c=='<' ||c=='>' || c=='=' || c=='.' ||  c=='[' ||
            c==']' || c=='"' || c=='.') {
        switch (c) {
            case '<':
            if (current + 1 < input.length() && input[current+1]=='='){
                current++;
                token = new Token(Token::LE, input, first, current + 1 - first);
            }
            else {token = new Token(Token::LT, c);}
            break;
            case '>': 
            if (current + 1 < input.length() && input[current+1]=='='){
                current++;
                token = new Token(Token::GE, input, first, current + 1 - first);
            }
            else {token = new Token(Token::GT, c);} 
            break;
            case '+': token = new Token(Token::PLUS,  c); break;
            case '-': 
            if (current + 1 < input.length() && input[current+1]=='>'){
                current++;
                token = new Token(Token::ARROW, input, first, current + 1 - first);
            }
            else{token = new Token(Token::MINUS, c);} 
            break;
            case '*': token = new Token(Token::MUL,   c); break;
            case '/': token = new Token(Token::DIV,   c); break;
            case '(': token = new Token(Token::LPAREN,c); break;
            case ')': token = new Token(Token::RPAREN,c); break;
            case '{': token = new Token(Token::LBRACE,c); break;
            case '}': token = new Token(Token::RBRACE,c); break;
            case '=': 
            if (current + 1 < input.length() && input[current+1]=='='){
                current++;
                token = new Token(Token::EQ, input, first, current + 1 - first);
            }
            else{token = new Token(Token::ASSIGN, c);} 
            break;
            case ';': token = new Token(Token::SEMICOL,c); break;
            case ',': token = new Token(Token::COMA,c); break;
            case ':': token = new Token(Token::COLON,c); break;
            case '.': 
            if (current + 1 < input.length() && input[current+1]=='.'){
                current++;
                if (current + 1 < input.length() && input[current+1]=='='){
                    current++;
                    token = new Token(Token::REQ, input, first, current + 1 - first);
                }
                else token = new Token(Token::RG, input, first, current + 1 - first);
            }else {token = new Token(Token::DOT, '.');}
            break;
            case '"': {
                current++;
                while (current < input.length() && input[current] != '"') {current++;}
                current++; 
                return new Token(Token::STRING, input, first+1, current - first-2);
            } break;
            case '[': token = new Token(Token::LBRACKET,c); break;
            case ']': token = new Token(Token::RBRACKET,c); break;
        }
        current++;
    }

    // Carácter inválido
    else {
        token = new Token(Token::ERR, c);
        current++;
    }

    return token;
}





// Destructor

Scanner::~Scanner() { }


// Función de prueba


int ejecutar_scanner(Scanner* scanner, const string& InputFile) {
    Token* tok;

    // Crear carpeta de salida
    string outDir = "outputs/";

    // Extraer el número del input (input3 → 3)
    string base = InputFile;
    size_t slash = base.find_last_of("/\\");
    if (slash != string::npos) base = base.substr(slash + 1);
    size_t dot = base.find_last_of(".");
    if (dot != string::npos) base = base.substr(0, dot);

    // Nombre de salida esperado: outputs/<base>_tokens.txt
    string OutputFileName = outDir + base + "_tokens.txt";

    ofstream outFile(OutputFileName);
    if (!outFile.is_open()) {
        cerr << "Error: no se pudo abrir el archivo " << OutputFileName << endl;
        return 0;
    }

    outFile << "Scanner\n\n";

    while (true) {
        tok = scanner->nextToken();

        if (tok->type == Token::END) {
            outFile << *tok << "\n\nScanner exitoso\n";
            delete tok;
            outFile.close();
            return 0;
        }

        if (tok->type == Token::ERR) {
            outFile << *tok << endl;
            delete tok;
            outFile << "Caracter invalido\n\nScanner no exitoso\n";
            outFile.close();
            return 0;
        }

        outFile << *tok << endl;
        delete tok;
    }
}
