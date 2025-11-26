#ifndef PARSER_H       
#define PARSER_H

#include "scanner.h"
#include "ast.h"

class Parser {
private:
    Scanner* scanner;
    Token *current, *previous;
    bool match(Token::Type ttype);
    bool check(Token::Type ttype);
    bool advance();
    bool isAtEnd();
    TypeInfo* parseType();
public:
    Parser(Scanner* scanner);       
    Program* parseProgram();
    FunDec* parseFunDec();
    StructDec* parseStructDec();
    Body* parseBody();
    GlobalVarDec* parseGlobalVarDec();
    LocalVarDec* parseLocalVarDec();
    Stm* parseStm();
    Exp* parseCE();
    Exp* parseE();
    Exp* parseT();
    Exp* parseI();
    Exp* parseF();
    void debugToken(const std::string& where);
};

#endif // PARSER_H