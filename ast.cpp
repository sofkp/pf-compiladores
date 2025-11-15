#include "ast.h"
#include <iostream>

using namespace std;

// ------------------ Exp ------------------
Exp::~Exp() {}

string Exp::binopToChar(BinaryOp op) {
    switch (op) {
        case PLUS_OP:  return "+";
        case MINUS_OP: return "-";
        case MUL_OP:   return "*";
        case DIV_OP:   return "/";
        case LT_OP:   return "<";
        case LE_OP:   return "<=";
        case GT_OP:   return ">";
        case GE_OP:   return ">=";
        case EQ_OP:   return "==";
        default:       return "?";
    }
}

// ------------------ BinaryExp ------------------
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp o)
    : left(l), right(r), op(o) {}

    
BinaryExp::~BinaryExp() {
    delete left;
    delete right;
}

// ------------------ NumberExp ------------------
NumberExp::NumberExp(int v) : value(v) {}

NumberExp::~NumberExp() {}


// ------------------idExp ------------------
IdExp::IdExp(string v) : value(v) {}

IdExp::~IdExp() {}

// ------------------FcallExp ------------------
FcallExp::FcallExp(string n) : nombre(n) {}

FcallExp::~FcallExp() {}

// ------------------ Stm ------------------

Stm::~Stm(){}

// ------------------AssignStm ------------------
AssignStm::AssignStm(string variable,Exp* expresion){
    var = variable;
    value = expresion;
}
AssignStm::~AssignStm(){}

// ------------------FcallStm ------------------
FcallStm::FcallStm(Exp* expresion){
    e = expresion;
}
FcallStm::~FcallStm(){}

// ------------------FcallStm ------------------
PrintStm::PrintStm(vector<Exp*> expresion){
    argumentos=expresion;
}
PrintStm::~PrintStm(){}

// ------------------ReturnStm ------------------
ReturnStm::ReturnStm(){}
ReturnStm::~ReturnStm(){}

// ------------------FcallStm ------------------
IfStm::IfStm(Exp* c, Body* t, bool e, bool ei): condition(c), ifbody(t), elsecond(e), elseifcond(ei) {}
IfStm::~IfStm(){}

// ------------------FcallStm ------------------
WhileStm::WhileStm(Exp* c, Body* t): condition(c), body(t) {}
WhileStm::~WhileStm(){}

// ------------------ForStm ------------------
ForStm::ForStm(string v, bool e, Exp* s, Exp* f, Body* b){
    var = v;
    equal = e;
    start = s;
    finish = f;
    body = b;
}
ForStm::~ForStm(){}


// ------------------GlobalVarDec ------------------
GlobalVarDec::GlobalVarDec(string t, bool m, string n, Exp* v) {
    type = t;
    mut = m;
    name = n;
    value = v;
}
GlobalVarDec::~GlobalVarDec() {}

// ------------------LocalVarDec ------------------
LocalVarDec::LocalVarDec(string t, bool m, string n) {
    type = t;
    mut = m;
    name = n;
}
LocalVarDec::~LocalVarDec() {}

// ------------------Body ------------------
Body::Body(){
    declarations=list<VarDec*>();
    StmList=list<Stm*>();
}
Body::~Body(){}

// ------------------FunDec ------------------
FunDec::FunDec(string n, Body* b){
    nombre=n;
    vody=b;
}
FunDec::~FunDec(){}


// ------------------Body ------------------
Program::Program(){
    vdlist=list<GlobalVarDec*>();
    fdlist=list<FunDec*>();
}
Program::~Program(){}