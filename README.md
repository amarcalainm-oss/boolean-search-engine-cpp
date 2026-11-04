# Buscador booleano con índice invertido (C++)

Motor de búsqueda de documentos escrito en **C++17** desde cero, solo con la STL. Construye un **índice invertido**, elimina *stopwords* y duplicados, y resuelve **consultas booleanas** (`AND` / `OR` con precedencia) convirtiéndolas a notación postfija. También procesa una **cola de peticiones de usuarios** en orden FIFO y mide el tiempo de cada una.

Es la misma idea que está en la base de los motores de búsqueda y de la fase de *retrieval* en sistemas RAG: indexar una vez y responder consultas rápido.

Proyecto de *Estructuras de Datos y Algoritmos* (Ingeniería en IA, TECNUN – Universidad de Navarra, curso 2025-26).
**Autores:** Beñat, Aitor Marcalain e Iker.

## Cómo funciona

```
corpus.txt ──► tokenizar + minúsculas + quitar stopwords ──► índice invertido ──► indice.txt
                                                              palabra → [docIDs]
consultas.txt ──► tokens ──► infijo → postfijo (Shunting Yard) ──► evaluar con pila ──► resultados_consultas.txt
peticiones.txt ──► cola FIFO ──► una consulta por petición + tiempo ──► log_procesamiento.txt
```

| Módulo | Qué hace | Estructuras de datos |
|---|---|---|
| **Indexado** | Lee el corpus por documentos (`DOC:n`), normaliza y filtra ~200 *stopwords* en español | `unordered_map<string, vector<int>>` + `unordered_set` de stopwords (búsqueda O(1)) |
| **Deduplicado** | Variante que ignora documentos y palabras repetidos | `unordered_map<string, unordered_set<int>>`: el set descarta duplicados sin tener que buscar en un vector |
| **Consultas booleanas** | `AND` tiene más precedencia que `OR`. Convierte a postfijo y evalúa | `stack` para operadores y resultados, `set<int>` con `set_intersection` / `set_union` |
| **Cola de peticiones** | Atiende las peticiones `hora \| usuario \| consulta` en orden de llegada y mide cada una | `queue` + `std::chrono` |

## Ejemplo

**Entrada** (`data/consultas.txt`):
```
inteligencia AND datos
algoritmos OR aprendizaje
inteligencia AND artificial AND datos
busqueda OR algoritmos AND datos
```

**Salida** (`examples/resultados_consultas.txt`):
```
Consulta: busqueda OR algoritmos AND datos
 Postfija: busqueda algoritmos datos AND OR
 Resultado: [2] <- AND evalua antes que OR
```

**Log de peticiones** (`examples/log_procesamiento.txt`):
```
[09:00:01] PROCESANDO -> usuario_42: 'inteligencia AND datos'
-> Documentos encontrados: [1, 3] | Tiempo: 16microsec
```

**Experimento de escala:** con `corpus1000.txt` (115 documentos, ~1.100 palabras), el índice se construye en **~0,2 ms**.

## Compilar y ejecutar

El programa lee y escribe los ficheros en el directorio actual, así que hay que ejecutarlo desde `data/`.

**g++ (Linux / macOS / MinGW)**
```bash
g++ -std=c++17 -O2 src/*.cpp -o buscador
cd data && ../buscador
```

**CMake (incluido Visual Studio)**
```bash
cmake -S . -B build
cmake --build build --config Release
cd data && ../build/Release/buscador     # con g++/Make: ../build/buscador
```

Probado con g++ 15.2 (MSYS2) y MSVC (Visual Studio 2022).

## Estructura

```
├── src/
│   ├── main.cpp                 # orquesta las 4 partes + el experimento
│   ├── funciones_buscador.cpp   # implementación
│   └── headers.h                # declaraciones
├── data/                        # ficheros de entrada (corpus, consultas, peticiones)
├── examples/                    # salidas generadas por el programa
└── CMakeLists.txt
```

## Limitaciones y posibles mejoras

- Las consultas no admiten paréntesis ni `NOT`.
- La tokenización separa solo por espacios, así que los signos de puntuación quedan pegados a las palabras ("datos," ≠ "datos") y no se normalizan los acentos.
- En el indexado básico, comprobar si un documento ya está en la lista es O(n) (`std::find` sobre un `vector`). La versión deduplicada lo resuelve con `unordered_set`.
- Siguientes pasos naturales: ranking por relevancia (TF-IDF / BM25), *stemming* y persistencia del índice en binario.
