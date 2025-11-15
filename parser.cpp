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

bool Parser::next(Token::Type ttype) {
    if (!isAtEnd()) {
        Token* temp = scanner->nextToken();
        if (temp->type == ttype) {
            return true
        }
        return false;
    }
    return false;
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


// =============================
// Reglas gramaticales
// =============================

Program* Parser::parseProgram() {
    Program* p = new Program();
    if(check(Token::STATIC)) {
        p->vdlist.push_back(parseGlobalVarDec());
        while(check(Token::STATIC)) {
            p->vdlist.push_back(parseGlobalVarDec());
        }
    }
    if(check(Token::FN)) {
        p->fdlist.push_back(parseFunDec());
        while(check(Token::FN)){
                p->fdlist.push_back(parseFunDec());
            }
        }
    cout << "Parser exitoso" << endl;
    return p;
}

GlobalVarDec* Parser::parseGlobalVarDec(){
    GlobalVarDec* vd = new GlobalVarDec();
    match(Token::STATIC);
    if(match(Token::MUT)){
        vd->mut = true;
    }else{
        vd->mut = false;
    }
    match(Token::ID);
    vd->name = previous->text;
    match(Token::COLON);

    if(match(Token::I32) || match(Token::I64) || match(Token::I64)){
        vd->type = previous->text;
    }

    match(Token::ASSIGN);
    vd->value = parseCE();
    match(Token::SEMICOL);
    return vd;
}

LocalVarDec* Parser::parseLocalVarDec(){
    LocalVarDec* vd = new LocalVarDec();
    match(Token::LET);
    if(match(Token::MUT)){
        vd->mut = true;
    }else{
        vd->mut = false;
    }
    match(Token::ID);
    vd->name = previous->text;
    match(Token::COLON);

    if(match(Token::I32) || match(Token::I64) || match(Token::I64)){
        vd->type = previous->text;
    }
    if(match(Token::ASSIGN)){
        vd->value = parseCE();
    }
    
    match(Token::SEMICOL);
    return vd;
}

FunDec *Parser::parseFunDec() {
    FunDec* fd = new FunDec();
    match(Token::FN);
    match(Token::ID);
    fd->nombre = previous->text;
    match(Token::LPAREN);
    if(match(Token::I32)|| match(Token::I64) || match(Token::BOOL)){
        fd->Ptipos.push_back(previous->text);
        match(Token::ID);
        fd->Pnombres.push_back(previous->text);
        while(match(Token::COMA)){
            if(match(Token::I32)|| match(Token::I64) || match(Token::BOOL)){
                fd->Ptipos.push_back(previous->text);
            }
            match(Token::ID);
            fd->Pnombres.push_back(previous->text);
        }
    }
    match(Token::RPAREN);
    if(match(Token::ARROW)){
        fd->return = true;
        if(match(Token::I32)|| match(Token::I64) || match(Token::BOOL)){
                fd->tipo = previous->text;
        }
    }else{ fd->return = false; }

    fd->body = parseBody();

    return fd;
}



Body* Parser::parseBody(){
    Body* b = new Body();
    match(Token::LBRACE)
    while(check(Token::LET)){
        b->declarations.push_back(parseLocalVarDec());
    }
    
    b->StmList.push_back(parseStm());
    while(match(Token::SEMICOL)) {
        b->StmList.push_back(parseStm());
    }
    match(Token::RBRACE)
    return b;
}

Stm* Parser::parseStm() {
    Stm* a;
    Exp* e;
    string variable;
    Body* tb = nullptr;
    Body* fb = nullptr;
    if(match(Token::ID)){
        if(check(Token::LPAREN)){
            string nom = previous->text;
            match(Token::LPAREN);
            FcallExp* f = new FcallExp();
            f->nombre = nom;
            if(!check(Token::RPAREN)){
                f->argumentos.push_back(parseCE());
                while(match(Token::COMA)){
                    f->argumentos.push_back(parseCE());
                }
            }
            match(Token::RPAREN);
            return new FcallStm(f);
        }
        
        else{
            variable = previous->text;
            match(Token::ASSIGN);
            e = parseCE();
            return new AssignStm(variable,e);
        }
    }
    else if(match(Token::PRINT)){
        PrintStm* p;
        match(Token::LPAREN);
        if(!check(Token::RPAREN)){
            p->argumentos.push_back(parseCE());
            while(match(Token::COMA)){
                f->argumentos.push_back(parseCE());
            }
        }
        match(Token::RPAREN);
        return p;
    }
    else if(match(Token::RETURN)) {
        ReturnStm* r  = new ReturnStm();
        match(Token::LPAREN);
        r->e = parseCE();
        match(Token::RPAREN);
        return r;
    }
    else if (match(Token::IF)) {
        IfStm i;
        i->condition = parseCE();
        i->ifbody = parseBody();
        if (check(Token::ELSE) && next(Token::IF)) {
            while(match(Token::ELSE) && next(Token::IF)){
                match(Token:::IF);
                i->elseifcond = true;
                i->elseifcondition.push_back(parseCExp());
                i->elseifbody.push_back(parseBody());
            }
            if(match(Token::ELSE)){
                i->elsecond = true;
                i->elsebody = parseBody();
            }else{
                i->elsecond = false;
            }
        }else if (match(Token::ELSE)){
            i->elseifcond = false;
            i->elsecond = true;
            i->elsebody = parseBody();
        }else{
            i->elseifcond = false;
            i->elsecond = false;
        }
        return i;
    }
    else if (match(Token::WHILE)) {
        e = parseCE();
        tb = parseBody();
        a = new WhileStm(e, tb);
    }
    else if (match(Token::FOR)) {
        match(Token::ID);
        string type = previous->text;
        match(Token::IN);
        bool eq;
        Exp* start = parseCE();
        if(match(Token::RG)){
            eq = false;
        }else if(match(Token::REQ)){
            eq = true;
        }
        Exp* finish = parseCE();
        Body* body = parseBody();
        a = new ForStm(type, eq, start, finish, body);
    }
    else{
        throw runtime_error("Error sintáctico");
    }
    return a;
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
    Exp* e;
    string nom;
    if (match(Token::NUM)) {
        return new NumberExp(stoi(previous->text));
    }
    else if (match(Token::TRUE)) {
        return new NumberExp(1);
    }
    else if (match(Token::FALSE)) {
        return new NumberExp(0);
    }
    else if (match(Token::LPAREN))
    {
        e = parseCE();
        match(Token::RPAREN);
        return e;
    }
    else if (match(Token::ID)) {
        nom = previous->text;
        if(check(Token::LPAREN)) {
            match(Token::LPAREN);
            FcallExp* fcall = new FcallExp();
            fcall->nombre = nom;
            fcall->argumentos.push_back(parseCE());
            while(match(Token::COMA)) {
                fcall->argumentos.push_back(parseCE());
            }
            match(Token::RPAREN);
            return fcall;
        }
        else {
            return new IdExp(nom);
            }
    }
    else {
        throw runtime_error("Error sintáctico");
    }
}
