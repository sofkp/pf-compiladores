#include <iostream>
#include "token.h"
#include "scanner.h"
#include "ast.h"
#include "parser.h"

using namespace std;

Parser::Parser(Scanner* sc) : scanner(sc) {
    previous = nullptr;
    current = scanner->nextToken();
    if (current->type == Token::ERR)
        throw runtime_error("Error léxico");
}

bool Parser::match(Token::Type t) {
    if (check(t)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(Token::Type t) {
    if (isAtEnd()) return false;
    return current->type == t;
}

bool Parser::advance() {
    if (!isAtEnd()) {
        Token* tmp = current;
        if (previous) delete previous;
        current = scanner->nextToken();
        previous = tmp;

        if (check(Token::ERR))
            throw runtime_error("Error léxico");
        return true;
    }
    return false;
}

bool Parser::isAtEnd() {
    return current->type == Token::END;
}

/* ============================================================
   TYPE PARSER
   ============================================================ */

TypeInfo* Parser::parseType() {
    // Array tipo:  [T; N]
    if (match(Token::LBRACKET)) {
        TypeInfo* elem = parseType();
        match(Token::SEMICOL);
        match(Token::NUM);
        int sz = stoi(previous->text);
        match(Token::RBRACKET);
        return new TypeInfo(T_ARRAY, elem, sz);
    }

    if (match(Token::I32)) return new TypeInfo(T_I32);
    if (match(Token::I64)) return new TypeInfo(T_I64);
    if (match(Token::BOOL)) return new TypeInfo(T_BOOL);

    // nombre de struct
    if (match(Token::ID))
        return new TypeInfo(previous->text);

    throw runtime_error("Tipo inválido");
}

/* ============================================================
   PROGRAM
   ============================================================ */

Program* Parser::parseProgram() {
    Program* p = new Program();

    while (check(Token::STRUCT))
        p->stlist.push_back(parseStructDec());

    while (check(Token::STATIC))
        p->vdlist.push_back(parseGlobalVarDec());

    while (check(Token::FN))
        p->fdlist.push_back(parseFunDec());

    return p;
}

/* ============================================================
   GLOBAL Y LOCAL VAR
   ============================================================ */

GlobalVarDec* Parser::parseGlobalVarDec() {
    match(Token::STATIC);
    bool mut = match(Token::MUT);

    match(Token::ID);
    string name = previous->text;

    TypeInfo* t = nullptr;
    if (match(Token::COLON))
        t = parseType();

    match(Token::ASSIGN);
    Exp* v = parseCE();
    match(Token::SEMICOL);

    if (!t) t = new TypeInfo(T_UNKNOWN);

    return new GlobalVarDec(name, t, mut, v);
}

LocalVarDec* Parser::parseLocalVarDec() {
    match(Token::LET);
    bool mut = match(Token::MUT);

    match(Token::ID);
    string name = previous->text;

    TypeInfo* t = nullptr;
    Exp* v = nullptr;

    if (match(Token::COLON))
        t = parseType();

    if (match(Token::ASSIGN))
        v = parseCE();

    match(Token::SEMICOL);

    if (!t) t = new TypeInfo(T_UNKNOWN);

    return new LocalVarDec(name, t, mut, v);
}

/* ============================================================
   STRUCT DEC
   ============================================================ */

StructDec* Parser::parseStructDec() {
    match(Token::STRUCT);
    match(Token::ID);
    string name = previous->text;

    StructDec* sd = new StructDec();
    sd->nombre = name;

    match(Token::LBRACE);

    if (!check(Token::RBRACE)) {
        while (true) {
            match(Token::ID);
            string fname = previous->text;
            match(Token::COLON);

            TypeInfo* t = parseType();
            sd->fields.push_back(fname);
            sd->tipos.push_back(t);

            if (!match(Token::COMA)) break;
        }
    }

    match(Token::RBRACE);
    return sd;
}

/* ============================================================
   FUNCTION
   ============================================================ */

FunDec* Parser::parseFunDec() {
    match(Token::FN);
    match(Token::ID);
    string name = previous->text;

    match(Token::LPAREN);

    vector<string> Pnames;
    vector<TypeInfo*> Ptypes;

    if (!check(Token::RPAREN)) {
        while (true) {
            match(Token::ID);
            string pname = previous->text;
            match(Token::COLON);
            TypeInfo* tp = parseType();
            Pnames.push_back(pname);
            Ptypes.push_back(tp);
            if (!match(Token::COMA)) break;
        }
    }

    match(Token::RPAREN);

    TypeInfo* ret = new TypeInfo(T_UNKNOWN);
    if (match(Token::ARROW))
        ret = parseType();

    Body* body = parseBody();

    return new FunDec(name, Pnames, Ptypes, ret, body);
}

/* ============================================================
   BODY
   ============================================================ */

Body* Parser::parseBody() {
    Body* b = new Body();
    match(Token::LBRACE);

    // Ahora recorremos TODO el bloque
    // y cada vez que veamos un LET lo tratamos como declaración local.
    while (!check(Token::RBRACE)) {
        if (check(Token::LET)) {
            b->declarations.push_back(parseLocalVarDec());
        } else {
            b->StmList.push_back(parseStm());
        }
    }

    match(Token::RBRACE);
    return b;
}

Stm* Parser::parseStm() {
    if (match(Token::ID)) {

        Exp* base = new IdExp(previous->text);
        Exp* obj = base;

        while (true) {

            if (match(Token::LPAREN)) {
                FcallExp* f = new FcallExp(((IdExp*)obj)->value);

                if (!check(Token::RPAREN)) {
                    f->argumentos.push_back(parseCE());
                    while (match(Token::COMA))
                        f->argumentos.push_back(parseCE());
                }

                match(Token::RPAREN);
                match(Token::SEMICOL);
                return new FcallStm(f);
            }

            if (match(Token::LBRACKET)) {

                Exp* idx = parseCE();
                match(Token::RBRACKET);

                if (match(Token::ASSIGN)) {
                    Exp* v = parseCE();
                    match(Token::SEMICOL);
                    return new ArrayStm(obj, idx, v);
                }

                obj = new ArrayAccessExp(obj, idx);
                continue;
            }

            if (match(Token::DOT)) {
                match(Token::ID);
                string field = previous->text;

                if (match(Token::ASSIGN)) {
                    Exp* v = parseCE();
                    match(Token::SEMICOL);
                    return new StructStm(obj, field, v);
                }

                obj = new FieldAccessExp(obj, field);
                continue;
            }

            break;
        }

        if (match(Token::ASSIGN)) {
            Exp* v = parseCE();
            match(Token::SEMICOL);
            return new AssignStm(base, v);
        }

        throw runtime_error("Statement inválido");
    }

    if (match(Token::PRINT)) {
        match(Token::LPAREN);
        Exp* e = parseCE();
        match(Token::RPAREN);
        match(Token::SEMICOL);
        return new PrintStm(e);
    }

    if (match(Token::RETURN)) {
        Exp* e = parseCE();
        match(Token::SEMICOL);
        return new ReturnStm(e);
    }

    if (match(Token::IF)) {
        Exp* cond = parseCE();
        Body* ifb = parseBody();
        Body* elseb = nullptr;
        bool hasElse = false;

        if (match(Token::ELSE)) {
            hasElse = true;
            elseb = parseBody();
        }

        return new IfStm(cond, ifb, hasElse, elseb);
    }

    if (match(Token::WHILE)) {
        Exp* c = parseCE();
        Body* b = parseBody();
        return new WhileStm(c, b);
    }

    if (match(Token::FOR)) {
        match(Token::ID);
        string var = previous->text;

        match(Token::IN);
        Exp* start = parseCE();

        bool eq = false;
        if (match(Token::RG)) eq = false;
        else if (match(Token::REQ)) eq = true;
        else throw runtime_error("expected .. or ..=");

        Exp* finish = parseCE();
        Body* b = parseBody();

        return new ForStm(var, eq, start, finish, b);
    }

    throw runtime_error("Statement inválido");
}

Exp* Parser::parseCE() {
    Exp* l = parseE();
    if (check(Token::LBRACE)) return l;
    if (match(Token::LE) || match(Token::LT) || match(Token::GE) ||
        match(Token::GT) || match(Token::EQ)) {

        Token::Type op = previous->type;
        Exp* r = parseE();
        BinaryOp bop;

        if (op == Token::LE) bop = LE_OP;
        else if (op == Token::LT) bop = LT_OP;
        else if (op == Token::GE) bop = GE_OP;
        else if (op == Token::GT) bop = GT_OP;
        else bop = EQ_OP;

        return new BinaryExp(l, r, bop);
    }
    return l;
}

Exp* Parser::parseE() {
    Exp* l = parseT();
    while (match(Token::PLUS) || match(Token::MINUS)) {
        BinaryOp op = (previous->type == Token::PLUS) ? PLUS_OP : MINUS_OP;
        Exp* r = parseT();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseT() {
    Exp* l = parseI();
    while (match(Token::MUL) || match(Token::DIV)) {
        BinaryOp op = (previous->type == Token::MUL) ? MUL_OP : DIV_OP;
        Exp* r = parseI();
        l = new BinaryExp(l, r, op);
    }
    return l;
}

Exp* Parser::parseI() {
    Exp* e = parseF();

    while (true) {
        if (match(Token::LBRACKET)) {
            Exp* idx = parseCE();
            match(Token::RBRACKET);
            e = new ArrayAccessExp(e, idx);
            continue;
        }
        if (match(Token::DOT)) {
            match(Token::ID);
            e = new FieldAccessExp(e, previous->text);
            continue;
        }
        if (match(Token::LPAREN)) {
            FcallExp* f = new FcallExp(((IdExp*)e)->value);
            if (!check(Token::RPAREN)) {
                f->argumentos.push_back(parseCE());
                while (match(Token::COMA))
                    f->argumentos.push_back(parseCE());
            }
            match(Token::RPAREN);
            e = f;
            continue;
        }
        break;
    }

    return e;
}

Exp* Parser::parseF() {

    if (match(Token::NUM))
        return new NumberExp(stol(previous->text));

    if (match(Token::TRUE)) 
        return new NumberExp(1);

    if (match(Token::FALSE))
        return new NumberExp(0);

    if (match(Token::STRING))
        return new StringExp(previous->text);

    if (match(Token::LPAREN)) {
        Exp* e = parseCE();
        match(Token::RPAREN);
        return e;
    }

    if (match(Token::LBRACKET)){
        ArrayExp* arr = new ArrayExp();
        if (!check(Token::RBRACKET)){
            arr->elems.push_back(parseCE());
            while (match(Token::COMA))
                arr->elems.push_back(parseCE());
        }
        match(Token::RBRACKET);
        return arr;
    }

    if (match(Token::ID)) {
        string name = previous->text;

        if (match(Token::LBRACE)) {
            vector<string> flds;
            vector<Exp*> vals;

            if (!check(Token::RBRACE)) {
                while (true) {
                    match(Token::ID);
                    string fname = previous->text;
                    match(Token::COLON);
                    flds.push_back(fname);
                    vals.push_back(parseCE());
                    if (!match(Token::COMA)) break;
                }
            }

            match(Token::RBRACE);
            return new StructInitExp(name, flds, vals);
        }

        Exp* e = new IdExp(name);

        while (true) {
            if (match(Token::LPAREN)) {
                FcallExp* f = new FcallExp(((IdExp*)e)->value);
                if (!check(Token::RPAREN)) {
                    f->argumentos.push_back(parseCE());
                    while (match(Token::COMA))
                        f->argumentos.push_back(parseCE());
                }
                match(Token::RPAREN);
                e = f;
                continue;
            }
            if (match(Token::LBRACKET)) {
                Exp* idx = parseCE();
                match(Token::RBRACKET);
                e = new ArrayAccessExp(e, idx);
                continue;
            }
            if (match(Token::DOT)) {
                match(Token::ID);
                e = new FieldAccessExp(e, previous->text);
                continue;
            }
            break;
        }
        return e;
    }
    
    if (check(Token::LBRACE))
        throw runtime_error("Factor inválido");
}
