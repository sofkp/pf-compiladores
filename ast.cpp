#include "ast.h"
#include <iostream>

using namespace std;

TypeInfo::TypeInfo(TypeKind k) : kind(k), structName(""), elem(nullptr), size(-1) {}

TypeInfo::TypeInfo(const string& st) : kind(T_STRUCT), structName(st), elem(nullptr), size(-1) {}

TypeInfo::TypeInfo(TypeKind k, TypeInfo* e, int s) : kind(k), structName(""), elem(e), size(s) {}

TypeInfo::~TypeInfo() {
    delete elem;
}
// ------------------ Exp ------------------
Exp::Exp() : inferredType(nullptr) {}
Exp::~Exp(){}

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
NumberExp::NumberExp(long v) : value(v) {}

NumberExp::~NumberExp() {}


// ------------------idExp ------------------
IdExp::IdExp(string v) : value(v) {}

IdExp::~IdExp() {}

// ------------------FcallExp ------------------
FcallExp::FcallExp(string n) {
    nombre = n;
    argumentos = vector<Exp*>();
}

FcallExp::~FcallExp() {}

// ------------------StructInitExp ------------------
StructInitExp::StructInitExp(string n, vector<string> f, vector<Exp*> v)
    : nombre(n), fields(f), values(v) {}

StructInitExp::~StructInitExp() {
}


// ------------------ArrayExp ------------------
ArrayExp::ArrayExp() {
    elems = vector<Exp*>();
}

ArrayExp::~ArrayExp() {}

// ------------------ArrayAccessExp ------------------
ArrayAccessExp::ArrayAccessExp(Exp* a, Exp* i) : arr(a), index(i) {}

ArrayAccessExp::~ArrayAccessExp() {
}

// ------------------StringExp ------------------
StringExp::StringExp(string ss) : s(ss) {
}

StringExp::~StringExp() {}

// ------------------FieldAccessExp ------------------
FieldAccessExp::FieldAccessExp(Exp* o, string f) : obj(o), field(f) {}

FieldAccessExp::~FieldAccessExp() {
}

// ------------------ Stm ------------------

Stm::~Stm(){}

// ------------------AssignStm ------------------
AssignStm::AssignStm(Exp* l, Exp* v) : left(l), value(v) {}
AssignStm::~AssignStm(){}

// ------------------FcallStm ------------------
FcallStm::FcallStm(FcallExp* expresion){
    e = expresion;
}
FcallStm::~FcallStm(){}

// ------------------PrintStm ------------------
PrintStm::PrintStm(Exp* expresion){
    e=expresion;
}
PrintStm::~PrintStm(){}

// ------------------ReturnStm ------------------
ReturnStm::ReturnStm(Exp* v): value(v){}
ReturnStm::~ReturnStm(){}

// ------------------FcallStm ------------------
IfStm::IfStm(Exp* c, Body* t, bool e, Body* el): condition(c), ifbody(t), elsecond(e), elsebody(el) {}
IfStm::~IfStm(){}

// ------------------WhileStm ------------------
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

// ------------------ArrayStm ------------------
ArrayStm::ArrayStm(Exp* a, Exp* idx, Exp* val) : array(a), index(idx), value(val) {}
ArrayStm::~ArrayStm(){}

// ------------------StructStm ------------------
StructStm::StructStm(Exp* obj, string fld, Exp* val) : object(obj), field(fld), value(val) {}
StructStm::~StructStm(){}


// ------------------GlobalVarDec ------------------
GlobalVarDec::GlobalVarDec(string n, TypeInfo* t, bool m, Exp* v) : name(n), type(t), mut(m), value(v) {}
GlobalVarDec::~GlobalVarDec() {}

// ------------------LocalVarDec ------------------
LocalVarDec::LocalVarDec(string n, TypeInfo* t, bool m, Exp* v) : name(n), type(t), mut(m), value(v) {}
LocalVarDec::~LocalVarDec() {}

// ------------------Body ------------------
Body::Body(){
    declarations=list<LocalVarDec*>();
    StmList=list<Stm*>();
}
Body::~Body(){}

// ------------------FunDec ------------------
FunDec::FunDec(string n, vector<string> pn, vector<TypeInfo*> pt, TypeInfo* ret, Body* b)
     : nombre(n), Pnombres(pn), Ptipos(pt), returnType(ret), body(b) {}
FunDec::~FunDec(){}


// ------------------StructDec ------------------
StructDec::StructDec(): nombre(""), fields(), tipos(), fieldOffsets(), totalSize(0) {}

StructDec::~StructDec(){}


// ------------------Body ------------------
Program::Program(){
    vdlist=list<GlobalVarDec*>();
    fdlist=list<FunDec*>();
    stlist=list<StructDec*>();
}
Program::~Program(){}