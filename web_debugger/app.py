import os
import subprocess
import shutil
from flask import Flask, render_template, jsonify, request, send_file

app = Flask(__name__, template_folder="templates")

BASE_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
CANDIDATES = ['a.exe', 'a.out', './a.out', os.path.join(BASE_DIR, 'a.exe'), os.path.join(BASE_DIR, 'a.out')]
COMPILER_EXE = None
for c in CANDIDATES:
    path = c
    if not os.path.isabs(path):
        path = os.path.join(BASE_DIR, c)
    if os.path.exists(path):
        COMPILER_EXE = path
        break

OUTPUTS_DIR = os.path.join(BASE_DIR, 'outputs')
TEMP_INPUT_NAME = "temp_web_input.txt"
TEMP_INPUT_PATH = os.path.join(BASE_DIR, TEMP_INPUT_NAME)
GENERATED_ASM = os.path.join(OUTPUTS_DIR, "temp_web_input.s")

if not os.path.exists(OUTPUTS_DIR):
    print(f"[WARN] La carpeta outputs no existía. Creándola en: {OUTPUTS_DIR}")
    os.makedirs(OUTPUTS_DIR, exist_ok=True)

class X86Simulator:
    def __init__(self):
        self.reset()

    def reset(self):
        self.registers = {
            'rax': 0, 'rbx': 0, 'rcx': 0, 'rdx': 0,
            'rsi': 0, 'rdi': 0, 'rbp': 2000, 'rsp': 2000,
            'r8': 0, 'r9': 0, 'r10': 0, 'r11': 0, 'r12': 0, 'r13': 0, 'r14': 0, 'r15': 0
        }
        self.memory = {}
        self.string_data = {}
        self.label_addrs = {}
        self.addr_to_label = {}
        self.next_data_addr = 10000
        self.instructions = []
        self.labels = {}
        self.ip = 0
        self.flags = {'ZF': 0, 'SF': 0}
        self.output_log = []
        self.finished = False

    def _alloc_data_for_string(self, s):
        """Aloca espacio para una cadena y retorna la dirección."""
        addr = self.next_data_addr
        self.string_data[addr] = s
        self.next_data_addr += max(8, len(s) + 1)
        return addr

    def _alloc_data_for_quad(self, value):
        """Aloca una dirección para un .quad y guarda el valor."""
        addr = self.next_data_addr
        self.memory[str(addr)] = int(value)
        self.next_data_addr += 8
        return addr

    def load_from_file(self, filepath):
        self.reset()
        if not os.path.exists(filepath):
            return False, f"Archivo .s no encontrado en: {filepath}"

        with open(filepath, 'r', encoding='utf-8') as f:
            lines = [ln.rstrip("\n") for ln in f]

        in_data = False
        in_rodata = False
        pending_label = None

        for idx, raw in enumerate(lines):
            line = raw.strip()
            if line.startswith(".data"):
                in_data = True
                in_rodata = False
                continue
            if line.startswith(".section") and ".rodata" in line:
                in_rodata = True
                in_data = False
                continue
            if line.startswith(".text"):
                in_data = False
                in_rodata = False
                continue

            if line.endswith(":"):
                lbl = line[:-1].strip()
                pending_label = lbl
                continue

            if pending_label and (in_data or in_rodata):
                if line.startswith(".string"):
                    start = line.find('"')
                    end = line.rfind('"')
                    content = ""
                    if start != -1 and end != -1 and end > start:
                        content = line[start+1:end]
                    addr = self._alloc_data_for_string(content)
                    self.label_addrs[pending_label] = addr
                    self.addr_to_label[addr] = pending_label
                    pending_label = None
                    continue

                if line.startswith(".quad"):
                    parts = line.split()
                    if len(parts) >= 2:
                        value = parts[1]
                        addr = self._alloc_data_for_quad(value)
                        self.label_addrs[pending_label] = addr
                        self.addr_to_label[addr] = pending_label
                        pending_label = None
                        continue

                if line.startswith(".long") or line.startswith(".zero") or line.startswith(".byte"):
                    addr = self.next_data_addr
                    self.next_data_addr += 8
                    self.label_addrs[pending_label] = addr
                    self.addr_to_label[addr] = pending_label
                    pending_label = None
                    continue

                addr = self.next_data_addr
                self.next_data_addr += 8
                self.label_addrs[pending_label] = addr
                self.addr_to_label[addr] = pending_label
                pending_label = None
                continue

            if pending_label and not (in_data or in_rodata):
                self.label_addrs[pending_label] = None
                pending_label = None

        self.instructions = []
        self.labels = {}
        clean_idx = 0
        current_section = None

        for raw in lines:
            line = raw.strip()
            if not line:
                continue
            if line.startswith(".data"):
                current_section = 'data'
                continue
            if line.startswith(".section") and ".rodata" in line:
                current_section = 'rodata'
                continue
            if line.startswith(".text"):
                current_section = 'text'
                continue

            if current_section != 'text':
                continue

            if line.endswith(":"):
                lbl = line[:-1].strip()
                self.labels[lbl] = clean_idx
                if lbl in self.label_addrs and self.label_addrs[lbl] is None:
                    self.label_addrs[lbl] = f"TEXTIDX_{clean_idx}"
                    self.addr_to_label[f"TEXTIDX_{clean_idx}"] = lbl
                clean_idx += 0
                continue

            if line.startswith(".") and not line.startswith(".LC"):
                continue

            self.instructions.append(line)
            clean_idx += 1

        for lbl, val in list(self.label_addrs.items()):
            if val is None:
                addr = self.next_data_addr
                self.next_data_addr += 8
                self.label_addrs[lbl] = addr
                self.addr_to_label[addr] = lbl

        if 'main' in self.labels:
            self.ip = self.labels['main']
        else:
            self.ip = 0

        return True, "Cargado correctamente"

    def get_val(self, op):
        op = op.strip()
        if not op:
            return 0
        if op.startswith('$'):
            try:
                return int(op[1:])
            except:
                return 0
        if op.startswith('%'):
            return int(self.registers.get(op[1:], 0))
        if op.endswith('(%rip)') or '(%rip)' in op:
            label = op.split('(')[0].strip()
            if label.isdigit():
                return int(label)
            if label in self.label_addrs:
                addr = self.label_addrs[label]
                if isinstance(addr, int):
                    return int(addr)
                if isinstance(addr, str) and addr.startswith("TEXTIDX_"):
                    return 0
            return 0
        if '(' in op:
            try:
                offset_str, base_part = op.split('(')
                base = base_part.strip(' )%')
                offset = int(offset_str) if offset_str else 0
                addr = int(self.registers.get(base, 0)) + offset
                return int(self.memory.get(str(addr), 0))
            except:
                return 0
        try:
            return int(op)
        except:
            if op in self.label_addrs:
                addr = self.label_addrs[op]
                if isinstance(addr, int):
                    return int(addr)
            return 0

    def set_val(self, op, val):
        op = op.strip()
        if not op:
            return
        if op.startswith('%'):
            self.registers[op[1:]] = int(val)
            return
        if op.endswith('(%rip)'):
            return
        if '(' in op:
            try:
                offset_str, base_part = op.split('(')
                base = base_part.strip(' )%')
                offset = int(offset_str) if offset_str else 0
                addr = int(self.registers.get(base, 0)) + offset
                self.memory[str(addr)] = int(val)
            except:
                return
            return
        if op in self.label_addrs and isinstance(self.label_addrs[op], int):
            addr = self.label_addrs[op]
            self.memory[str(addr)] = int(val)
            return

    def step(self):
        if self.finished or self.ip >= len(self.instructions):
            self.finished = True
            return

        line = self.instructions[self.ip]
        parts = line.split(maxsplit=1)
        opcode = parts[0]
        args = parts[1].split(',') if len(parts) > 1 else []

        try:
            if opcode == 'movq':
                val = self.get_val(args[0])
                self.set_val(args[1], val)
                self.ip += 1
            elif opcode == 'addq':
                src = self.get_val(args[0])
                dest = self.get_val(args[1])
                self.set_val(args[1], dest + src)
                self.ip += 1
            elif opcode == 'subq':
                src = self.get_val(args[0])
                dest = self.get_val(args[1])
                res = dest - src
                self.set_val(args[1], res)
                self.flags['ZF'] = 1 if res == 0 else 0
                self.flags['SF'] = 1 if res < 0 else 0
                self.ip += 1
            elif opcode == 'imulq':
                src = self.get_val(args[0])
                dest = self.get_val(args[1])
                self.set_val(args[1], dest * src)
                self.ip += 1
            elif opcode == 'pushq':
                val = self.get_val(args[0])
                self.registers['rsp'] -= 8
                self.memory[str(self.registers['rsp'])] = int(val)
                self.ip += 1
            elif opcode == 'popq':
                val = self.memory.get(str(self.registers['rsp']), 0)
                self.set_val(args[0], val)
                self.registers['rsp'] += 8
                self.ip += 1
            elif opcode == 'leaq':
                op1 = args[0].strip()
                dest = args[1].strip()
                if op1.endswith('(%rip)') or '(%rip)' in op1:
                    label = op1.split('(')[0].strip()
                    addr = self.label_addrs.get(label, 0)
                    if isinstance(addr, int):
                        self.set_val(dest, addr)
                    else:
                        self.set_val(dest, 0)
                elif '(' in op1:
                    try:
                        offset_str, base_part = op1.split('(')
                        base = base_part.strip(' )%')
                        offset = int(offset_str) if offset_str else 0
                        addr = int(self.registers.get(base, 0)) + offset
                        self.set_val(dest, addr)
                    except:
                        self.set_val(dest, 0)
                else:
                    if op1 in self.label_addrs and isinstance(self.label_addrs[op1], int):
                        self.set_val(dest, int(self.label_addrs[op1]))
                    else:
                        self.set_val(dest, 0)
                self.ip += 1
            elif opcode == 'cmpq':
                v1 = self.get_val(args[0])
                v2 = self.get_val(args[1])
                res = v2 - v1
                self.flags['ZF'] = 1 if res == 0 else 0
                self.flags['SF'] = 1 if res < 0 else 0
                self.ip += 1
            elif opcode.startswith('j'):
                target = args[0].strip()
                jump = False
                if opcode == 'jmp': jump = True
                elif opcode == 'je' and self.flags['ZF']: jump = True
                elif opcode == 'jne' and not self.flags['ZF']: jump = True
                elif opcode == 'jg' and not self.flags['SF'] and not self.flags['ZF']: jump = True
                elif opcode == 'jge' and not self.flags['SF']: jump = True
                elif opcode == 'jl' and self.flags['SF']: jump = True
                elif opcode == 'jle' and (self.flags['SF'] or self.flags['ZF']): jump = True

                if jump:
                    if target in self.labels:
                        self.ip = self.labels[target]
                    else:
                        tgt = target.rstrip(':')
                        if tgt in self.labels:
                            self.ip = self.labels[tgt]
                        else:
                            self.ip += 1
                else:
                    self.ip += 1
            elif opcode == 'call':
                target = args[0].strip()
                if target == 'printf@PLT':
                    rdi = self.registers.get('rdi', 0)
                    rsi = self.registers.get('rsi', 0)

                    label = None
                    for lbl, addr in self.label_addrs.items():
                        if isinstance(addr, int) and addr == rdi:
                            label = lbl
                            break

                    if label:
                        if rdi in self.string_data:
                            fmt = self.string_data[rdi]
                        else:
                            addr_lbl = self.label_addrs.get(label)
                            if isinstance(addr_lbl, int) and addr_lbl in self.string_data:
                                fmt = self.string_data[addr_lbl]
                            else:
                                fmt = ""
                    else:
                        fmt = ""

                    if ("%d" in fmt) or ("%ld" in fmt) or ("{}" in fmt) or ("%i" in fmt):
                        self.output_log.append(str(rsi))
                    else:
                        if rsi in self.string_data:
                            self.output_log.append(self.string_data[rsi])
                        else:
                            lab = self.addr_to_label.get(rsi)
                            if lab and isinstance(self.label_addrs.get(lab), int) and self.label_addrs.get(lab) in self.string_data:
                                self.output_log.append(self.string_data[self.label_addrs[lab]])
                            else:
                                self.output_log.append(str(rsi))
                    self.ip += 1

                elif target == 'malloc@PLT':
                    addr = self.next_data_addr
                    self.next_data_addr += 64
                    self.registers['rax'] = addr
                    self.ip += 1
                elif target in self.labels:
                    self.registers['rsp'] -= 8
                    self.memory[str(self.registers['rsp'])] = self.ip + 1
                    self.ip = self.labels[target]
                else:
                    self.ip += 1
            elif opcode == 'ret':
                ret_addr = int(self.memory.get(str(self.registers['rsp']), 0))
                self.registers['rsp'] += 8
                if ret_addr == 0:
                    self.finished = True
                else:
                    self.ip = ret_addr
            elif opcode == 'leave':
                self.registers['rsp'] = self.registers['rbp']
                val = int(self.memory.get(str(self.registers['rsp']), 0))
                self.registers['rbp'] = val
                self.registers['rsp'] += 8
                self.ip += 1
            else:
                self.ip += 1
        except Exception as e:
            self.output_log.append(f"RuntimeError en '{line}': {e}")
            self.ip += 1

    def __init__(self):
        self.reset()

    def reset(self):
        self.registers = {
            'rax': 0, 'rbx': 0, 'rcx': 0, 'rdx': 0,
            'rsi': 0, 'rdi': 0, 'rbp': 2000, 'rsp': 2000,
            'r8': 0, 'r9': 0, 'r10': 0, 'r11': 0, 'r12': 0, 'r13': 0, 'r14': 0, 'r15': 0
        }
        self.memory = {}
        self.instructions = []
        self.labels = {}
        self.ip = 0
        self.flags = {'ZF': 0, 'SF': 0}
        self.output_log = []
        self.finished = False

    def load_from_file(self, filepath):
        self.reset()
        if not os.path.exists(filepath):
            return False, f"Archivo .s no encontrado en: {filepath}"

        with open(filepath, 'r', encoding='utf-8') as f:
            lines = f.readlines()

        clean_idx = 0
        for line in lines:
            line = line.strip()
            if not line:
                continue

            if line.endswith(':'):
                label = line[:-1]
                self.labels[label] = clean_idx
                continue

            if line.startswith(".") and not line.endswith(":") and not line.startswith(".LC"):
                continue

            self.instructions.append(line)
            clean_idx += 1

        if 'main' in self.labels:
            self.ip = self.labels['main']
        else:
            self.ip = 0

        return True, "Cargado correctamente"

    def get_val(self, op):
        op = op.strip()
        if not op:
            return 0
        if op.startswith('$'):
            return int(op[1:])
        if op.startswith('%'):
            return int(self.registers.get(op[1:], 0))
        if '(' in op:
            parts = op.split('(')
            offset_str = parts[0].strip()
            base = parts[1].strip(' )%')
            offset = int(offset_str) if offset_str else 0
            addr = int(self.registers.get(base, 0)) + offset
            return int(self.memory.get(str(addr), 0))
        try:
            return int(op)
        except:
            return 0

    def set_val(self, op, val):
        op = op.strip()
        if op.startswith('%'):
            self.registers[op[1:]] = int(val)
        elif '(' in op:
            parts = op.split('(')
            offset_str = parts[0].strip()
            base = parts[1].strip(' )%')
            offset = int(offset_str) if offset_str else 0
            addr = int(self.registers.get(base, 0)) + offset
            self.memory[str(addr)] = int(val)
        else:
            pass

    def step(self):
        if self.finished or self.ip >= len(self.instructions):
            self.finished = True
            return

        line = self.instructions[self.ip]
        parts = line.split(maxsplit=1)
        opcode = parts[0]
        args = parts[1].split(',') if len(parts) > 1 else []

        try:
            if opcode == 'movq':
                val = self.get_val(args[0])
                self.set_val(args[1], val)
                self.ip += 1
            elif opcode == 'addq':
                src = self.get_val(args[0])
                dest = self.get_val(args[1])
                self.set_val(args[1], dest + src)
                self.ip += 1
            elif opcode == 'subq':
                src = self.get_val(args[0])
                dest = self.get_val(args[1])
                res = dest - src
                self.set_val(args[1], res)
                self.flags['ZF'] = 1 if res == 0 else 0
                self.flags['SF'] = 1 if res < 0 else 0
                self.ip += 1
            elif opcode == 'imulq':
                src = self.get_val(args[0])
                dest = self.get_val(args[1])
                self.set_val(args[1], dest * src)
                self.ip += 1
            elif opcode == 'pushq':
                val = self.get_val(args[0])
                self.registers['rsp'] -= 8
                self.memory[str(self.registers['rsp'])] = int(val)
                self.ip += 1
            elif opcode == 'popq':
                val = self.memory.get(str(self.registers['rsp']), 0)
                self.set_val(args[0], val)
                self.registers['rsp'] += 8
                self.ip += 1
            elif opcode == 'leaq':
                op1 = args[0].strip()
                if '(' in op1:
                    offset_str, base = op1.split('(')
                    base = base.strip(')%')
                    offset = int(offset_str) if offset_str else 0
                    addr = self.registers.get(base, 0) + offset
                    self.set_val(args[1], addr)
                else:
                    self.set_val(args[1], 0)
                self.ip += 1
            elif opcode == 'cmpq':
                v1 = self.get_val(args[0])
                v2 = self.get_val(args[1])
                res = v2 - v1
                self.flags['ZF'] = 1 if res == 0 else 0
                self.flags['SF'] = 1 if res < 0 else 0
                self.ip += 1
            elif opcode.startswith('j'):
                target = args[0].strip()
                jump = False
                if opcode == 'jmp': jump = True
                elif opcode == 'je' and self.flags['ZF']: jump = True
                elif opcode == 'jne' and not self.flags['ZF']: jump = True
                elif opcode == 'jg' and not self.flags['SF'] and not self.flags['ZF']: jump = True
                elif opcode == 'jge' and not self.flags['SF']: jump = True
                elif opcode == 'jl' and self.flags['SF']: jump = True
                elif opcode == 'jle' and (self.flags['SF'] or self.flags['ZF']): jump = True

                if jump:
                    if target in self.labels:
                        self.ip = self.labels[target]
                    else:
                        lbl = target
                        if lbl.endswith(':'): lbl = lbl[:-1]
                        if lbl in self.labels:
                            self.ip = self.labels[lbl]
                        else:
                            self.ip += 1
                else:
                    self.ip += 1
            elif opcode == 'call':
                target = args[0].strip()
                if target == 'printf@PLT':
                    val = self.registers.get('rsi', 0)
                    self.output_log.append(str(val))
                    self.ip += 1
                elif target == 'malloc@PLT':
                    self.registers['rax'] = 5000 + len(self.memory)
                    self.ip += 1
                elif target in self.labels:
                    self.registers['rsp'] -= 8
                    self.memory[str(self.registers['rsp'])] = self.ip + 1
                    self.ip = self.labels[target]
                else:
                    self.ip += 1
            elif opcode == 'ret':
                ret_addr = self.memory.get(str(self.registers['rsp']), 0)
                self.registers['rsp'] += 8
                if ret_addr == 0:
                    self.finished = True
                else:
                    self.ip = ret_addr
            elif opcode == 'leave':
                self.registers['rsp'] = self.registers['rbp']
                val = self.memory.get(str(self.registers['rsp']), 0)
                self.registers['rbp'] = val
                self.registers['rsp'] += 8
                self.ip += 1
            else:
                self.ip += 1
        except Exception as e:
            self.output_log.append(f"RuntimeError en '{line}': {e}")
            self.ip += 1

sim = X86Simulator()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/compile', methods=['POST'])
def compile_code():
    code = request.json.get('code', '')
    if not code:
        return jsonify({'status': 'error', 'msg': 'No enviaste código'})

    try:
        with open(TEMP_INPUT_PATH, 'w', encoding='utf-8') as f:
            f.write(code)
    except Exception as e:
        return jsonify({'status': 'error', 'msg': f'Error escribiendo input: {str(e)}'})

    if COMPILER_EXE is None:
        return jsonify({'status': 'error', 'msg': 'No se encontró ejecutable del compilador (busqué a.exe / a.out).'})

    try:
        result = subprocess.run([COMPILER_EXE, TEMP_INPUT_NAME], cwd=BASE_DIR, capture_output=True, text=True)
    except Exception as e:
        return jsonify({'status': 'error', 'msg': f'Error al ejecutar el compilador: {e}'})

    if result.returncode != 0:
        return jsonify({
            'status': 'compile_error',
            'stdout': result.stdout,
            'stderr': result.stderr
        })

    if not os.path.exists(GENERATED_ASM):
        return jsonify({'status': 'error', 'msg': f'Compilador terminó pero no generó {GENERATED_ASM}', 'stdout': result.stdout, 'stderr': result.stderr})

    success, msg = sim.load_from_file(GENERATED_ASM)
    if not success:
        return jsonify({'status': 'error', 'msg': f'Error cargando ASM: {msg}'})

    return jsonify({
        'status': 'ok',
        'compiler_out': result.stdout,
        'compiler_err': result.stderr,
        'instructions': sim.instructions,
        'ip': sim.ip
    })

@app.route('/step', methods=['POST'])
def step():
    sim.step()
    mem_out = {}
    for k, v in sim.memory.items():
        try:
            mem_out[str(int(k))] = int(v)
        except:
            mem_out[str(k)] = v

    return jsonify({
        'ip': sim.ip,
        'registers': sim.registers,
        'memory': mem_out,
        'logs': sim.output_log,
        'finished': sim.finished
    })

@app.route('/asm', methods=['GET'])
def get_asm():
    if os.path.exists(GENERATED_ASM):
        return send_file(GENERATED_ASM, mimetype='text/plain')
    return jsonify({'status': 'error', 'msg': 'ASM no generado aún'})

if __name__ == '__main__':
    print("Iniciando servidor...")
    print(f"Raíz del proyecto: {BASE_DIR}")
    print(f"Compilador detectado: {COMPILER_EXE}")
    app.run(debug=True, port=5000)
