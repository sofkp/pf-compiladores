#ifndef AST_H
#define AST_H

#include <string>
#include <list>
#include <ostream>
#include <vector>
using namespace std;

class Visitor;
class VarDec;

// Operadores binarios soportados
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

// Clase abstracta Exp
class Exp {
public:
    virtual int  accept(Visitor* visitor) = 0;
    virtual ~Exp() = 0;  // Destructor puro → clase abstracta
    static string binopToChar(BinaryOp op);  // Conversión operador → string
};

// Expresión binaria
class BinaryExp : public Exp {
public:
    Exp* left;
    Exp* right;
    BinaryOp op;
    int accept(Visitor* visitor);
    BinaryExp(Exp*, Exp*, BinaryOp);
    ~BinaryExp();

};

// Expresión numérica
class NumberExp : public Exp {
public:
    int value;
    int accept(Visitor* visitor);
    NumberExp(int);
    ~NumberExp();
};

// Expresión numérica
class IdExp : public Exp {
public:
    string value;
    int accept(Visitor* visitor);
    IdExp(string);
    ~IdExp();
};

class FcallExp: public Exp {
public:
    string nombre;
    vector<Exp*> argumentos;
    int accept(Visitor* visitor);
    FcallExp(string){};
    ~FcallExp(){};
};


class Stm{
public:
    virtual int accept(Visitor* visitor) = 0;
    virtual ~Stm() = 0;
};

class AssignStm: public Stm {
public:
    string var;
    Exp* value;
    AssignStm(string, Exp*);
    ~AssignStm();
    int accept(Visitor* visitor);
};

class FcallStm: public Stm {
public:
    FcallExp* e;
    int accept(Visitor* visitor);
    FcallStm(FcallExp*);
    ~FcallStm(){};
};

class PrintStm: public Stm {
public:
    vector<Exp*> argumentos;
    PrintStm(vector<Exp*>);
    ~PrintStm();
    int accept(Visitor* visitor);
};

class ReturnStm: public Stm {
public:
    Exp* value;
    ReturnStm(Exp* value){};
    ~ReturnStm(){};
    int accept(Visitor* visitor);
};

class IfStm: public Stm {
public:
    Exp* condition;
    bool elsecond;
    bool elseifcond;
    Body* ifbody;
    vector<Exp*> elseifcondition;
    Body* elsebody;
    vector<Body*> elseifbody;
    IfStm(Exp*, Body*, bool, bool);
    int accept(Visitor* visitor);
    ~IfStm(){};
};

class WhileStm: public Stm {
public:
    Exp* condition;
    Body* body;
    WhileStm(Exp*, Body*);
    int accept(Visitor* visitor);
    ~WhileStm(){};
};

class ForStm: public Stm {
public:
    string var;
    bool equal;
    Exp* start;
    Exp* finish;
    Body* body;
    ForStm(string, bool,Exp*, Exp*, Body*);
    int accept(Visitor* visitor);
    ~ForStm(){};
};

class GlobalVarDec{
public:
    string type;
    bool mut;
    string name;
    Exp* value;
    GlobalVarDec(string type, bool mut, string name, Exp* value);
    int accept(Visitor* visitor);
    ~GlobalVarDec();
};

class LocalVarDec{
public:
    string type;
    bool mut;
    string name;
    Exp* value;
    LocalVarDec(string type, bool mut, string name);
    int accept(Visitor* visitor);
    ~LocalVarDec();
};

class Body{
public:
    list<Stm*> StmList;
    list<LocalVarDec*> declarations;
    int accept(Visitor* visitor);
    Body();
    ~Body();
};

class FunDec{
public:
    string nombre;
    string tipo;
    bool retun;
    Body* body;
    vector<string> Ptipos;
    vector<string> Pnombres;
    int accept(Visitor* visitor);
    FunDec(string nombre, Body* body){};
    ~FunDec(){};
};

class Program{
public:
    list<GlobalVarDec*> vdlist;
    list<FunDec*> fdlist;
    Program(){};
    ~Program(){};
    int accept(Visitor* visitor);
};

#endif // AST_H
