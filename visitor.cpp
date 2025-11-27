#include "visitor.h"
#include "ast.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <cstdint>

using namespace std;

static int countLocalsInBody(Body* b) {
    if (!b) return 0;
    int c = 0;

    for (auto d : b->declarations) c++;

    for (auto s : b->StmList) {
        if (auto f = dynamic_cast<ForStm*>(s)) {
            c++;
            c += countLocalsInBody(f->body);
        } else if (auto w = dynamic_cast<WhileStm*>(s)) {
            c += countLocalsInBody(w->body);
        } else if (auto i = dynamic_cast<IfStm*>(s)) {
            c += countLocalsInBody(i->ifbody);
            if (i->elsecond && i->elsebody) {
                c += countLocalsInBody(i->elsebody);
            }
        }
    }
    return c;
}

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

int StructDec::accept(Visitor* visitor) {
    return visitor->visit(this); 
}
int StructInitExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int ArrayExp::accept(Visitor* visitor) {
    return visitor->visit(this); 
}
int ArrayAccessExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int StringExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int FieldAccessExp::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int StructStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int ArrayStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}
int BlockStm::accept(Visitor* visitor) {
    return visitor->visit(this);
}

int Visitor::visit(BlockStm* s) {
    s->body->accept(this);
    return 0;
}
int TypeCheckerVisitor::visit(BlockStm* s) {
    s->body->accept(this);
    return 0;
}
int GenCodeVisitor::visit(BlockStm* s) {
    s->body->accept(this);
    return 0;
}

TypeInfo* TypeCheckerVisitor::convert(TypeKind k) {
    return new TypeInfo(k);
}

bool TypeCheckerVisitor::same(TypeInfo* a, TypeInfo* b) {
    if (a == b) return true;
    if (!a || !b) return false;

    if (a->kind == T_I32 && b->kind == T_I64) return true;
    if (a->kind == T_I64 && b->kind == T_I32) return true;

    if (a->kind != b->kind) return false;

    switch (a->kind) {
        case T_I32:
        case T_I64:
        case T_BOOL:
        case T_STRING:
            return true;
        case T_STRUCT:
            return a->structName == b->structName;
        case T_ARRAY:
            return same(a->elem, b->elem) && a->size == b->size;
        default:
            return true;
    }
}

bool TypeCheckerVisitor::numeric(TypeInfo* a) {
    return a && (a->kind == T_I32 || a->kind == T_I64);
}

TypeInfo* TypeCheckerVisitor::makeSame(TypeInfo* a, TypeInfo* b) {
    if (!numeric(a) || !numeric(b)) {
        throw runtime_error("operación aritmética con tipo no numérico");
    }

    if (a->kind == T_I32 && b->kind == T_I32)
        return convert(T_I32);

    return convert(T_I64);
}

TypeInfo* TypeCheckerVisitor::get(Exp* e) {
    auto it = inferred.find(e);
    if (it == inferred.end()) {
        throw runtime_error("expresión sin tipo inferido");
    }
    return it->second;
}

int TypeCheckerVisitor::typeCheck(Program* p) {
    tenv.clear();
    tenv.add_level();
    structs.clear();
    funcs.clear();
    inferred.clear();
    currentFun = "";
    expectedReturn = nullptr;
    localCount = 0;
    funLocalCount.clear();

    for (auto sd : p->stlist) {
        sd->accept(this);
    }

    for (auto fd : p->fdlist) {
        FunInfo fi;
        fi.ret = fd->returnType;
        fi.params = fd->Ptipos;
        funcs[fd->nombre] = fi;
    }

    for (auto gv : p->vdlist) {
        gv->accept(this);
    }

    for (auto fd : p->fdlist) {
        fd->accept(this);
    }

    return 0;
}

int TypeCheckerVisitor::visit(Program* p) {
    return 0;
}

int TypeCheckerVisitor::visit(StructDec* sd) {
    StructInfo info;
    info.fields = sd->fields;
    info.types  = sd->tipos;
    info.sizeBytes = (int)sd->fields.size() * 8;

    for (int i = 0; i < (int)sd->fields.size(); i++) {
        info.index[sd->fields[i]] = i;
    }
    structs[sd->nombre] = info;
    return 0;
}

int TypeCheckerVisitor::visit(GlobalVarDec* vd) {
    if (vd->value) {
        vd->value->accept(this);
        TypeInfo* vt = get(vd->value);

        if (vt->kind == T_UNKNOWN && vd->type && vd->type->kind != T_UNKNOWN) {
            inferred[vd->value] = vd->type;
            vd->value->inferredType = vd->type;
        } else if (vd->type && vd->type->kind != T_UNKNOWN) {
            if (!same(vd->type, vt)) {
                throw runtime_error("tipo del inicializador global '" + vd->name + "' incompatible");
            }
        } else {
            vd->type = new TypeInfo(*vt);
        }
    } else {
        if (!vd->type || vd->type->kind == T_UNKNOWN) {
            throw runtime_error("variable global sin tipo ni valor: " + vd->name);
        }
    }

    tenv.add_var(vd->name, vd->type);
    return 0;
}

int TypeCheckerVisitor::visit(LocalVarDec* vd) {
    if ((!vd->type || vd->type->kind == T_UNKNOWN) && !vd->value) {
        throw runtime_error(
            "variable local '" + vd->name + "' sin tipo ni inicializador"
        );
    }

    if (vd->value)
        vd->value->accept(this);

    TypeInfo* vt = vd->value ? get(vd->value) : nullptr;

    if (vd->value && vt->kind == T_UNKNOWN &&
        vd->type && vd->type->kind != T_UNKNOWN) {

        inferred[vd->value] = vd->type;
        vd->value->inferredType = vd->type;
    } else if (vd->value && vd->type && vd->type->kind != T_UNKNOWN) {
        if (!same(vd->type, vt)) {
            throw runtime_error("tipo incompatible en inicialización de '" + vd->name + "'");
        }
    } else if (!vd->type || vd->type->kind == T_UNKNOWN) {
        vd->type = new TypeInfo(*vt);
    }

    tenv.add_var(vd->name, vd->type);
    localCount++;

    return 0;
}

int TypeCheckerVisitor::visit(FunDec* f) {
    currentFun = f->nombre;
    tenv.add_level();
    localCount = 0;

    int paramCount = (int)f->Pnombres.size();
    for (int i = 0; i < paramCount; i++) {
        tenv.add_var(f->Pnombres[i], f->Ptipos[i]);
    }

    expectedReturn = f->returnType;
    if (expectedReturn && expectedReturn->kind == T_UNKNOWN) {
        expectedReturn = nullptr;
    }

    f->body->accept(this);

    if (!f->returnType) {
        f->returnType = expectedReturn;
    }

    funLocalCount[f->nombre] = paramCount + localCount;

    tenv.remove_level();
    currentFun = "";
    expectedReturn = nullptr;
    return 0;
}

int TypeCheckerVisitor::visit(Body* body) {
    tenv.add_level();
    for (auto i:body->declarations) {
        i->accept(this);
    }

    for(auto i:body->StmList) {
        i->accept(this);
    }
    tenv.remove_level();
    return 0;
}


int TypeCheckerVisitor::visit(NumberExp* e) {
    if (e->value >= INT32_MIN && e->value <= INT32_MAX) {
        TypeInfo* t = convert(T_I32);
        inferred[e] = t;
        e->inferredType = t;
        return 0;
    }

    TypeInfo* t = convert(T_I64);
    inferred[e] = t;
    e->inferredType = t;
    return 0;
}

int TypeCheckerVisitor::visit(StringExp* e) {
    TypeInfo* t = convert(T_STRING);
    inferred[e] = t;
    e->inferredType = t;
    return 0;
}

int TypeCheckerVisitor::visit(IdExp* e) {
    TypeInfo* t = nullptr;
    if (!tenv.lookup(e->value, t)) {
        throw runtime_error("identificador no declarado: " + e->value);
    }
    inferred[e] = t;
    e->inferredType = t;
    return 0;
}

int TypeCheckerVisitor::visit(BinaryExp* e) {
    e->left->accept(this);
    e->right->accept(this);
    TypeInfo* tl = get(e->left);
    TypeInfo* tr = get(e->right);

    switch (e->op) {
        case PLUS_OP:
        case MINUS_OP:
        case MUL_OP:
        case DIV_OP: {
            TypeInfo* t = makeSame(tl, tr);
            inferred[e] = t;
            e->inferredType = t;
            break;
        }
        default: {
            (void)makeSame(tl, tr);
            TypeInfo* t = convert(T_BOOL);
            inferred[e] = t;
            e->inferredType = t;
            break;
        }
    }
    return 0;
}

int TypeCheckerVisitor::visit(FcallExp* f) {
    auto it = funcs.find(f->nombre);
    if (it == funcs.end()) {
        throw runtime_error("función no declarada: " + f->nombre);
    }
    FunInfo& info = it->second;

    if (f->argumentos.size() != info.params.size()) {
        throw runtime_error("número de argumentos incorrecto en llamada a " + f->nombre);
    }

    for (size_t i = 0; i < f->argumentos.size(); i++) {
        f->argumentos[i]->accept(this);
        TypeInfo* at = get(f->argumentos[i]);
        if (!same(at, info.params[i])) {
            throw runtime_error("tipo de argumento " + to_string(i) + " incorrecto en " + f->nombre);
        }
    }

    TypeInfo* ret = info.ret ? info.ret : convert(T_I64);
    inferred[f] = ret;
    f->inferredType = ret;
    return 0;
}

int TypeCheckerVisitor::visit(StructInitExp* s) {
    auto it = structs.find(s->nombre);
    if (it == structs.end()) {
        throw runtime_error("struct no declarado: " + s->nombre);
    }
    StructInfo& info = it->second;

    if (s->fields.size() != info.fields.size()) {
        throw runtime_error("cantidad de campos incorrecta al inicializar struct " + s->nombre);
    }

    for (size_t i = 0; i < s->fields.size(); i++) {
        const string& fname = s->fields[i];
        auto fi = info.index.find(fname);
        if (fi == info.index.end()) {
            throw runtime_error("campo " + fname + " no existe en struct " + s->nombre);
        }
        int idx = fi->second;
        TypeInfo* expected = info.types[idx];

        s->values[i]->accept(this);
        TypeInfo* tv = get(s->values[i]);

        if (!same(tv, expected)) {
            throw runtime_error("tipo incorrecto en campo " + fname + " de struct " + s->nombre);
        }
    }

    TypeInfo* t = new TypeInfo(s->nombre);
    inferred[s] = t;
    s->inferredType = t;
    return 0;
}

int TypeCheckerVisitor::visit(ArrayExp* a) {
    if (a->elems.empty()) {
        TypeInfo* elem = convert(T_UNKNOWN);
        TypeInfo* t = new TypeInfo(T_ARRAY, elem, 0);
        inferred[a] = t;
        a->inferredType = t;
        return 0;
    }

    a->elems[0]->accept(this);
    TypeInfo* base = get(a->elems[0]);

    for (size_t i = 1; i < a->elems.size(); i++) {
        a->elems[i]->accept(this);
        TypeInfo* ti = get(a->elems[i]);
        if (!same(ti, base)) {
            throw runtime_error("elementos de array con tipos incompatibles");
        }
    }

    TypeInfo* t = new TypeInfo(T_ARRAY, base, (int)a->elems.size());
    inferred[a] = t;
    a->inferredType = t;
    return 0;
}

int TypeCheckerVisitor::visit(ArrayAccessExp* a) {
    a->arr->accept(this);
    a->index->accept(this);

    TypeInfo* ta = get(a->arr);
    TypeInfo* ti = get(a->index);

    if (!ta || ta->kind != T_ARRAY) {
        throw runtime_error("indexación [] sobre valor no array");
    }
    if (!numeric(ti)) {
        throw runtime_error("índice de array no numérico");
    }

    TypeInfo* t = ta->elem;
    inferred[a] = t;
    a->inferredType = t;
    return 0;
}

int TypeCheckerVisitor::visit(FieldAccessExp* f) {
    f->obj->accept(this);
    TypeInfo* tobj = get(f->obj);

    if (!tobj || tobj->kind != T_STRUCT) {
        throw runtime_error("acceso a campo sobre valor no struct");
    }

    auto sit = structs.find(tobj->structName);
    if (sit == structs.end()) {
        throw runtime_error("struct no registrado: " + tobj->structName);
    }

    StructInfo& info = sit->second;
    auto fi = info.index.find(f->field);
    if (fi == info.index.end()) {
        throw runtime_error("campo " + f->field + " no existe en struct " + tobj->structName);
    }

    int idx = fi->second;
    TypeInfo* t = info.types[idx];
    inferred[f] = t;
    f->inferredType = t;
    return 0;
}

int TypeCheckerVisitor::visit(AssignStm* s) {
    s->left->accept(this);
    s->value->accept(this);
    TypeInfo* tl = get(s->left);
    TypeInfo* tv = get(s->value);
    if (!same(tl, tv)) {
        throw runtime_error("tipos incompatibles en asignación");
    }
    return 0;
}

int TypeCheckerVisitor::visit(FcallStm* s) {
    s->e->accept(this);
    return 0;
}

int TypeCheckerVisitor::visit(PrintStm* s) {
    if (s->pre)
        s->pre->accept(this);

    for (auto arg : s->args)
        arg->accept(this);

    return 0;
}


int TypeCheckerVisitor::visit(ReturnStm* r) {
    r->value->accept(this);
    TypeInfo* rt = get(r->value);

    if (!expectedReturn) {
        expectedReturn = rt;
    } else if (!same(expectedReturn, rt)) {
        throw runtime_error("tipo de return inconsistente en función " + currentFun);
    }
    return 0;
}

int TypeCheckerVisitor::visit(StructStm* s) {
    FieldAccessExp tmp(s->object, s->field);
    tmp.accept(this);
    s->value->accept(this);

    TypeInfo* tl = get(&tmp);
    TypeInfo* tv = get(s->value);
    if (!same(tl, tv)) {
        throw runtime_error("asignación a campo struct con tipo incompatible");
    }
    return 0;
}

int TypeCheckerVisitor::visit(ArrayStm* s) {
    ArrayAccessExp tmp(s->array, s->index);
    tmp.accept(this);
    s->value->accept(this);

    TypeInfo* tl = get(&tmp);
    TypeInfo* tv = get(s->value);
    if (!same(tl, tv)) {
        throw runtime_error("asignación a elemento de array con tipo incompatible");
    }
    return 0;
}

int TypeCheckerVisitor::visit(ForStm* f) {
    tenv.add_level();
    TypeInfo* t = convert(T_I64);
    tenv.add_var(f->var, t);
    localCount++;

    f->start->accept(this);
    f->finish->accept(this);
    TypeInfo* ts = get(f->start);
    TypeInfo* tf = get(f->finish);
    (void)makeSame(ts, tf);

    f->body->accept(this);

    tenv.remove_level();
    return 0;
}

int TypeCheckerVisitor::visit(WhileStm* w) {
    w->condition->accept(this);
    TypeInfo* tc = get(w->condition);
    if (!numeric(tc) && (!tc || tc->kind != T_BOOL)) {
        throw runtime_error("condición de while no es booleana/numérica");
    }

    w->body->accept(this);
    return 0;
}

int TypeCheckerVisitor::visit(IfStm* i) {
    i->condition->accept(this);
    TypeInfo* tc = get(i->condition);
    if (!numeric(tc) && (!tc || tc->kind != T_BOOL)) {
        throw runtime_error("condición de if no es booleana/numérica");
    }
    tenv.add_level();
    i->ifbody->accept(this);
    tenv.remove_level();

    if (i->elsecond && i->elsebody) {
        tenv.add_level();
        i->elsebody->accept(this);
        tenv.remove_level();
    }
    return 0;
}

GenCodeVisitor::GenCodeVisitor(ostream& o)
    : out(o),
      strCount(0),
      currentFun(""),
      offset(0),
      labelCount(0) {}

void GenCodeVisitor::buildStructLayouts(Program* p) {
    layouts.clear();
    for (auto sd : p->stlist) {
        StructLayout lay;
        lay.sizeBytes = (int)sd->fields.size() * 8;
        for (int i = 0; i < (int)sd->fields.size(); i++) {
            lay.fieldOffset[sd->fields[i]] = i * 8;
        }
        layouts[sd->nombre] = lay;
    }
}

void GenCodeVisitor::getAddr(Exp* e) {
    if (auto id = dynamic_cast<IdExp*>(e)) {
        int off;
        if (envStack.lookup(id->value, off)) {
            out << " leaq " << off << "(%rbp), %rax\n";
        } else {
            out << " leaq " << id->value << "(%rip), %rax\n";
        }
        return;
    }

    if (auto aa = dynamic_cast<ArrayAccessExp*>(e)) {
        aa->arr->accept(this);
        out << " movq %rax, %rdx\n";
        aa->index->accept(this);
        out << " imulq $8, %rax\n";
        out << " addq %rax, %rdx\n";
        out << " movq %rdx, %rax\n";
        return;
    }

    if (auto fa = dynamic_cast<FieldAccessExp*>(e)) {
        fa->obj->accept(this);
        out << " movq %rax, %rdx\n";

        TypeInfo* tobj = tipe.get(fa->obj);
        auto it = layouts.find(tobj->structName);
        if (it == layouts.end()) {
            throw runtime_error("layout no encontrado para struct en getAddr");
        }
        int off = it->second.fieldOffset[fa->field];

        out << " leaq " << off << "(%rdx), %rax\n";
        return;
    }

    throw runtime_error("lvalue inválido en getAddr");
}


int GenCodeVisitor::generate(Program* p) {
    tipe.typeCheck(p);
    buildStructLayouts(p);
    constEnv.clear();
    constEnv.push();

    out << ".data\n";
    out << "print_int_fmt: .string \"%ld\\n\"\n";
    out << "print_str_fmt: .string \"%s\\n\"\n";

    for (auto g : p->vdlist) {
        g->accept(this);
    }

    out << ".text\n";

    for (auto f : p->fdlist) {
        f->accept(this);
    }

    out << ".section .note.GNU-stack,\"\",@progbits\n";
    return 0;
}

int GenCodeVisitor::visit(Program* p) {
    return 0;
}

int GenCodeVisitor::visit(GlobalVarDec* vd) {
    constEnv.declare(vd->name);

    if (!vd->type || vd->type->kind != T_ARRAY) {
        long val = 0;
        if (vd->value) {
            ConstVisitor cv(&constEnv);
            vd->value->accept(&cv);
            if (!vd->value->cont) {
                throw runtime_error("inicializador global no constante para '" + vd->name + "'");
            }
            val = vd->value->valor;
        }
        out << vd->name << ": .quad " << val << "\n";
        return 0;
    }

    ArrayExp* arr = dynamic_cast<ArrayExp*>(vd->value);
    if (!arr) {
        throw runtime_error("inicializador global de array no es ArrayExp para '" + vd->name + "'");
    }

    out << vd->name << ":\n";
    for (size_t i = 0; i < arr->elems.size(); i++) {
        ConstVisitor cv(&constEnv);
        arr->elems[i]->accept(&cv);
        if (!arr->elems[i]->cont) {
            throw runtime_error("elemento " + to_string(i) + " de array global '" + vd->name + "' no es constante");
        }
        out << " .quad " << arr->elems[i]->valor << "\n";
    }
    return 0;
}

int GenCodeVisitor::visit(StructDec* sd) {
    return 0;
}

int GenCodeVisitor::visit(FunDec* f) {
    currentFun = f->nombre;
    envStack.add_level();
    offset = -8;

    out << ".globl " << f->nombre << "\n";
    out << f->nombre << ":\n";
    out << " pushq %rbp\n";
    out << " movq %rsp, %rbp\n";

    int locals = countLocalsInBody(f->body);
    int params = (int)f->Pnombres.size();
    int total = (locals + params)*8;

    if (total % 16 != 0) total += 8;

    if (total > 0) {
        out << " subq $" << total << ", %rsp\n";
    }

    vector<string> argRegs = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };
    for (int i = 0; i < params && i < 6; i++) {
        envStack.add_var(f->Pnombres[i], offset);
        out << " movq " << argRegs[i] << ", " << offset << "(%rbp)\n";
        offset -= 8;
    }

    f->body->accept(this);

    out << ".Lend_" << f->nombre << ":\n";
    out << " leave\n";
    out << " ret\n";

    envStack.remove_level();
    currentFun = "";
    return 0;
}

int GenCodeVisitor::visit(LocalVarDec* vd) {
    envStack.add_var(vd->name, offset);
    constEnv.declare(vd->name);

    if (vd->value) {
        ConstVisitor cv(&constEnv);
        vd->value->accept(&cv);

        if (vd->value->cont == 1) {
            constEnv.setConst(vd->name, vd->value->valor);
            out << " movq $" << vd->value->valor << ", %rax\n";
        } else {
            constEnv.unsetConst(vd->name);
            vd->value->accept(this);
        }

        out << " movq %rax, " << offset << "(%rbp)\n";
    } else {
        constEnv.unsetConst(vd->name);
    }

    offset -= 8;
    return 0;
}


int GenCodeVisitor::visit(Body* body) {
    envStack.add_level();

    struct Node {
        int order;
        LocalVarDec* d;
        Stm* s;
    };

    vector<Node> merged;

    for (auto d : body->declarations) {
        Node n;
        n.order = d->order;
        n.d = d;
        n.s = nullptr;
        merged.push_back(n);
    }

    for (auto s : body->StmList) {
        Node n;
        n.order = s->order;
        n.d = nullptr;
        n.s = s;
        merged.push_back(n);
    }

    sort(merged.begin(), merged.end(),
         [](const Node& a, const Node& b) {
             return a.order < b.order;
         });

    for (auto& n : merged) {
        if (n.d) n.d->accept(this);
        else if (n.s) n.s->accept(this);
    }

    envStack.remove_level();
    return 0;
}

int GenCodeVisitor::visit(NumberExp* e) {
    out << " movq $" << e->value << ", %rax\n";
    return 0;
}

int GenCodeVisitor::visit(StringExp* s) {
    string lbl;
    auto it = strLabels.find(s->s);
    if (it == strLabels.end()) {
        lbl = ".LC" + to_string(strCount++);
        strLabels[s->s] = lbl;
        out << ".section .rodata\n";
        out << lbl << ": .string \"" << s->s << "\"\n";
        out << ".text\n";
    } else {
        lbl = it->second;
    }
    out << " leaq " << lbl << "(%rip), %rax\n";
    return 0;
}

int GenCodeVisitor::visit(IdExp* e) {
    long v;
    if (constEnv.getConst(e->value, v)) {
        out << " movq $" << v << ", %rax\n";
        return 0;
    }
    int off;
    if (envStack.lookup(e->value, off))
        out << " movq " << off << "(%rbp), %rax\n";
    else
        out << " movq " << e->value << "(%rip), %rax\n";
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* b) {
    ConstVisitor cv(&constEnv);
    b->accept(&cv);

    if (b->cont == 1) {
        out << " movq $" << b->valor << ", %rax\n";
        return 0;
    }

    b->left->accept(this);
    out << " pushq %rax\n";
    b->right->accept(this);
    out << " movq %rax, %rcx\n";
    out << " popq %rax\n";

    switch (b->op) {
        case PLUS_OP:
            out << " addq %rcx, %rax\n";
            break;
        case MINUS_OP:
            out << " subq %rcx, %rax\n";
            break;
        case MUL_OP:
            out << " imulq %rcx, %rax\n";
            break;
        case DIV_OP:
            out << " cqto\n";
            out << " idivq %rcx\n";
            break;
        default:
            out << " cmpq %rcx, %rax\n";
            if (b->op == LT_OP)       out << " setl %al\n";
            else if (b->op == LE_OP)  out << " setle %al\n";
            else if (b->op == GT_OP)  out << " setg %al\n";
            else if (b->op == GE_OP)  out << " setge %al\n";
            else if (b->op == EQ_OP)  out << " sete %al\n";
            out << " movzbq %al, %rax\n";
            break;
    }
    return 0;
}

int GenCodeVisitor::visit(FcallExp* f) {
    vector<string> argRegs = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };

    for (size_t i = 0; i < f->argumentos.size() && i < argRegs.size(); i++) {
        f->argumentos[i]->accept(this);
        out << " movq %rax, " << argRegs[i] << "\n";
    }

    out << " call " << f->nombre << "\n";
    return 0;
}

int GenCodeVisitor::visit(ArrayExp* a) {
    int n = (int)a->elems.size();
    int bytes = n * 8;

    out << " movq $" << bytes << ", %rdi\n";
    out << " call malloc@PLT\n";
    out << " movq %rax, %r10\n";

    for (int i = 0; i < n; i++) {
        a->elems[i]->accept(this);
        out << " movq %rax, " << (i * 8) << "(%r10)\n";
    }

    out << " movq %r10, %rax\n";
    return 0;
}

int GenCodeVisitor::visit(ArrayAccessExp* a) {
    a->arr->accept(this);
    out << " movq %rax, %rdx\n";
    a->index->accept(this);
    out << " imulq $8, %rax\n";
    out << " addq %rax, %rdx\n";
    out << " movq (%rdx), %rax\n";
    return 0;
}

int GenCodeVisitor::visit(StructInitExp* s) {
    auto it = layouts.find(s->nombre);
    if (it == layouts.end()) {
        throw runtime_error("layout no encontrado para struct en StructInitExp");
    }
    StructLayout& lay = it->second;

    out << " movq $" << lay.sizeBytes << ", %rdi\n";
    out << " call malloc@PLT\n";
    out << " movq %rax, %r11\n";

    for (size_t i = 0; i < s->fields.size(); i++) {
        int off = lay.fieldOffset[s->fields[i]];
        s->values[i]->accept(this);
        out << " movq %rax, " << off << "(%r11)\n";
    }

    out << " movq %r11, %rax\n";
    return 0;
}



int GenCodeVisitor::visit(FieldAccessExp* f) {
    f->obj->accept(this);
    out << " movq %rax, %rdx\n";
    TypeInfo* tobj = tipe.get(f->obj);
    auto it = layouts.find(tobj->structName);
    if (it == layouts.end()) {
        throw runtime_error("layout no encontrado para struct en FieldAccessExp");
    }
    int off = it->second.fieldOffset[f->field];
    out << " movq " << off << "(%rdx), %rax\n";
    return 0;
}

static bool getVarName(Exp* e, string& outName) {
    if (auto id = dynamic_cast<IdExp*>(e)) {
        outName = id->value;
        return true;
    }
    return false;
}

int GenCodeVisitor::visit(AssignStm* s) {
    ConstVisitor cv(&constEnv);
    s->value->accept(&cv);

    string var;
    bool flag = getVarName(s->left, var);

    if (flag && s->value->cont == 1) {
        constEnv.setConst(var, s->value->valor);
        out << " movq $" << s->value->valor << ", %rax\n";
    } else {
        if (flag) constEnv.unsetConst(var);
        s->value->accept(this);
    }

    out << " pushq %rax\n";
    getAddr(s->left);
    out << " movq %rax, %rdx\n";
    out << " popq %rax\n";
    out << " movq %rax, (%rdx)\n";

    return 0;
}


int GenCodeVisitor::visit(FcallStm* s) {
    s->e->accept(this);
    return 0;
}

int GenCodeVisitor::visit(PrintStm* s) {
    auto print = [&](Exp* e) {
        e->accept(this);

        TypeInfo* t = nullptr;
        try {
            t = tipe.get(e);
        } catch (...) {
            t = nullptr;
        }

        out << " movq %rax, %rsi\n";

        if (t && t->kind == T_STRING) {
            out << " leaq print_str_fmt(%rip), %rdi\n";
        } else {
            out << " leaq print_int_fmt(%rip), %rdi\n";
        }

        out << " movl $0, %eax\n";
        out << " call printf@PLT\n";
    };

    if (s->args.empty()) {
        print(s->pre);
        return 0;
    }

    for (auto arg : s->args) {
        print(arg);
    }

    return 0;
}


int GenCodeVisitor::visit(ReturnStm* r) {
    r->value->accept(this);
    out << " jmp .Lend_" << currentFun << "\n";
    return 0;
}

int GenCodeVisitor::visit(ForStm* f) {
    int lbl = labelCount++;

    int slot = offset;
    envStack.add_var(f->var, offset);
    offset -= 8;

    f->start->accept(this);
    out << " movq %rax, " << slot << "(%rbp)\n";

    out << ".Lfor_" << lbl << ":\n";
    
    f->finish->accept(this);
    out << " movq %rax, %rcx\n";
    out << " movq " << slot << "(%rbp), %rax\n";
    out << " cmpq %rcx, %rax\n";
    if (f->equal) {
        out << " jg .Lendfor_" << lbl << "\n";
    } else {
        out << " jge .Lendfor_" << lbl << "\n";
    }

    f->body->accept(this);

    out << " movq " << slot << "(%rbp), %rax\n";
    out << " addq $1, %rax\n";
    out << " movq %rax, " << slot << "(%rbp)\n";
    out << " jmp .Lfor_" << lbl << "\n";
    out << ".Lendfor_" << lbl << ":\n";
    return 0;
}

int GenCodeVisitor::visit(WhileStm* w) {
    int lbl = labelCount++;

    out << ".Lwhile_" << lbl << ":\n";
    w->condition->accept(this);
    out << " cmpq $0, %rax\n";
    out << " je .Lendwhile_" << lbl << "\n";
    w->body->accept(this);
    out << " jmp .Lwhile_" << lbl << "\n";
    out << ".Lendwhile_" << lbl << ":\n";
    return 0;
}

int GenCodeVisitor::visit(IfStm* i) {
    ConstVisitor cv(&constEnv);
    i->condition->accept(&cv);

    if (i->condition->cont == 1) {

        constEnv.push();

        if (i->condition->valor != 0) {
            i->ifbody->accept(this);
        } else if (i->elsebody) {
            i->elsebody->accept(this);
        }

        constEnv.pop();
        return 0;
    }

    int lbl = labelCount++;

    i->condition->accept(this);
    out << " cmpq $0, %rax\n";
    out << " je .Lelse_" << lbl << "\n";

    constEnv.push();
    i->ifbody->accept(this);
    constEnv.pop();

    out << " jmp .Lendif_" << lbl << "\n";
    out << ".Lelse_" << lbl << ":\n";

    if (i->elsebody) {
        constEnv.push();
        i->elsebody->accept(this);
        constEnv.pop();
    }

    out << ".Lendif_" << lbl << ":\n";
    return 0;
}


int GenCodeVisitor::visit(StructStm* s) {
    s->value->accept(this);
    out << " pushq %rax\n";
    FieldAccessExp fa(s->object, s->field);
    getAddr(&fa);
    out << " movq %rax, %rdx\n";
    out << " popq %rax\n";
    out << " movq %rax, (%rdx)\n";
    return 0;
}


int GenCodeVisitor::visit(ArrayStm* s) {
    ArrayAccessExp aa(s->array, s->index);
    s->value->accept(this);
    out << " pushq %rax\n";
    getAddr(&aa);
    out << " movq %rax, %rdx\n";
    out << " popq %rax\n";
    out << " movq %rax, (%rdx)\n";
    return 0;
}
