import os
import subprocess
import shutil

# Archivos c++
programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp"]

# Compilar con C++17 y generar ejecutable portable
import platform
is_windows = platform.system().lower().startswith("win")
exe_name = "pf.exe" if is_windows else "pf"
compile = ["g++", "-std=gnu++17"] + programa + ["-O2", "-o", exe_name]
print("Compilando:", " ".join(compile))
result = subprocess.run(compile, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación:\n", result.stderr)
    exit(1)

if result.returncode == 0:
    print("Compilación exitosa")
else:
    print("Error en compilación:\n", result.stderr)
    exit(1)

# Ejecutar
input_dir = "inputs"
output_dir = "outputs"
os.makedirs(output_dir, exist_ok=True)

# Ejecutar para cada archivo input*.txt en la carpeta inputs
exe_path = os.path.join(os.getcwd(), exe_name)
for filename in os.listdir(input_dir):
    if filename.startswith("input") and filename.endswith(".txt"):
        filepath = os.path.join(input_dir, filename)
        print(f"Ejecutando {filename}")
        if is_windows:
            run_cmd = [exe_path, filepath]
        else:
            run_cmd = [exe_path, filepath]
        result = subprocess.run(run_cmd, capture_output=True, text=True)
        print(result.stdout)
        if result.returncode != 0:
            print("Ejecución fallida:\n", result.stderr)
        # outputs se generan en carpeta outputs por el programa principal
