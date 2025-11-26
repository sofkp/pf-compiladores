#include "visitor.h"
#include "ast.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// Cuenta locales (LocalVarDec + variables de for) de un Body recursivamente
static int countLocalsInBody(Body* b) {
    if (!b) return 0;
    int c = 0;

    for (auto d : b->declarations) c++;

    for (auto s : b->StmList) {
        if (auto f = dynamic_cast<ForStm*>(s)) {
            c++; // variable del for
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

int Program::accept(Visitor* v)      { return v->visit(this); }
int FunDec::accept(Visitor* v)       { return v->visit(this); }
int StructDec::accept(Visitor* v)    { return v->visit(this); }
int GlobalVarDec::accept(Visitor* v) { return v->visit(this); }
int LocalVarDec::accept(Visitor* v)  { return v->visit(this); }
int Body::accept(Visitor* v)         { return v->visit(this); }

int BinaryExp::accept(Visitor* v)        { return v->visit(this); }
int NumberExp::accept(Visitor* v)        { return v->visit(this); }
int IdExp::accept(Visitor* v)            { return v->visit(this); }
int FcallExp::accept(Visitor* v)         { return v->visit(this); }
int StructInitExp::accept(Visitor* v)    { return v->visit(this); }
int ArrayExp::accept(Visitor* v)         { return v->visit(this); }
int ArrayAccessExp::accept(Visitor* v)   { return v->visit(this); }
int StringExp::accept(Visitor* v)        { return v->visit(this); }
int FieldAccessExp::accept(Visitor* v)   { return v->visit(this); }

int AssignStm::accept(Visitor* v)    { return v->visit(this); }
int FcallStm::accept(Visitor* v)     { return v->visit(this); }
int PrintStm::accept(Visitor* v)     { return v->visit(this); }
int ReturnStm::accept(Visitor* v)    { return v->visit(this); }
int StructStm::accept(Visitor* v)    { return v->visit(this); }
int ArrayStm::accept(Visitor* v)     { return v->visit(this); }
int ForStm::accept(Visitor* v)       { return v->visit(this); }
int WhileStm::accept(Visitor* v)     { return v->visit(this); }
int IfStm::accept(Visitor* v)        { return v->visit(this); }


TypeInfo* TypeCheckerVisitor::makeSimple(TypeKind k) {
    return new TypeInfo(k);
}

bool TypeCheckerVisitor::same(TypeInfo* a, TypeInfo* b) {
    if (a == b) return true;
    if (!a || !b) return false;
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

TypeInfo* TypeCheckerVisitor::unifyNum(TypeInfo* a, TypeInfo* b) {
    if (!numeric(a) || !numeric(b)) {
        throw runtime_error("operación aritmética con tipo no numérico");
    }
    // simplificación: siempre elevamos a I64
    return makeSimple(T_I64);
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

    // Primero: registrar structs
    for (auto sd : p->stlist) {
        sd->accept(this);
    }

    // Segundo: registrar firmas de funciones
    for (auto fd : p->fdlist) {
        FunInfo fi;
        fi.ret = fd->returnType;
        fi.params = fd->Ptipos;
        funcs[fd->nombre] = fi;
    }

    // Globales
    for (auto gv : p->vdlist) {
        gv->accept(this);
    }

    // Cuerpos de funciones
    for (auto fd : p->fdlist) {
        fd->accept(this);
    }

    return 0;
}


int TypeCheckerVisitor::visit(Program* /*p*/) {
    return 0;
}

int TypeCheckerVisitor::visit(StructDec* sd) {
    StructInfo info;
    info.fields = sd->fields;
    info.types  = sd->tipos;
    info.sizeBytes = (int)sd->fields.size() * 8; // layout homogéneo de 8 bytes

    for (int i = 0; i < (int)sd->fields.size(); i++) {
        info.index[sd->fields[i]] = i;
    }
    structs[sd->nombre] = info;
    return 0;
}

int TypeCheckerVisitor::visit(GlobalVarDec* vd) {
    // 1) Si hay valor, tiparlo SIEMPRE
    if (vd->value) {
        vd->value->accept(this);
        TypeInfo* vt = get(vd->value);

        if (!vd->type || vd->type->kind == T_UNKNOWN) {
            vd->type = new TypeInfo(*vt);
        }
        else {
            // Si hay tipo declarado, verificar compatibilidad (incluyendo arrays)
            if (!same(vd->type, vt)) {
                throw runtime_error("tipo del inicializador global '" + vd->name + "' incompatible");
            }
        }
    } else {
        // Sin valor: necesitamos al menos un tipo
        if (!vd->type || vd->type->kind == T_UNKNOWN) {
            throw runtime_error("variable global sin tipo ni valor: " + vd->name);
        }
    }

    // 2) Registrar en el entorno global
    tenv.add_var(vd->name, vd->type);
    return 0;
}


int TypeCheckerVisitor::visit(LocalVarDec* vd) {
    if (!vd->type || vd->type->kind == T_UNKNOWN) {
        vd->value->accept(this);
        TypeInfo* vt = get(vd->value);
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

    // parámetros
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

int TypeCheckerVisitor::visit(Body* b) {
    tenv.add_level();
    for (auto d : b->declarations) d->accept(this);
    for (auto s : b->StmList) s->accept(this);
    tenv.remove_level();
    return 0;
}

int TypeCheckerVisitor::visit(NumberExp* e) {
    TypeInfo* t = makeSimple(T_I64);
    inferred[e] = t;
    e->inferredType = t;
    return 0;
}

int TypeCheckerVisitor::visit(StringExp* e) {
    TypeInfo* t = makeSimple(T_STRING);
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
            TypeInfo* t = unifyNum(tl, tr);
            inferred[e] = t;
            e->inferredType = t;
            break;
        }
        default: {
            // comparaciones
            (void)unifyNum(tl, tr);
            TypeInfo* t = makeSimple(T_BOOL);
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

    TypeInfo* ret = info.ret ? info.ret : makeSimple(T_I64);
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

    TypeInfo* t = new TypeInfo(s->nombre); // T_STRUCT con nombre
    inferred[s] = t;
    s->inferredType = t;
    return 0;
}

int TypeCheckerVisitor::visit(ArrayExp* a) {
    if (a->elems.empty()) {
        TypeInfo* elem = makeSimple(T_UNKNOWN);
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

    // Tipo del resultado: tipo de elemento del array
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
    s->e->accept(this);
    // asumimos print solo de enteros / algo numérico
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
    // usar expresión temporal FieldAccessExp para reusar la lógica
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
    // variable del for es i64
    TypeInfo* t = makeSimple(T_I64);
    tenv.add_var(f->var, t);
    localCount++;

    f->start->accept(this);
    f->finish->accept(this);
    TypeInfo* ts = get(f->start);
    TypeInfo* tf = get(f->finish);
    (void)unifyNum(ts, tf);

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

// Calcula la dirección de un lvalue en %rax
void GenCodeVisitor::emitLValueAddr(Exp* e) {
    if (auto id = dynamic_cast<IdExp*>(e)) {
        int off;
        if (envStack.lookup(id->value, off)) {
            // local/param en stack
            out << " leaq " << off << "(%rbp), %rax\n";
        } else {
            // global
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

        // Obtener dirección base del objeto
        emitLValueAddr(fa->obj);   // <<< RECURSIVO Y CORRECTO
        out << " movq %rax, %rdx\n";

        // Tipo del objeto (ya no se muere por patch #1)
        TypeInfo* tobj = tc.get(fa->obj);

        auto it = layouts.find(tobj->structName);
        int off = it->second.fieldOffset[fa->field];

        out << " leaq " << off << "(%rdx), %rax\n";
        return;
    }

    throw runtime_error("lvalue inválido en emitLValueAddr");
}

void GenCodeVisitor::copyStruct(int size) {
    // NO usado por ahora; placeholder
    // podrías implementar un loop si quisieras copiar bloques grandes
    (void)size;
}

void GenCodeVisitor::copyArray(int size) {
    // NO usado por ahora; placeholder
    (void)size;
}

int GenCodeVisitor::generate(Program* p) {
    // 1) Typechecking completo
    tc.typeCheck(p);

    // 2) Construir layouts de struct para código
    buildStructLayouts(p);

    // 3) Sección de datos
    out << ".data\n";
    out << "print_int_fmt: .string \"%ld\\n\"\n";

    // Globales
    for (auto g : p->vdlist) {
        g->accept(this);
    }

    // 4) Código
    out << ".text\n";

    for (auto f : p->fdlist) {
        f->accept(this);
    }

    out << ".section .note.GNU-stack,\"\",@progbits\n";
    return 0;
}

int GenCodeVisitor::visit(Program* /*p*/) {
    return 0;
}

// Global variables: simple .quad 0 (sin init por ahora)
int GenCodeVisitor::visit(GlobalVarDec* vd) {
    out << vd->name << ": .quad 0\n";
    return 0;
}

int GenCodeVisitor::visit(StructDec* /*sd*/) {
    // Nada que emitir, ya usamos layouts
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
    int totalSlots = locals + params;

    int frameSize = totalSlots * 8;
    if (frameSize % 16 != 0) frameSize += 8;

    if (frameSize > 0) {
        out << " subq $" << frameSize << ", %rsp\n";
    }

    // Pasar parámetros a stack
    vector<string> argRegs = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };
    for (int i = 0; i < params && i < 6; i++) {
        envStack.add_var(f->Pnombres[i], offset);
        out << " movq " << argRegs[i] << ", " << offset << "(%rbp)\n";
        offset -= 8;
    }

    // Cuerpo
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

    if (vd->value) {
        vd->value->accept(this);
        out << " movq %rax, " << offset << "(%rbp)\n";
    }

    offset -= 8;
    return 0;
}

int GenCodeVisitor::visit(Body* b) {
    envStack.add_level();
    for (auto d : b->declarations) d->accept(this);
    for (auto s : b->StmList) s->accept(this);
    envStack.remove_level();
    return 0;
}

/* ---- Expresiones ---- */

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
    int off;
    if (envStack.lookup(e->value, off)) {
        out << " movq " << off << "(%rbp), %rax\n";
    } else {
        out << " movq " << e->value << "(%rip), %rax\n";
    }
    return 0;
}

int GenCodeVisitor::visit(BinaryExp* b) {
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

    // malloc(n * 8)
    out << " movq $" << bytes << ", %rdi\n";
    out << " call malloc@PLT\n";
    out << " movq %rax, %r10\n";  // base del array

    for (int i = 0; i < n; i++) {
        a->elems[i]->accept(this);
        out << " movq %rax, " << (i * 8) << "(%r10)\n";
    }

    // return pointer in %rax
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
    StructLayout& lay = it->second;

    out << " movq $" << lay.sizeBytes << ", %rdi\n";
    out << " call malloc@PLT\n";
    out << " movq %rax, %r10\n";

    for (size_t i = 0; i < s->fields.size(); i++) {
        int off = lay.fieldOffset[s->fields[i]];
        s->values[i]->accept(this);
        out << " movq %rax, " << off << "(%r10)\n";
    }

    out << " movq %r10, %rax\n";
    return 0;
}



int GenCodeVisitor::visit(FieldAccessExp* f) {
    f->obj->accept(this);
    out << " movq %rax, %rdx\n";
    TypeInfo* tobj = tc.get(f->obj);
    auto it = layouts.find(tobj->structName);
    if (it == layouts.end()) {
        throw runtime_error("layout no encontrado para struct en FieldAccessExp");
    }
    int off = it->second.fieldOffset[f->field];
    out << " movq " << off << "(%rdx), %rax\n";
    return 0;
}

/* ---- Statements ---- */

int GenCodeVisitor::visit(AssignStm* s) {
    s->value->accept(this);
    out << " pushq %rax\n";
    emitLValueAddr(s->left);
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
    s->e->accept(this);
    out << " movq %rax, %rsi\n";
    out << " leaq print_int_fmt(%rip), %rdi\n";
    out << " movl $0, %eax\n";
    out << " call printf@PLT\n";
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

    // init
    f->start->accept(this);
    out << " movq %rax, " << slot << "(%rbp)\n";

    out << ".Lfor_" << lbl << ":\n";

    // condición
    f->finish->accept(this);
    out << " movq %rax, %rcx\n";
    out << " movq " << slot << "(%rbp), %rax\n";
    out << " cmpq %rcx, %rax\n";
    if (f->equal) {
        // while i <= finish
        out << " jg .Lendfor_" << lbl << "\n";
    } else {
        // while i < finish
        out << " jge .Lendfor_" << lbl << "\n";
    }

    // cuerpo
    f->body->accept(this);

    // i++
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
    int lbl = labelCount++;

    i->condition->accept(this);
    out << " cmpq $0, %rax\n";
    out << " je .Lelse_" << lbl << "\n";

    i->ifbody->accept(this);
    out << " jmp .Lendif_" << lbl << "\n";

    out << ".Lelse_" << lbl << ":\n";
    if (i->elsecond && i->elsebody) {
        i->elsebody->accept(this);
    }

    out << ".Lendif_" << lbl << ":\n";
    return 0;
}

int GenCodeVisitor::visit(StructStm* s) {
    FieldAccessExp fa(s->object, s->field);
    s->value->accept(this);
    out << " pushq %rax\n";
    emitLValueAddr(&fa);
    out << " movq %rax, %rdx\n";
    out << " popq %rax\n";
    out << " movq %rax, (%rdx)\n";
    return 0;
}

int GenCodeVisitor::visit(ArrayStm* s) {
    ArrayAccessExp aa(s->array, s->index);
    s->value->accept(this);
    out << " pushq %rax\n";
    emitLValueAddr(&aa);
    out << " movq %rax, %rdx\n";
    out << " popq %rax\n";
    out << " movq %rax, (%rdx)\n";
    return 0;
}
