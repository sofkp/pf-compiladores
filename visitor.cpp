#include <iostream>
#include "ast.h"
#include "visitor.h"
#include <unordered_map>
using namespace std;

///////////////////////////////////////////////////////////////////////////////////
int Program::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int FunDec::accept(Visitor* visitor){
    return visitor->visit(this);
}
int GlobalVarDec::accept(Visitor* visitor){
    return visitor->visit(this);
}
int LocalVarDec::accept(Visitor* visitor){
    return visitor->visit(this);
}
int Body::accept(Visitor* visitor){
    return visitor->visit(this);
}

int BinaryExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int NumberExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int IdExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int FcallExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int AssignStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int FcallStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int PrintStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int ReturnStm::accept(Visitor* visitor){
    return visitor->visit(this);
}
int ForStm::accept(Visitor* visitor){
    return visitor->visit(this);
}
int WhileStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int IfStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

///////////////////////////////////////////////////////////////////////////////////
//-------PRINT VISITOR------
void PrintVisitor::imprimir(Program* programa){
    cout << "Codigo:" << endl; 
    programa->accept(this);
    cout << endl;
}

int PrintVisitor::visit(Program* p) {
    for (auto g : p->vdlist) {
        g->accept(this);
        cout << endl;
    }
    if (!p->vdlist.empty() && !p->fdlist.empty()) cout << endl;
    for (auto f : p->fdlist) {
        f->accept(this);
        cout << endl;
    }
    return 0;
}

int PrintVisitor::visit(FunDec* f) {
    tab();
    cout << "fn " << f->nombre << "(";
    for (int i=0; i<f->Pnombres.size(); i++){
        if(i>0) cout << ", ";
        cout << f->Pnombres[i] << ": " << f->Ptipos[i];
    }
    cout << ")";
    if(f->returns)
        cout << " -> " << f->tipo;
    cout << " {" << endl;

    indent++;
    if (f->body) f->body->accept(this);
    indent--;

    tab();
    cout << "}";
    return 0;
}

int PrintVisitor::visit(GlobalVarDec* vd) {
    tab();
    cout << "static ";
    if(vd->mut) cout << "mut ";
    cout << vd->name << ": " << vd->type << " = ";
    vd->value->accept(this);
    cout << ";";
    return 0;
}

int PrintVisitor::visit(LocalVarDec* vd) {
    tab();
    cout << "let ";
    if(vd->mut) cout << "mut ";
    cout << vd->name << ": " << vd->type;
    if(vd->value) {
        cout << " = ";
        vd->value->accept(this);
    }
    cout << ";";
    return 0;
}

int PrintVisitor::visit(Body* body) {
    for(auto d : body->declarations){
        d->accept(this);
        cout << endl;
    }
    for(auto s : body->StmList){
        s->accept(this);
        cout << endl;
    }
    return 0;
}

int PrintVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    cout << ' ' << Exp::binopToChar(exp->op) << ' ';
    exp->right->accept(this);
    return 0;
}

int PrintVisitor::visit(NumberExp* exp) {
    cout << exp->value;
    return 0;
}


int PrintVisitor::visit(IdExp* exp) {
    cout << exp->value;
    return 0;
}


int PrintVisitor::visit(FcallExp* exp) {
    cout << exp->nombre << "(";
    for(int i=0; i<exp->argumentos.size(); i++){
        if(i>0) cout << ", ";
        exp->argumentos[i]->accept(this);
    }
    cout << ")";
    return 0;
}

int PrintVisitor::visit(AssignStm* stm) {
    tab();
    cout << stm->var << "=";
    stm->value->accept(this);
    cout << ";" << endl;
    return 0;
}

int PrintVisitor::visit(FcallStm* stm) {
    tab();
    stm->e->accept(this);
    cout << ";";
    return 0;
}

int PrintVisitor::visit(PrintStm* stm) {
    tab();
    cout << "println!(";
    stm->e->accept(this);
    cout << ");" << endl;
    return 0;
}

int PrintVisitor::visit(ReturnStm* stm) {
    tab();
    cout << "return ";
    if (stm->value) stm->value->accept(this);
    cout << ";";
    return 0;
}


int PrintVisitor::visit(ForStm* stm) {
    tab();
    cout << "for " << stm->var << " in ";
    stm->start->accept(this);
    if(stm->equal) cout << "..=";
    else cout << "..";
    stm->finish->accept(this);
    cout << " {" <<endl;

    indent++;
    stm->body->accept(this);
    indent--;

    tab();
    cout << "}" <<endl;
    return 0;
}


int PrintVisitor::visit(WhileStm* stm) {
    tab();
    cout << "while " ;
    stm->condition->accept(this);
    cout  << " {" << endl;
    indent++;
    stm->body->accept(this);
    indent--;
    tab();
    cout << "}" << endl;
    return 0;
}

int PrintVisitor::visit(IfStm* stm) {
    tab();
    cout << "if " ;
    stm->condition->accept(this);
    cout <<" {" << endl;
    indent++;
    stm->ifbody->accept(this);
    indent--;
    tab();
    if(stm->elsecond) {
        tab();
        cout << "} else {" << endl;

        indent++;
        stm->elsebody->accept(this);
        indent--;

        tab();
        cout << "}";
    } else {
        tab();
        cout << "}";
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////
//-------EVAL VISITOR------
void EVALVisitor::interprete(Program* programa){
    root = programa;
    returned = false;
    return_value = 0;
    env.add_level();
    cout << "Interprete:" << endl;
    programa->accept(this);
    env.remove_level();
    return;
}
int EVALVisitor::visit(Program* p) {
    for(auto g : p->vdlist)
        g->accept(this);

    for(auto f : p->fdlist){
        if(f->nombre == "main"){
            f->accept(this);
            break;
        }
    }
    return 0;
}

int EVALVisitor::visit(FunDec* fd) {
    env.add_level();
    returned = false;      // <-- reiniciamos
    return_value = 0;
    fd->body->accept(this);
    env.remove_level();
    return return_value;
}


int EVALVisitor::visit(GlobalVarDec* v) {
    int val = v->value->accept(this);
    env.add_var(v->name, val);
    return 0;
}

int EVALVisitor::visit(LocalVarDec* v) {
    int val = v->value ? v->value->accept(this) : 0;
    env.add_var(v->name, val);
    return 0;
}

int EVALVisitor::visit(Body* body) {
    env.add_level();
    for (auto d : body->declarations) {
        if (returned) break;
        d->accept(this);
    }
    for (auto s : body->StmList) {
        if (returned) break;
        s->accept(this);
    }
    env.remove_level();
    return 0;
}


int EVALVisitor::visit(BinaryExp* exp) {
    int result;
    int v1 = exp->left->accept(this);
    int v2 = exp->right->accept(this);
    switch (exp->op) {
        case PLUS_OP:
            result = v1 + v2;
            break;
        case MINUS_OP:
            result = v1 - v2;
            break;
        case MUL_OP:
            result = v1 * v2;
            break;
        case DIV_OP:
            if (v2 != 0)
                result = v1 / v2;
            else {
                cout << "Error: división por cero" << endl;
                result = 0;
            }
            break;
        case LT_OP:
            result = v1 < v2;
            break;
        case LE_OP:
            result = v1 <= v2;
            break;
        case GT_OP:
            result = v1 > v2;
            break;
        case GE_OP:
            result = v1 >= v2;
            break;
        case EQ_OP:
            result = v1 == v2;
            break;
        default:
            cout << "Operador desconocido" << endl;
            result = 0;
    }
    return result;
}

int EVALVisitor::visit(NumberExp* exp) {
    return exp->value;
}

int EVALVisitor::visit(IdExp* exp) {
    return env.lookup(exp->value);
}

int EVALVisitor::visit(FcallExp* exp) {
    FunDec* fun = nullptr;

    for (auto f : root->fdlist) {
        if (f->nombre == exp->nombre) {
            fun = f;
            break;
        }
    }

    if (!fun) {
        cout << "función no encontrada: " << exp->nombre << endl;
        return 0;
    }

    env.add_level();

    bool old_returned = returned;
    int  old_retval   = return_value;
    returned = false;
    return_value = 0;

    for (int i = 0; i < exp->argumentos.size(); i++) {
        int val = exp->argumentos[i]->accept(this);
        env.add_var(fun->Pnombres[i], val);
    }

    fun->body->accept(this);
    int ret = return_value;

    returned = old_returned;
    return_value = old_retval;

    env.remove_level();
    return ret;
}


int EVALVisitor::visit(AssignStm* stm) {
    int val = stm->value->accept(this);
    env.update(stm->var, val);
    return 0;
}
int EVALVisitor::visit(FcallStm* stm) {
    return stm->e->accept(this);;
}

int EVALVisitor::visit(PrintStm* stm) {
    cout << stm->e->accept(this) << endl;
    return 0;
}

int EVALVisitor::visit(ReturnStm* stm) {
    returned = true;

    if (stm->value)
        return_value = stm->value->accept(this);
    else
        return_value = 0;

    return return_value;
}


int EVALVisitor::visit(ForStm*stm) {
    int s = stm->start->accept(this);
    int f = stm->finish->accept(this);

    env.add_level();
    env.add_var(stm->var, s);

    for(int x=s; stm->equal ? x <= f : x < f; x++){
        env.update(stm->var, x);
        stm->body->accept(this);
    }

    env.remove_level();
    return 0;
}

int EVALVisitor::visit(WhileStm* stm) {
    while(stm->condition->accept(this)){
        stm->body->accept(this);
    }
    return 0;
}

int EVALVisitor::visit(IfStm* stm) {
    if(stm->condition->accept(this))
        stm->ifbody->accept(this);
    else if(stm->elsecond)
        stm->elsebody->accept(this);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////
//-------TYPECHECKER VISITOR------
int TypeCheckerVisitor::tipe(Program *program) {
    fun_locales.clear();

    for (auto f : program->fdlist)
        f->accept(this);

    return 0;
}

int TypeCheckerVisitor::visit(Program *p) {
    return 0;
}


int TypeCheckerVisitor::visit(GlobalVarDec *vd) {
    return 0;
}

int TypeCheckerVisitor::visit(LocalVarDec *vd) {
    locales += 1;
    return 0;
}


int TypeCheckerVisitor::visit(FunDec *fd) {
    int parametros = fd->Pnombres.size();
    locales = 0;
    fd->body->accept(this);
    fun_locales[fd->nombre] = parametros + locales;
    return 0;
}

int TypeCheckerVisitor::visit(Body *body) {
    for (auto i:body->declarations) {
        i->accept(this);
    }

    for(auto i:body->StmList) {
        i->accept(this);
    }
    return 0;
}

int TypeCheckerVisitor::visit(BinaryExp *exp) {
    exp->left->accept(this);
    exp->right->accept(this);
    return 0;
}

int TypeCheckerVisitor::visit(NumberExp *exp) {
    return 0;
}

int TypeCheckerVisitor::visit(IdExp *exp) {
    return 0;
}

int TypeCheckerVisitor::visit(FcallExp *fcall) {
    return 0;
}

int TypeCheckerVisitor::visit(AssignStm *stm) {
    stm->value->accept(this);
    return 0;
}

int TypeCheckerVisitor::visit(FcallStm *fcall) {
    return 0;
}

int TypeCheckerVisitor::visit(PrintStm *stm) {
    return 0;
}

int TypeCheckerVisitor::visit(ReturnStm *r) {
    r->value->accept(this);
    return 0;
}

int TypeCheckerVisitor::visit(ForStm *stm) {
    locales += 1;
    stm->body->accept(this);

    return 0;
}

int TypeCheckerVisitor::visit(WhileStm *stm) {
    stm->body->accept(this);
    return 0;
}

int TypeCheckerVisitor::visit(IfStm *stm) {
    int before = locales;

    int after_if = locales;
    stm->ifbody->accept(this);
    after_if = locales;

    int after_else = before;
    if (stm->elsecond) {
        stm->elsebody->accept(this);
        after_else = locales;
    }

    locales = max(after_if, after_else);

    return 0;
}


///////////////////////////////////////////////////////////////////////////////////
//-------GENCODE VISITOR------
GenCodeVisitor::~GenCodeVisitor() {
    delete memoria;
}

int GenCodeVisitor::generar(Program* program) {
    tipe.tipe(program);
    fun_reserva = tipe.fun_locales;
    program->accept(this);
    return 0;
}

int GenCodeVisitor::visit(Program* program) {
    out << ".data\nprint_fmt: .string \"%ld \\n\""<<endl;
    memoria->add_level();

    for (auto dec : program->vdlist){
        dec->accept(this);
    }

    for (auto& [var, val] : memoriaGlobal) {
        out << var << ": .quad " << val << endl;
    }

    out << ".text\n";
    
    for (auto dec : program->fdlist){
        dec->accept(this);
    }

    out << ".section .note.GNU-stack,\"\",@progbits"<<endl;
    
    memoria->remove_level();

    return 0;
    
}

int GenCodeVisitor::visit(FunDec* f) {
    entornoFuncion = true;
    memoria->add_level();
    offset = -8;
    nombreFuncion = f->nombre;
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    
    int vars = fun_reserva[f->nombre];
    int total = (vars + 1) * 8;

    if (total % 16 != 0) {
        total += 8;
    }
    
    out << ".globl " << f->nombre << endl;
    out << f->nombre <<  ":" << endl;
    out << " pushq %rbp" << endl;
    out << " movq %rsp, %rbp" << endl;
    out << " subq $" << total << ", %rsp\n";
    int size = f->Pnombres.size();
    for (int i = 0; i < size; i++) { //parametros
        memoria->add_var(f->Pnombres[i], offset);
        out << " movq " << argRegs[i] << ", " << offset << "(%rbp)\n";
        offset -=8;
    }
    
    if (f->body) f->body->accept(this);

    out << ".end_"<< f->nombre << ":"<< endl;
    out << "leave" << endl;
    out << "ret" << endl;

    memoria->remove_level();
    entornoFuncion = false;
    return 0;
}

int GenCodeVisitor::visit(GlobalVarDec* vd) {
    auto num = dynamic_cast<NumberExp*>(vd->value);
    int val = num->value;
    memoriaGlobal[vd->name] = val;
    return 0;
}

int GenCodeVisitor::visit(LocalVarDec* vd) {
    memoria->add_var(vd->name,offset);
    if(vd->value){
        vd->value->accept(this);
        out << " movq %rax, " << offset << "(%rbp)\n";
    }
    offset -= 8;
    return 0;
}

int GenCodeVisitor::visit(Body* b) {
    memoria->add_level();
    for (auto dec : b->declarations){
        dec->accept(this);
    }
    for (auto s : b->StmList){
        s->accept(this);
    }
    memoria->remove_level();
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* exp) {
    exp->left->accept(this);
    out << " pushq %rax\n";
    exp->right->accept(this);
    out << " movq %rax, %rcx\n popq %rax\n";

    switch (exp->op) {
        case PLUS_OP:  out << " addq %rcx, %rax\n"; break;
        case MINUS_OP: out << " subq %rcx, %rax\n"; break;
        case MUL_OP:   out << " imulq %rcx, %rax\n"; break;
        case DIV_OP:   out << " cqto\n"
                           << " idivq %rcx\n";
            break;
        case LT_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setl %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case LE_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setle %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case GT_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setg %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case GE_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " setge %al\n"
                      << " movzbq %al, %rax\n";
            break;
        case EQ_OP:
            out << " cmpq %rcx, %rax\n"
                      << " movl $0, %eax\n"
                      << " sete %al\n"
                      << " movzbq %al, %rax\n";
            break;
    }
    return 0;
}

int GenCodeVisitor::visit(NumberExp* exp) {
    out << " movq $" << exp->value << ", %rax"<<endl;
    return 0;
}

int GenCodeVisitor::visit(IdExp* exp) {
    if (memoriaGlobal.count(exp->value))
        out << " movq " << exp->value << "(%rip), %rax"<<endl;
    else
        out << " movq " << memoria->lookup(exp->value) << "(%rbp), %rax"<<endl;
    return 0;
}

int GenCodeVisitor::visit(FcallExp* exp) {
    vector<std::string> argRegs = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    int size = exp->argumentos.size();
    for (int i = 0; i < size; i++) {
        exp->argumentos[i]->accept(this);
        out << " movq %rax, " << argRegs[i] <<endl;
    }
    out << "call " << exp->nombre << endl;
    return 0;
}

int GenCodeVisitor::visit(AssignStm* stm) {
    stm->value->accept(this);
    if (memoriaGlobal.count(stm->var))
        out << " movq %rax, " << stm->var << "(%rip)"<<endl;
    else
        out << " movq %rax, " << memoria->lookup(stm->var) << "(%rbp)"<<endl;
    return 0;
}

int GenCodeVisitor::visit(FcallStm* stm) {
    stm->e->accept(this);
    return 0;
}

int GenCodeVisitor::visit(PrintStm* stm) {
    stm->e->accept(this);
    out <<
        " movq %rax, %rsi\n"
        " leaq print_fmt(%rip), %rdi\n"
        " movl $0, %eax\n"
        " call printf@PLT\n";

    return 0;
}

int GenCodeVisitor::visit(ReturnStm* stm) {
    stm->value->accept(this);
    out << " jmp .end_"<<nombreFuncion << endl;
    return 0;
}

int GenCodeVisitor::visit(ForStm* stm) {
    int label = labelcont++;
    memoria->add_level();
    memoria->add_var(stm->var,offset);
    int slot = offset;
    offset -= 8;

    stm->start->accept(this);
    out << " movq %rax, " << slot << "(%rbp)\n";
    out << "for_" << label << ":"<<endl;
    
    stm->finish->accept(this);
    out << " movq %rax, %rcx" <<endl;
    out << " movq " << slot << "(%rbp), %rax" <<endl;
    out << " cmpq %rcx, %rax" << endl;
    out << " movl $0, %eax" << endl;
    if (stm->equal)
        out << " setle %al" << endl;
    else
        out << " setl %al" << endl;
    out << " movzbq %al, %rax" << endl;
    out << " cmpq $0, %rax" << endl;
    out << " je endfor_" << label << endl;
    
    stm->body->accept(this);
    out << " movq " << slot << "(%rbp), %rax" <<endl;
    out << " addq $1, %rax" << endl;
    out << " movq %rax, " << slot << "(%rbp)" << endl;

    out << " jmp for_" << label << endl;
    out << "endfor_" << label << ":" << endl;
    memoria->remove_level();
    return 0;
}

int GenCodeVisitor::visit(WhileStm* stm) {
    int label = labelcont++;
    out << "while_" << label << ":"<<endl;
    stm->condition->accept(this);
    out << " cmpq $0, %rax" << endl;
    out << " je endwhile_" << label << endl;
    stm->body->accept(this);
    out << " jmp while_" << label << endl;
    out << "endwhile_" << label << ":"<< endl;
    return 0;
}


int GenCodeVisitor::visit(IfStm* stm) {
    int label = labelcont++;
    stm->condition->accept(this);
    out << " cmpq $0, %rax"<<endl;
    out << " je else_" << label << endl;
    int a = offset;
    stm->ifbody->accept(this);
    out << " jmp endif_" << label << endl;
    out << " else_" << label << ":"<< endl;
    offset = a;
    if (stm->elsecond) stm->elsebody->accept(this);
    out << "endif_" << label << ":"<< endl;
    return 0;
}

