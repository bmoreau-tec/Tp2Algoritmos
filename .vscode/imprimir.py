# numerar_codigo.py
import re

def numerar_cpp(archivo_cpp, archivo_salida):
    with open(archivo_cpp, "r", encoding="utf-8") as entrada, \
         open(archivo_salida, "w", encoding="utf-8") as salida:
        
        numero_linea = 1
        for linea in entrada:
            # Reemplazar tabs reales por dos espacios
            linea = linea.replace("\t", "  ")
            # Reemplazar grupos de 4 espacios al inicio por 2 espacios
            linea = re.sub(r"^( {4})", "  ", linea)
            # Quitar salto de línea final
            linea_sin_salto = linea.rstrip("\n")
            # Escribir número de línea
            salida.write(f"{numero_linea:4} {linea_sin_salto}\n")
            numero_linea += 1

    print(f"Archivo '{archivo_salida}' generado con numeración y sangría de 2 espacios.")

# Ejemplo de uso:
# Cambiá estos nombres por el tuyo
entrada_cpp = "TP1V1K1021GX-Scalise Giussani Federico.cpp"
salida_txt = "programa_numerado.txt"

numerar_cpp(entrada_cpp, salida_txt)
