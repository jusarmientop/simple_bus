import pandas as pd
import glob
import os
import re
def calcular_porcentaje_por_master(lineas_transferencia):
    total = len(lineas_transferencia)
    if total == 0:
        return {}

    conteo_por_master = {}

    for linea in lineas_transferencia:
        partes = linea.split(',')
        if len(partes) < 3:
            continue
        master_id = partes[2].strip()
        conteo_por_master[master_id] = conteo_por_master.get(master_id, 0) + 1

    porcentajes = {}
    for master_id, cuenta in conteo_por_master.items():
        porcentajes[f'master_{master_id}_%'] = round(100 * cuenta / total, 2)

    return porcentajes


def calcular_throughput(lineas_transferencia):
    """Calcula el throughput como transferencias efectivas por unidad de tiempo."""
    lineas_transferencia = [l.strip() for l in lineas_transferencia if l.strip()]
    if not lineas_transferencia:
        return 0.0

    lineas_validas = [l for l in lineas_transferencia if "idle" not in l.lower()]
    transferencias = len(lineas_validas)

    # Corregido: extraer tiempo de líneas CSV
    tiempo_inicio = int(lineas_transferencia[0].split(',')[0])
    tiempo_fin = int(lineas_transferencia[-1].split(',')[0])
    tiempo_total = tiempo_fin - tiempo_inicio

    if tiempo_total <= 0:
        return 0.0

    throughput = (transferencias*4) / (tiempo_total/1000)  # Convertir a bytes por nanosegundo
    return round(throughput, 6)


def calcular_uso_bus(lineas_transferencia):
    """Calcula el porcentaje de tiempo que el bus estuvo en uso (no idle)."""
    total_lineas = 0
    lineas_idle = 0

    for linea in lineas_transferencia:
        linea = linea.strip()
        if not linea:
            continue
        total_lineas += 1
        if linea.startswith("idle") or "idle" in linea:
            lineas_idle += 1

    if total_lineas == 0:
        return 0.0

    uso_bus = (total_lineas - lineas_idle) / total_lineas * 100
    return round(uso_bus, 2)


def detectar_separador(linea):
    if ',' in linea:
        return ','
    elif '\t' in linea:
        return '\t'
    elif re.search(r'\s{2,}', linea):
        return r'\s+'
    else:
        return None

def extraer_configuracion(path):
    with open(path, 'r') as f:
        lines = f.readlines()

    config_lines = lines[1:5]
    separador = detectar_separador(config_lines[0])
    if separador is None:
        print(f"⚠️ No se pudo detectar separador en {path}")
        return {}

    resultado = {'archivo': os.path.basename(path)}

    for line in config_lines:
        if separador == r'\s+':
            parts = re.split(separador, line.strip())
        else:
            parts = line.strip().split(separador)

        if len(parts) < 5:
            continue

        nombre = parts[0]
        datos = {
            'prioridad': int(parts[1]),
            'direccion': int(parts[2]),
            'lock': int(parts[3]),
            'descanso': int(parts[4])
        }

        # clave abreviada por nombre de maestro
        if '_b_0' in nombre:
            prefijo = 'b0'
        elif '_b_1' in nombre:
            prefijo = 'b1'
        elif '_nb_0' in nombre:
            prefijo = 'nb0'
        elif '_nb_1' in nombre:
            prefijo = 'nb1'
        else:
            continue  # Si no es reconocible

        for k, v in datos.items():
            resultado[f'{prefijo}_{k}'] = v

    return resultado

def procesar_todos_los_archivos(directorio='./ejecuciones/'):
    archivos = sorted(glob.glob(os.path.join(directorio, 'bus_log_*.csv')))
    todas_configuraciones = []

    for archivo in archivos:
        config = extraer_configuracion(archivo)
        if not config:
            continue

        # ➕ Leer de nuevo para extraer transferencias
        with open(archivo, 'r') as f:
            lineas = f.readlines()

        lineas_transferencia = []
        leyendo_transferencias = False
        for linea in lineas:
            if linea.strip().startswith("Tiempo"):
                leyendo_transferencias = True
                continue
            if leyendo_transferencias:
                lineas_transferencia.append(linea.strip())

        # ➕ Calcular uso del bus
        uso_bus = calcular_uso_bus(lineas_transferencia)
        config['uso_bus_%'] = uso_bus

        # 🆕 Calcular throughput
        throughput = calcular_throughput(lineas_transferencia)
        config['throughput(bytes/ns)'] = throughput

        # ➕ Calcular porcentaje de transferencias por master
        porcentaje_masters = calcular_porcentaje_por_master(lineas_transferencia)
        config.update(porcentaje_masters)
            

        todas_configuraciones.append(config)

    df = pd.DataFrame(todas_configuraciones)
    df.to_csv('configuraciones.csv', index=False)
    print(f"✅ Configuraciones + uso del bus extraídas de {len(archivos)} archivos y guardadas en 'configuraciones.csv'")

if __name__ == "__main__":
    procesar_todos_los_archivos()
