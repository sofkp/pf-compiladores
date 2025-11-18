#ifndef VISITOR_H
#define VISITOR_H
#include "ast.h"
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
#include "environment.h"
using namespace std;

class Program;
class FunDec;
class GlovalVarDec;
class LocalVarDec;
class Body;
class BinaryExp;
class NumberExp;
class IdExp;
class FcallExp;
class AssignStm;
class FcallStm;
class PrintStm;
class ReturnStm;
class ForStm;
class WhileStm;
class IfStm;


class Visitor {
public:
    // 
    virtual int visit(Program* p) = 0;
    virtual int visit(FunDec* fd) = 0;
    virtual int visit(GlobalVarDec* vd) = 0;
    virtual int visit(LocalVarDec* vd) = 0;
    virtual int visit(Body* body) = 0;

    virtual int visit(BinaryExp* exp) = 0;
    virtual int visit(NumberExp* exp) = 0;
    virtual int visit(IdExp* exp) = 0;
    virtual int visit(FcallExp* exp) = 0;
    
    virtual int visit(AssignStm* stm) = 0;
    virtual int visit(FcallStm* stm) = 0;
    virtual int visit(PrintStm* stm) = 0;
    virtual int visit(ReturnStm* stm) = 0;
    virtual int visit(ForStm* stm) = 0;
    virtual int visit(WhileStm* stm) = 0;
    virtual int visit(IfStm* stm) = 0;
};


class PrintVisitor : public Visitor {
public:
    int indent = 0;
    void imprimir(Program* program); 
    int visit(Program* program) override;
    int visit(FunDec* fd) override;
    int visit(GlobalVarDec* vd) override;
    int visit(LocalVarDec* vd) override;
    int visit(Body* body) override;

    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(FcallExp* exp) override;
    
    int visit(AssignStm* stm) override;
    int visit(FcallStm* stm) override;
    int visit(PrintStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(IfStm* stm) override;

    void tab() {
        for (int i = 0; i < indent; i++)
            cout << "    ";
    }


};

class EVALVisitor : public Visitor {
public:
    Environment<int> env;
    void interprete(Program* program);

    int visit(Program* program) override;
    int visit(FunDec* fd) override;
    int visit(GlobalVarDec* vd) override;
    int visit(LocalVarDec* vd) override;
    int visit(Body* body) override;

    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(FcallExp* exp) override;
    
    int visit(AssignStm* stm) override;
    int visit(FcallStm* stm) override;
    int visit(PrintStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(IfStm* stm) override;
};

class TypeCheckerVisitor : public Visitor {
public:
    unordered_map<string,int> fun_locales;
    int locales = 0;
    int tipe(Program* program);
    int visit(Program* program) override;
    int visit(FunDec* fd) override;
    int visit(GlobalVarDec* vd) override;
    int visit(LocalVarDec* vd) override;
    int visit(Body* body) override;

    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(FcallExp* exp) override;
    
    int visit(AssignStm* stm) override;
    int visit(FcallStm* stm) override;
    int visit(PrintStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(IfStm* stm) override;
};

class GenCodeVisitor : public Visitor {
private:
    std::ostream& out;
public:
    TypeCheckerVisitor tipe;
    unordered_map<string,int> fun_reserva;
    GenCodeVisitor(std::ostream& out) : out(out) {
        memoria = new Environment<int>();
    }
    ~GenCodeVisitor();
    
    Environment<int>* memoria;
    unordered_map<string, bool> memoriaGlobal;

    int offset = -8;
    int labelcont = 0;
    bool entornoFuncion = false;
    string nombreFuncion;

    int generar(Program* program);

    int visit(Program* program) override;
    int visit(FunDec* fd) override;
    int visit(GlobalVarDec* vd) override;
    int visit(LocalVarDec* vd) override;
    int visit(Body* body) override;

    int visit(BinaryExp* exp) override;
    int visit(NumberExp* exp) override;
    int visit(IdExp* exp) override;
    int visit(FcallExp* exp) override;
    
    int visit(AssignStm* stm) override;
    int visit(FcallStm* stm) override;
    int visit(PrintStm* stm) override;
    int visit(ReturnStm* stm) override;
    int visit(ForStm* stm) override;
    int visit(WhileStm* stm) override;
    int visit(IfStm* stm) override;

};

#endif // VISITOR_H