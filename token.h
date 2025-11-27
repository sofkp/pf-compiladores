#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <ostream>

using namespace std;

class Token {
public:
    // Tipos de token
    enum Type {
        PLUS,
        MINUS,
        MUL,
        DIV,
        LT,
        LE, //<=
        EQ, //==
        GT, //>
        GE, //>=
        LPAREN, // (
        RPAREN, // )
        NUM,
        ERR, // Error
        ID,
        TRUE,
        FALSE,
        LET,
        MUT,
        ASSIGN, //=
        COMA, //,
        COLON, //:
        SEMICOL, //;
        IF,
        ELSE,
        FOR,
        RG,
        REQ,
        IN,
        LBRACE,
        RBRACE,
        WHILE,
        FN,
        ARROW,
        I32,
        I64,
        BOOL,
        STATIC,
        RETURN,
        PRINT,
        END,
        LBRACKET,
        RBRACKET,
        STRING,
        STRINGTYPE,
        STRUCT,
        DOT
    };

    Type type;
    string text;

    Token(Type type);
    Token(Type type, char c);
    Token(Type type, const string& source, int first, int last);

    friend ostream& operator<<(ostream& outs, const Token& tok);
    friend ostream& operator<<(ostream& outs, const Token* tok);
};

#endif // TOKEN_H