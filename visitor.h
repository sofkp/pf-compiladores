#ifndef VISITOR_H
#define VISITOR_H

#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include "ast.h"
#include "environment.h"

using namespace std;

/* ============================================================
   Struct Layout para código x86
   ============================================================ */
struct StructLayout {
    int sizeBytes;
    unordered_map<string,int> fieldOffset;
};

/* ============================================================
   Función y Struct Info para el TypeChecker
   ============================================================ */
struct FunInfo {
    TypeInfo* ret;
    vector<TypeInfo*> params;
};

struct StructInfo {
    vector<string> fields;
    vector<TypeInfo*> types;
    unordered_map<string,int> index;
    int sizeBytes;
};

/* ============================================================
   Visitor Base
   ============================================================ */

class Visitor {
public:
    virtual int visit(Program*) = 0;
    virtual int visit(FunDec*) = 0;
    virtual int visit(StructDec*) = 0;
    virtual int visit(GlobalVarDec*) = 0;
    virtual int visit(LocalVarDec*) = 0;
    virtual int visit(Body*) = 0;

    virtual int visit(BinaryExp*) = 0;
    virtual int visit(NumberExp*) = 0;
    virtual int visit(IdExp*) = 0;
    virtual int visit(FcallExp*) = 0;
    virtual int visit(StructInitExp*) = 0;
    virtual int visit(ArrayExp*) = 0;
    virtual int visit(ArrayAccessExp*) = 0;
    virtual int visit(StringExp*) = 0;
    virtual int visit(FieldAccessExp*) = 0;

    virtual int visit(AssignStm*) = 0;
    virtual int visit(FcallStm*) = 0;
    virtual int visit(PrintStm*) = 0;
    virtual int visit(ReturnStm*) = 0;
    virtual int visit(StructStm*) = 0;
    virtual int visit(ArrayStm*) = 0;
    virtual int visit(ForStm*) = 0;
    virtual int visit(WhileStm*) = 0;
    virtual int visit(IfStm*) = 0;
};

/* ============================================================
   TYPE CHECKER (INFERECIA COMPLETA)
   ============================================================ */

class TypeCheckerVisitor : public Visitor {
public:

    // entorno de tipos
    Environment<TypeInfo*> tenv;

    // tablas globales de tipos
    unordered_map<string,StructInfo> structs;
    unordered_map<string,FunInfo> funcs;

    // mapa de tipo inferido por expresión
    unordered_map<Exp*,TypeInfo*> inferred;

    // retorno de la función actual
    string currentFun;
    TypeInfo* expectedReturn;

    // conteo de variables locales
    int localCount;
    unordered_map<string,int> funLocalCount;

    /* ===== Helpers ===== */
    TypeInfo* makeSimple(TypeKind k);
    bool same(TypeInfo* a, TypeInfo* b);
    bool numeric(TypeInfo* a);
    TypeInfo* unifyNum(TypeInfo* a, TypeInfo* b);
    TypeInfo* get(Exp* e);

    /* ===== Type Checking ===== */
    int typeCheck(Program* p);

    int visit(Program*) override;
    int visit(FunDec*) override;
    int visit(StructDec*) override;
    int visit(GlobalVarDec*) override;
    int visit(LocalVarDec*) override;
    int visit(Body*) override;

    int visit(BinaryExp*) override;
    int visit(NumberExp*) override;
    int visit(IdExp*) override;
    int visit(FcallExp*) override;
    int visit(StructInitExp*) override;
    int visit(ArrayExp*) override;
    int visit(ArrayAccessExp*) override;
    int visit(StringExp*) override;
    int visit(FieldAccessExp*) override;

    int visit(AssignStm*) override;
    int visit(FcallStm*) override;
    int visit(PrintStm*) override;
    int visit(ReturnStm*) override;
    int visit(StructStm*) override;
    int visit(ArrayStm*) override;
    int visit(ForStm*) override;
    int visit(WhileStm*) override;
    int visit(IfStm*) override;
};

class ConstEnv {
public:
    vector<unordered_map<string,long>> mem;
    vector<unordered_map<string,bool>> isConst;

    void clear() { mem.clear(); isConst.clear(); }
    void push()  { mem.emplace_back(); isConst.emplace_back(); }
    void pop()   { mem.pop_back(); isConst.pop_back(); }

    void declare(const string& id) {
        mem.back()[id] = 0;
        isConst.back()[id] = false;
    }

    void setConst(const string& id, long v) {
        for (int i = (int)mem.size()-1; i >= 0; i--) {
            if (mem[i].count(id)) {
                mem[i][id] = v;
                isConst[i][id] = true;
                return;
            }
        }
        mem.back()[id] = v;
        isConst.back()[id] = true;
    }

    void unsetConst(const string& id) {
        for (int i = (int)mem.size()-1; i >= 0; i--) {
            if (mem[i].count(id)) {
                isConst[i][id] = false;
                return;
            }
        }
    }

    bool getConst(const string& id, long &out) const {
        for (int i = (int)mem.size()-1; i >= 0; i--) {
            if (mem[i].count(id)) {
                if (isConst[i].at(id)) {
                    out = mem[i].at(id);
                    return true;
                }
                return false;
            }
        }
        return false;
    }
};


class GenCodeVisitor : public Visitor {
public:
    ostream& out;

    TypeCheckerVisitor tipe;
    Environment<int> envStack;
    unordered_map<string,StructLayout> layouts;
    unordered_map<string,string> strLabels;
    int strCount;
    string currentFun;
    int offset;
    int labelCount;
    ConstEnv constEnv;

    GenCodeVisitor(ostream& o);

    void buildStructLayouts(Program* p);
    void emitLValueAddr(Exp* e);
    void copyStruct(int size);
    void copyArray(int size);

    int generate(Program* p);

    int visit(Program*) override;
    int visit(FunDec*) override;
    int visit(StructDec*) override;
    int visit(GlobalVarDec*) override;
    int visit(LocalVarDec*) override;
    int visit(Body*) override;

    int visit(BinaryExp*) override;
    int visit(NumberExp*) override;
    int visit(IdExp*) override;
    int visit(FcallExp*) override;
    int visit(StructInitExp*) override;
    int visit(ArrayExp*) override;
    int visit(ArrayAccessExp*) override;
    int visit(StringExp*) override;
    int visit(FieldAccessExp*) override;

    int visit(AssignStm*) override;
    int visit(FcallStm*) override;
    int visit(PrintStm*) override;
    int visit(ReturnStm*) override;
    int visit(StructStm*) override;
    int visit(ArrayStm*) override;
    int visit(ForStm*) override;
    int visit(WhileStm*) override;
    int visit(IfStm*) override;
};

class ConstVisitor : public Visitor {
public:
    ConstEnv* env;

    ConstVisitor(ConstEnv* e) : env(e) {}

    int visit(NumberExp* e) override {
        e->cont = 1;
        e->valor = e->value;
        return 0;
    }

    int visit(IdExp* e) override {
        long v;
        if (env->getConst(e->value, v)) {
            e->cont = 1;
            e->valor = v;
        } else e->cont = 0;
        return 0;
    }

    int visit(BinaryExp* e) override {
        e->left->accept(this);
        e->right->accept(this);

        if (e->left->cont == 1 && e->right->cont == 1) {
            e->cont = 1;
            long a = e->left->valor;
            long b = e->right->valor;
            switch (e->op){
                case PLUS_OP: e->valor = a+b; break;
                case MINUS_OP: e->valor = a-b; break;
                case MUL_OP: e->valor = a*b; break;
                case DIV_OP: if (b==0) e->cont=0; else e->valor=a/b; break;
                case LT_OP: e->valor = (a<b); break;
                case LE_OP: e->valor = (a<=b); break;
                case GT_OP: e->valor = (a>b); break;
                case GE_OP: e->valor = (a>=b); break;
                case EQ_OP: e->valor = (a==b); break;
                default: e->cont = 0;
            }
        } else e->cont = 0;

        return 0;
    }

    int visit(Program*) override { return 0; }
    int visit(FunDec*) override { return 0; }
    int visit(StructDec*) override { return 0; }
    int visit(GlobalVarDec*) override { return 0; }
    int visit(LocalVarDec*) override { return 0; }
    int visit(Body*) override { return 0; }
    int visit(FcallExp*) override { return 0; }
    int visit(StructInitExp*) override { return 0; }
    int visit(ArrayExp*) override { return 0; }
    int visit(ArrayAccessExp*) override { return 0; }
    int visit(StringExp*) override { return 0; }
    int visit(FieldAccessExp*) override { return 0; }
    int visit(AssignStm*) override { return 0; }
    int visit(FcallStm*) override { return 0; }
    int visit(PrintStm*) override { return 0; }
    int visit(ReturnStm*) override { return 0; }
    int visit(StructStm*) override { return 0; }
    int visit(ArrayStm*) override { return 0; }
    int visit(ForStm*) override { return 0; }
    int visit(WhileStm*) override { return 0; }
    int visit(IfStm*) override { return 0; }
};


#endif
