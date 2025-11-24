#include<iostream>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

// =============================
// Métodos de la clase Parser
// =============================

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR) {
        throw runtime_error("Error léxico");
    }
}

bool Parser::match(Token::Type ttype) {
    if (check(ttype)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type ttype) {
    if (isAtEnd()) return false;
    return current->type == ttype;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* temp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = temp;

        if (check(Token::ERR)) {
            throw runtime_error("Error lexico");
        }
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return (current->type == Token::END);
}

Program* Parser::parseProgram() {
    Program* p = new Program();

    while (check(Token::STATIC)) {
        p->vdlist.push_back(parseGlobalVarDec());
    }

    while (check(Token::STRUCT)) {
        p->sdlist.push_back(parseStructDec());
    }

    while (check(Token::FN)) {
        p->fdlist.push_back(parseFunDec());
    }

    cout << "Parser exitoso" << endl;
    return p;
}

GlobalVarDec* Parser::parseGlobalVarDec() {
    bool mut;
    string name;
    string type;

    match(Token::STATIC);
    mut = match(Token::MUT);

    match(Token::ID);
    name = previous->text;

    match(Token::COLON);

    if(match(Token::I32)) type = "i32";
    else if(match(Token::I64)) type = "i64";
    else if(match(Token::BOOL)) type = "bool";
    else if (match(Token::ID)) type = previous->text; // allow struct types
    else throw runtime_error("Tipo inválido en variable global");

    match(Token::ASSIGN);
    Exp* value = parseCE();
    match(Token::SEMICOL);

    return new GlobalVarDec(type, mut, name, value);
}

LocalVarDec* Parser::parseLocalVarDec() {
    bool mut;
    string type;
    string name;

    match(Token::LET);
    mut = match(Token::MUT);

    match(Token::ID);
    name = previous->text;

    match(Token::COLON);

    if(match(Token::I32)) type = "i32";
    else if(match(Token::I64)) type = "i64";
    else if(match(Token::BOOL)) type = "bool";
    else if (match(Token::ID)) type = previous->text; // allow struct types
    else throw runtime_error("Tipo inválido en variable local");

    Exp* e = nullptr;
    if(match(Token::ASSIGN)){
        e = parseCE();
    }

    match(Token::SEMICOL);
    return new LocalVarDec(type, mut, name, e);
}

FunDec* Parser::parseFunDec() {
    match(Token::FN);

    match(Token::ID);
    string nombre = previous->text;

    match(Token::LPAREN);

    vector<string> Pnombres;
    vector<string> Ptipos;

    if (check(Token::ID)) {
        match(Token::ID);
        string pname = previous->text;

        match(Token::COLON);

        if (match(Token::I32)) Ptipos.push_back("i32");
        else if (match(Token::I64)) Ptipos.push_back("i64");
        else if (match(Token::BOOL)) Ptipos.push_back("bool");
        else if (match(Token::ID)) Ptipos.push_back(previous->text);
        else throw runtime_error("Tipo invalido");

        Pnombres.push_back(pname);

        while(match(Token::COMA)){
            match(Token::ID);
            pname = previous->text;

            match(Token::COLON);

            if (match(Token::I32)) Ptipos.push_back("i32");
            else if (match(Token::I64)) Ptipos.push_back("i64");
            else if (match(Token::BOOL)) Ptipos.push_back("bool");
            else if (match(Token::ID)) Ptipos.push_back(previous->text);
            else throw runtime_error("Tipo invalido");

            Pnombres.push_back(pname);
        }
    }

    match(Token::RPAREN);

    bool returns = false;
    string tipo;

    if(match(Token::ARROW)) {
        returns = true;

        if(match(Token::I32)) tipo = "i32";
        else if(match(Token::I64)) tipo = "i64";
        else if(match(Token::BOOL)) tipo = "bool";
        else if (match(Token::ID)) tipo = previous->text;
        else throw runtime_error("Tipo invalido");
    }

    Body* body = parseBody();

    FunDec* fd = new FunDec(nombre, body);
    fd->returns = returns;
    fd->tipo = tipo;
    fd->Pnombres = Pnombres;
    fd->Ptipos = Ptipos;

    return fd;
}

StructDec* Parser::parseStructDec() {
    match(Token::STRUCT);
    match(Token::ID);
    string name = previous->text;

    match(Token::LBRACE);
    StructDec* sd = new StructDec(name);

    if (!check(Token::RBRACE)) {
        // parse first field
        match(Token::ID);
        string fname = previous->text;
        match(Token::COLON);
        string ftype;
        if (match(Token::I32)) ftype = "i32";
        else if (match(Token::I64)) ftype = "i64";
        else if (match(Token::BOOL)) ftype = "bool";
        else if (match(Token::ID)) ftype = previous->text;
        else throw runtime_error("Tipo invalido en campo de struct");
        sd->fieldNames.push_back(fname);
        sd->fieldTypes.push_back(ftype);

        while (match(Token::COMA)) {
            match(Token::ID);
            fname = previous->text;
            match(Token::COLON);
            if (match(Token::I32)) ftype = "i32";
            else if (match(Token::I64)) ftype = "i64";
            else if (match(Token::BOOL)) ftype = "bool";
            else if (match(Token::ID)) ftype = previous->text;
            else throw runtime_error("Tipo invalido en campo de struct");
            sd->fieldNames.push_back(fname);
            sd->fieldTypes.push_back(ftype);
        }
    }

    match(Token::RBRACE);
    return sd;
}

Body* Parser::parseBody() {
    Body* b = new Body();

    match(Token::LBRACE);

    while(check(Token::LET)) {
        b->declarations.push_back(parseLocalVarDec());
    }

    while(!check(Token::RBRACE)) {
        b->StmList.push_back(parseStm());
        match(Token::SEMICOL);
    }

    match(Token::RBRACE);
    return b;
}


Stm* Parser::parseStm() {
    Exp* e;
    if(match(Token::ID)) {
        string name = previous->text;

        if (match(Token::LPAREN)) {
            FcallExp* f = new FcallExp(name);
            if (!check(Token::RPAREN)) {
                f->argumentos.push_back(parseCE());
                while (match(Token::COMA)) {
                    f->argumentos.push_back(parseCE());
                }
            }
            match(Token::RPAREN);
            return new FcallStm(f);
        }

        match(Token::ASSIGN);
        Exp* val = parseCE();
        return new AssignStm(name, val);
    }
    if (match(Token::PRINT)) {
        match(Token::LPAREN);
        e = parseCE();
        match(Token::RPAREN);
        return new PrintStm(e);
    }

    if (match(Token::RETURN)) {
        e = parseCE();
        return new ReturnStm(e);
    }

    if (match(Token::IF)) {
        Exp* condition = parseCE();
        Body* ifbody = parseBody();

        bool elsecond = false;
        Body* elsebody = nullptr;

        if(match(Token::ELSE)) {
            elsecond = true;
            elsebody = parseBody();
        }

        return new IfStm(condition, ifbody, elsecond, elsebody);
    }

    if(match(Token::WHILE)) {
        Exp* condition = parseCE();
        Body* body = parseBody();
        return new WhileStm(condition, body);
    }

    if(match(Token::FOR)) {
        match(Token::ID);
        string var = previous->text;

        match(Token::IN);

        Exp* start = parseCE();
        bool eq = false;

        if(match(Token::RG)) eq = false;
        else if(match(Token::REQ)) eq = true;
        else throw runtime_error(".. o ..=");

        Exp* finish = parseCE();
        Body* body = parseBody();

        return new ForStm(var, eq, start, finish, body);
    }

    throw runtime_error("Error sintáctico");
}

Exp* Parser::parseCE() {
    Exp* l = parseE();
    if (match(Token::LE)) {
        BinaryOp op = LE_OP;
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    else if (match(Token::LT)) {
        BinaryOp op = LT_OP;
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    else if (match(Token::GE)) {
        BinaryOp op = GE_OP;
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    else if (match(Token::GT)) {
        BinaryOp op = GT_OP;
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    else if (match(Token::EQ)) {
        BinaryOp op = EQ_OP;
        Exp* r = parseE();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseE() {
    Exp* l = parseT();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op;
        if (previous->type == Token::PLUS){
            op = PLUS_OP;
        }
        else{
            op = MINUS_OP;
        }
        Exp* r = parseT();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseT() {
    Exp* l = parseF();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op;
        if (previous->type == Token::MUL){
            op = MUL_OP;
        }
        else{
            op = DIV_OP;
        }
        Exp* r = parseF();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseF() {
    if(match(Token::NUM))
        return new NumberExp(stoi(previous->text));

    if(match(Token::STRING))
        return new StringExp(previous->text);

    if(match(Token::TRUE))
        return new NumberExp(1);

    if(match(Token::FALSE))
        return new NumberExp(0);

    if(match(Token::LPAREN)) {
        Exp* e = parseCE();
        match(Token::RPAREN);
        return e;
    }

    if(match(Token::ID)) {
        string nom = previous->text;

        if(check(Token::LPAREN)) {
            match(Token::LPAREN);

            FcallExp* fcall = new FcallExp(nom);

            if(!check(Token::RPAREN)) {
                fcall->argumentos.push_back(parseCE());
                while(match(Token::COMA)) {
                    fcall->argumentos.push_back(parseCE());
                }
            }

            match(Token::RPAREN);
            return fcall;
        }

        return new IdExp(nom);
    }

    throw runtime_error("Error sintáctico en factor");
}
