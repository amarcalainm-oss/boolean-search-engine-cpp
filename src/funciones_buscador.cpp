#include "headers.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>
#include <chrono>
#include <algorithm> //Para usar transform
#include <queue>
#include <chrono>
using namespace std::chrono;


//BEÑAT

// Diccionario de palabras que no queremos (stopwords)
std::unordered_set<std::string> stopwords = {
    // Articulos y determinantes
    "el","la","los","las","un","una","unos","unas","lo","al","del","este","esta","estos","estas","ese","esa","esos","esas","aquel","aquella","aquellos","aquellas","cualquiera","cualquier","tal","tales",
    // Preposiciones
    "a","ante","bajo","cabe","con","contra","de","desde","durante","en","entre","hacia","hasta","mediante","para","por","segun","sin","sobre","tras","versus","via","salvo","excepto","frente","junto",
    // Conjunciones y conectores
    "y","e","ni","o","u","pero","aunque","mas","sino","pues","si","asi","luego","entonces","mientras","como","aun","tan","ademas","es","ya","todavia","incluso","finalmente",
    // Pronombres
    "yo","tu","ella","nosotros","nosotras","vosotros","vosotras","ellos","ellas","me","te","se","nos","os","mi","ti","conmigo","contigo","consigo","su","sus","nuestro","nuestra","nuestros","nuestras","vuestro","vuestra","vuestros","vuestras","quien","quienes","cual","cuales","cuyo","cuyos","cuya","cuyas","donde","cuan","cualesquiera",
    // Adverbios
    "muy","mas","menos","asi","tambien","entonces","aqui","alli","alla","hoy","ahora","siempre","nunca","ya","todavia","apenas","incluso","luego","tan","tanto","bastante","casi","solo","solamente","demasiado","igualmente","seguramente","realmente","posiblemente","probablemente","justamente","efectivamente",
    // Negacion
    "no","nada","nadie","ninguno","ninguna","tampoco","ningunos","ningunas",
    // Verbos comunes
    "ser","es","eres","somos","sois","son","fui","fue","fueron","era","eran","estar","estoy","esta","estamos","estan","estaba","estaban","haber","he","has","ha","hemos","han","habia","habian","tener","tengo","tienes","tiene","tenemos","tienen","tenia","tenian","hacer","hago","hace","hacemos","hacen","hice","hizo","hicieron","ir","voy","va","vamos","van","decir","digo","dice","decimos","dicen","dijo","dijeron","poder","puedo","puede","podemos","pueden","pude","pudo","pudieron","deber","debo","debe","debemos","deben","debia","debian"
};

void procesarArchivo(const std::string& nombreArchivo, const std::string& archivoSalida) // Evitamos copiar el string completo (eficiencia)
{
    std::ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir " << nombreArchivo << "\n";
        return;
    }

    // Indice invertido: palabra -> lista de documentos
    std::unordered_map<std::string, std::vector<int>> indice;

    std::string linea; // Donde guardamos cada linea
    int docID = 0;     // Documento actual

    // ?? INICIO TIEMPO INSERCIÓN
    auto inicio_insercion = std::chrono::high_resolution_clock::now();

    while (std::getline(archivo, linea)) // Leer linea a linea
    {
        if (linea.empty()) continue; // Ignorar lineas vacias

        if (linea.rfind("--", 0) == 0) continue; // Ignorar titulo

        // Detectar documento
        if (linea.substr(0, 4) == "DOC:")
        {
            docID = std::stoi(linea.substr(4)); // Convertir a int
        }
        else
        {
            std::istringstream iss(linea); // Separar palabras
            std::string palabra;

            while (iss >> palabra)
            {
                // Convertir a minusculas
                std::transform(palabra.begin(), palabra.end(), palabra.begin(), ::tolower);

                // Filtrar usando diccionario
                if (stopwords.find(palabra) == stopwords.end())
                {
                    // Accedemos a la lista de documentos de esa palabra
                    auto& lista = indice[palabra];

                    // Evitar duplicados manualmente
                    if (std::find(lista.begin(), lista.end(), docID) == lista.end())
                    {
                        lista.push_back(docID);
                    }
                }
            }
        }
    }

    archivo.close();

    // ?? FIN TIEMPO INSERCIÓN
    auto fin_insercion = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> tiempo_insercion = fin_insercion - inicio_insercion;

    std::cout << "Tiempo de insercion: " << tiempo_insercion.count() << " ms\n";

    // Crear archivo de salida
    std::ofstream salida(archivoSalida);

    if (!salida.is_open()) {
        std::cerr << "Error al crear " << archivoSalida << "\n";
        return;
    }

    salida << "-- Fichero: " << archivoSalida << " --\n\n"; // Titulo

    // Escribir el indice
    for (const auto& par : indice)
    {
        salida << par.first << " -> [";

        for (size_t i = 0; i < par.second.size(); ++i)
        {
            salida << par.second[i];

            if (i != par.second.size() - 1)
                salida << ", ";
        }

        salida << "]\n";
    }

    salida.close();

    std::cout << "Indice invertido generado en " << archivoSalida << "\n";
}


//AITOR

void procesarArchivo_Duplicados(const std::string& nombreArchivo)
{
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir " << nombreArchivo << "\n";
        return;
    }

    // CAMBIO 1: Se usa std::unordered_set en lugar de std::vector
    std::unordered_map<std::string, std::unordered_set<int>> indice;
    std::string linea;
    int docID = 0;

    while (std::getline(archivo, linea))
    {
        if (linea.empty()) continue;

        if (linea.rfind("--", 0) == 0) continue;

        if (linea.substr(0, 4) == "DOC:")
        {
            docID = std::stoi(linea.substr(4));
        }
        else {
            std::istringstream iss(linea);
            std::string palabra;
            while (iss >> palabra) {

                std::transform(palabra.begin(), palabra.end(), palabra.begin(), ::tolower);

                if (stopwords.find(palabra) == stopwords.end())
                {
                    // CAMBIO 2: Inserción directa. El unordered_set ignora los duplicados automáticamente.
                    // Nos ahorramos el std::find y el push_back
                    indice[palabra].insert(docID);
                }
            }
        }
    }
    archivo.close();

    // CAMBIO 3: Según el PDF, la salida de la parte 2 debe llamarse indice_deduplicado.txt
    std::ofstream salida("indice_deduplicado.txt");
    salida << "-- Fichero: indice_deduplicado.txt --\n\n";
    if (!salida.is_open()) {
        std::cerr << "Error al crear indice_deduplicado.txt\n";
        return;
    }

    for (const auto& par : indice)
    {
        salida << par.first << " -> [";

        // CAMBIO 4: Como el unordered_set no tiene índices (como par.second[i]), 
        // usamos un bucle basado en rangos y un contador manual para las comas
        size_t i = 0;
        size_t total = par.second.size();
        for (int id : par.second) {
            salida << id;
            if (i != total - 1)
                salida << ", ";
            i++;
        }

        salida << "]\n";
    }

    salida.close();
    std::cout << "Indice invertido generado en indice_deduplicado.txt\n";
}

//IKER

IndiceConsulta leerIndiceConsulta(const std::string& nombreArchivo) {                // Esta funcion lee indice.txt y construye el indice invertido en memoria
    IndiceConsulta indice;
    std::ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir " << nombreArchivo << "\n";
        return indice;
    }

    std::string linea;

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;
        if (linea.rfind("--", 0) == 0) continue; // saltar cabeceras tipo -- Fichero: ...

        size_t posFlecha = linea.find("->");
        if (posFlecha == std::string::npos) continue;

        std::string palabra = linea.substr(0, posFlecha);
        std::string parteDocs = linea.substr(posFlecha + 2);

        while (!palabra.empty() && palabra.front() == ' ')
            palabra.erase(0, 1);
        while (!palabra.empty() && palabra.back() == ' ')
            palabra.pop_back();

        size_t ini = parteDocs.find('[');
        size_t fin = parteDocs.find(']');

        if (ini == std::string::npos || fin == std::string::npos || ini >= fin)
            continue;

        std::string contenido = parteDocs.substr(ini + 1, fin - ini - 1);
        std::stringstream ss(contenido);
        std::string num;

        while (std::getline(ss, num, ',')) {
            while (!num.empty() && num.front() == ' ')
                num.erase(0, 1);
            while (!num.empty() && num.back() == ' ')
                num.pop_back();

            if (!num.empty()) {
                indice[palabra].insert(std::stoi(num));
            }
        }
    }

    archivo.close();
    return indice;
}

bool esOperador(const std::string& token) {
    return token == "AND" || token == "OR";
}

int precedencia(const std::string& op) {
    if (op == "AND") return 2;
    if (op == "OR") return 1;
    return 0;
}

std::vector<std::string> tokenizarConsulta(const std::string& consulta) {
    std::vector<std::string> tokens;
    std::stringstream ss(consulta);
    std::string token;

    while (ss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

std::vector<std::string> infijoAPostfijo(const std::vector<std::string>& tokens) {
    std::vector<std::string> salida;
    std::stack<std::string> pila;         // Se usa en la conversion infijo a posfijo. El algoritmo Shunting Yard necesita una pila para guardar temporalmente los operadores (AND, OR) y sacarlos segun su prioridad.

    for (const std::string& token : tokens) {
        if (!esOperador(token)) {
            salida.push_back(token);
        }
        else {
            while (!pila.empty() && precedencia(pila.top()) >= precedencia(token)) {
                salida.push_back(pila.top());
                pila.pop();
            }
            pila.push(token);
        }
    }

    while (!pila.empty()) {
        salida.push_back(pila.top());
        pila.pop();
    }

    return salida;
}

std::set<int> unirSets(const std::set<int>& a, const std::set<int>& b) {
    std::set<int> resultado;
    std::set_union(a.begin(), a.end(),
        b.begin(), b.end(),
        std::inserter(resultado, resultado.begin()));
    return resultado;
}

std::set<int> intersectarSets(const std::set<int>& a, const std::set<int>& b) {
    std::set<int> resultado;
    std::set_intersection(a.begin(), a.end(),
        b.begin(), b.end(),
        std::inserter(resultado, resultado.begin()));
    return resultado;
}

std::set<int> evaluarPostfija(const std::vector<std::string>& postfija, const IndiceConsulta& indice) {
    std::stack<std::set<int>> pila;       // Se usa al evaluar la expresion postfija. Cuando aparaece una palabra, se mete en la pila su conjunto de documentos. Cuando aparece un operador se sacan los dos conjuntos de arriba, se hace interseccion o union y se vuelve a meter el resultado. 

    for (const std::string& token : postfija) {
        if (!esOperador(token)) {
            auto it = indice.find(token);
            if (it != indice.end()) {
                pila.push(it->second);
            }
            else {
                pila.push(std::set<int>()); // palabra no encontrada => conjunto vacío
            }
        }
        else {
            if (pila.size() < 2) {
                return std::set<int>();
            }

            std::set<int> derecha = pila.top();
            pila.pop();
            std::set<int> izquierda = pila.top();
            pila.pop();

            if (token == "AND") {
                pila.push(intersectarSets(izquierda, derecha));
            }
            else if (token == "OR") {
                pila.push(unirSets(izquierda, derecha));
            }
        }
    }

    if (pila.empty()) return std::set<int>();
    return pila.top();
}

std::string vectorAString(const std::vector<std::string>& v) {
    std::string resultado;
    for (size_t i = 0; i < v.size(); i++) {
        resultado += v[i];
        if (i != v.size() - 1) resultado += " ";
    }
    return resultado;
}

std::string setAString(const std::set<int>& s) {
    std::string resultado = "[";
    size_t contador = 0;

    for (int x : s) {
        resultado += std::to_string(x);
        if (contador != s.size() - 1) resultado += ", ";
        contador++;
    }

    resultado += "]";
    return resultado;
}

std::string setAStringCompact(const std::set<int>& s) {
    std::string resultado = "[";
    size_t contador = 0;

    for (int x : s) {
        resultado += std::to_string(x);
        if (contador != s.size() - 1) resultado += ",";
        contador++;
    }

    resultado += "]";
    return resultado;
}

std::string construirExplicacion(const std::vector<std::string>& tokens, const IndiceConsulta& indice) {
    std::vector<std::string> operandos;
    int numAND = 0;
    int numOR = 0;

    for (const std::string& token : tokens) {
        if (token == "AND") {
            numAND++;
        }
        else if (token == "OR") {
            numOR++;
        }
        else {
            auto it = indice.find(token);
            if (it != indice.end()) {
                operandos.push_back(setAStringCompact(it->second));
            }
            else {
                operandos.push_back("[]");
            }
        }
    }

    if (numAND > 0 && numOR > 0) {
        return "AND evalua antes que OR";
    }

    if (numAND > 0) {
        std::string explicacion = "interseccion de ";
        for (size_t i = 0; i < operandos.size(); i++) {
            explicacion += operandos[i];
            if (i != operandos.size() - 1) {
                if (i == operandos.size() - 2) explicacion += " y ";
                else explicacion += ", ";
            }
        }
        return explicacion;
    }

    if (numOR > 0) {
        std::string explicacion = "union de ";
        for (size_t i = 0; i < operandos.size(); i++) {
            explicacion += operandos[i];
            if (i != operandos.size() - 1) {
                if (i == operandos.size() - 2) explicacion += " y ";
                else explicacion += ", ";
            }
        }
        return explicacion;
    }

    return "";
}

void procesarConsultas(const std::string& archivoIndice,
    const std::string& archivoConsultas,
    const std::string& archivoSalida) {
    IndiceConsulta indice = leerIndiceConsulta(archivoIndice);

    std::ifstream archivoConsultasIn(archivoConsultas);
    if (!archivoConsultasIn.is_open()) {
        std::cerr << "Error: no se pudo abrir " << archivoConsultas << "\n";
        return;
    }

    std::ofstream archivoSalidaOut(archivoSalida);
    if (!archivoSalidaOut.is_open()) {
        std::cerr << "Error: no se pudo crear " << archivoSalida << "\n";
        return;
    }

    archivoSalidaOut << "-- Fichero: resultados_consultas.txt --\n\n";

    std::string consulta;
    while (std::getline(archivoConsultasIn, consulta)) {
        if (consulta.empty()) continue;

        std::vector<std::string> tokens = tokenizarConsulta(consulta);
        std::vector<std::string> postfija = infijoAPostfijo(tokens);
        std::set<int> resultado = evaluarPostfija(postfija, indice);
        std::string explicacion = construirExplicacion(tokens, indice);

        archivoSalidaOut << "Consulta: " << consulta << "\n";
        archivoSalidaOut << " Postfija: " << vectorAString(postfija) << "\n";
        archivoSalidaOut << " Resultado: " << setAString(resultado);

        if (!explicacion.empty()) {
            archivoSalidaOut << " <- " << explicacion;
        }

        archivoSalidaOut << "\n";
    }

    archivoConsultasIn.close();
    archivoSalidaOut.close();
}

void procesarPeticiones(const std::string& archivoIndice,
    const std::string& archivoPeticiones,
    const std::string& archivoSalida)
{
    using namespace std::chrono;

    // 1. Cargar índice invertido
    IndiceConsulta indice = leerIndiceConsulta(archivoIndice);

    std::ifstream archivoIn(archivoPeticiones);
    std::ofstream archivoOut(archivoSalida);

    if (!archivoIn.is_open()) {
        std::cerr << "Error al abrir peticiones\n";
        return;
    }

    if (!archivoOut.is_open()) {
        std::cerr << "Error al crear salida\n";
        return;
    }

    archivoOut << "-- Fichero de salida: log_procesamiento.txt --\n\n";

    // 2. Cola de peticiones
    struct Peticion {
        std::string hora;
        std::string usuario;
        std::string consulta;
    };

    std::queue<Peticion> cola; //Creacion de la cola FIFO

    std::string linea;
 
    while (std::getline(archivoIn, linea)) //Leemos cada linea del archivo
    {
        if (linea.empty() || linea.rfind("--", 0) == 0) //ignoramos huecos y el titulo
            continue;

        std::stringstream ss(linea); //Separacion de la linea dependiendo del dato

        Peticion p;
        std::string separador;

        // Formato: hora | usuario | consulta
        std::getline(ss, p.hora, '|');
        std::getline(ss, p.usuario, '|');
        std::getline(ss, p.consulta);

        // limpiar espacios
        auto trim = [](std::string& s)
            {
                while (!s.empty() && s.front() == ' ') s.erase(0, 1);
                while (!s.empty() && s.back() == ' ') s.pop_back();
            };

        trim(p.hora);
        trim(p.usuario);
        trim(p.consulta);

        cola.push(p);
    }

    archivoIn.close();

    int total = 0;

    while (!cola.empty()) //Mientras haya peticiones que se ejecute
    {
        Peticion p = cola.front(); //Miramos la primera linea y la eliminamos de la cola
        cola.pop();

        total++;
        auto inicio = high_resolution_clock::now(); //Calculo del tiempo
        // Trabajo de busqueda
        std::vector<std::string> tokens = tokenizarConsulta(p.consulta);
        std::vector<std::string> postfija = infijoAPostfijo(tokens);
        std::set<int> resultado = evaluarPostfija(postfija, indice);
       
        auto fin = high_resolution_clock::now();
        auto duracion = duration_cast<microseconds>(fin - inicio); //Vemos cuanto duro
        // Escribimos el archivo
        archivoOut << "[" << p.hora << "] PROCESANDO -> "
            << p.usuario << ": '" << p.consulta << "'\n";

        archivoOut << "-> Documentos encontrados: "
            << setAString(resultado)
            << " | Tiempo: "
            << duracion.count()
            << "microsec\n\n";
    }
    // ESTADÍSTICAS FINALES
    archivoOut << "Total peticiones procesadas: " << total << "\n";
    archivoOut << "Orden de procesamiento respetado: SI\n";

    archivoOut.close();
}