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

/* ============================================================
   GEN CODE VISITOR (X86-64 REAL)
   ============================================================ */

class GenCodeVisitor : public Visitor {
public:
    ostream& out;

    // typechecker usado para expTypes y layouts
    TypeCheckerVisitor tc;

    // tabla real de offsets en stack
    Environment<int> envStack;

    // layouts de struct para x86
    unordered_map<string,StructLayout> layouts;

    // manejo de strings en .rodata
    unordered_map<string,string> strLabels;
    int strCount;

    // nombre de funcion actual
    string currentFun;

    // offset actual en stack
    int offset;

    // contador de labels
    int labelCount;

    GenCodeVisitor(ostream& o);

    /* ===== Helpers ===== */
    void buildStructLayouts(Program* p);
    void emitLValueAddr(Exp* e); // coloca dirección en %rax
    void copyStruct(int size);
    void copyArray(int size);

    /* ===== Visit ===== */
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

#endif
