#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
using namespace std;

class Visitor;
class VarDec;

enum BinaryOp { 
    PLUS_OP,
    MINUS_OP,
    MUL_OP,
    DIV_OP,
    LT_OP,
    LE_OP,
    GT_OP,
    GE_OP,
    EQ_OP
};

enum TypeKind {
    T_I32,
    T_I64,
    T_BOOL,
    T_STRING,
    T_ARRAY,
    T_STRUCT,
    T_UNKNOWN
};

struct TypeInfo {
    TypeKind kind;
    string structName;
    TypeInfo* elem;
    int size;

    TypeInfo(TypeKind k);
    TypeInfo(const string& st);
    TypeInfo(TypeKind k, TypeInfo* e, int s);
    ~TypeInfo();
};

class Exp {
public:
    TypeInfo* inferredType;
    int cont = 0;
    long valor = 0;
    Exp();
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;

    static string binopToChar(BinaryOp op);
};

class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;

    BinaryExp(Exp*, Exp*, BinaryOp);
    ~BinaryExp();
    int accept(Visitor* visitor);
};

class NumberExp : public Exp {
public:
    long value;

    NumberExp(long);
    ~NumberExp();
    int accept(Visitor* visitor);
};

class IdExp : public Exp {
public:
    string value;

    IdExp(string);
    ~IdExp();
    int accept(Visitor* visitor);
};

class FcallExp: public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;

    FcallExp(string);
    ~FcallExp();
    int accept(Visitor* visitor);
};

class StructInitExp : public Exp {
public:
    string nombre;
    vector<string> fields;
    vector<Exp*> values;

    StructInitExp(string, vector<string>, vector<Exp*>);
    ~StructInitExp();
    int accept(Visitor* visitor);
};

class ArrayExp: public Exp {
public:
    vector<Exp*> elems;

    ArrayExp();
    ~ArrayExp();
    int accept(Visitor* visitor);
};

class ArrayAccessExp : public Exp {
public:
    Exp* arr;
    Exp* index;

    ArrayAccessExp(Exp*, Exp*);
    ~ArrayAccessExp();
    int accept(Visitor* visitor);
};

class StringExp: public Exp {
public:
    string s;

    StringExp(string);
    ~StringExp();
    int accept(Visitor* visitor);
};

class FieldAccessExp : public Exp {
public:
    Exp* obj;
    string field;

    FieldAccessExp(Exp*, string);
    ~FieldAccessExp();
    int accept(Visitor* visitor);
};

class Stm {
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

class AssignStm: public Stm {
public:
    Exp* left;
    Exp* value;

    AssignStm(Exp*, Exp*);
    ~AssignStm();
    int accept(Visitor* visitor);
};

class FcallStm: public Stm {
public:
    FcallExp* e;

    FcallStm(FcallExp*);
    ~FcallStm();
    int accept(Visitor* visitor);
};

class PrintStm: public Stm {
public:
    Exp* e;

    PrintStm(Exp*);
    ~PrintStm();
    int accept(Visitor* visitor);
};

class ReturnStm: public Stm {
public:
    Exp* value;

    ReturnStm(Exp*);
    ~ReturnStm();
    int accept(Visitor* visitor);
};

class StructStm: public Stm {
public:
    Exp* object;
    string field;
    Exp* value;

    StructStm(Exp*, string, Exp*);
    ~StructStm();
    int accept(Visitor* visitor);
};

class ArrayStm: public Stm {
public:
    Exp* array;
    Exp* index;
    Exp* value;

    ArrayStm(Exp*, Exp* idx, Exp* val);
    ~ArrayStm();
    int accept(Visitor* visitor);
};

class GlobalVarDec{
public:
    TypeInfo* type;
    bool mut;
    string name;
    Exp* value;

    GlobalVarDec(string, TypeInfo*, bool, Exp*);
    ~GlobalVarDec();
    int accept(Visitor* visitor);
};

class LocalVarDec{
public:
    bool mut;
    string name;
    Exp* value;
    TypeInfo* type;

    LocalVarDec(string, TypeInfo*, bool, Exp*);
    ~LocalVarDec();
    int accept(Visitor* visitor);
};

class Body{
public:
    list<Stm*> StmList;
    list<LocalVarDec*> declarations;

    Body();
    ~Body();
    int accept(Visitor* visitor);
};

class ForStm: public Stm {
public:
    string var;
    bool equal;
    Exp* start;
    Exp* finish;
    Body* body;

    ForStm(string, bool, Exp*, Exp*, Body*);
    ~ForStm();
    int accept(Visitor* visitor);
};

class WhileStm: public Stm {
public:
    Exp* condition;
    Body* body;

    WhileStm(Exp*, Body*);
    ~WhileStm();
    int accept(Visitor* visitor);
};

class IfStm: public Stm {
public:
    Exp* condition;
    bool elsecond;
    Body* ifbody;
    Body* elsebody;

    IfStm(Exp*, Body*, bool, Body*);
    ~IfStm();
    int accept(Visitor* visitor);
};

class FunDec{
public:
    string nombre;
    TypeInfo* returnType;
    Body* body;
    vector<TypeInfo*> Ptipos;
    vector<string> Pnombres;

    FunDec(string, vector<string>, vector<TypeInfo*>, TypeInfo*, Body*);
    ~FunDec();
    int accept(Visitor* visitor);
};

class StructDec{
public:
    string nombre;
    vector<string> fields;
    vector<TypeInfo*> tipos;
    vector<int> fieldOffsets;
    int totalSize;

    StructDec();
    ~StructDec();
    int accept(Visitor* visitor);
};

class Program{
public:
    list<GlobalVarDec*> vdlist;
    list<FunDec*> fdlist;
    list<StructDec*> stlist;

    Program();
    ~Program();
    int accept(Visitor* visitor);
};

#endif
