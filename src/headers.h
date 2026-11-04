#ifndef HEADERS_H
#define HEADERS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>


//BEÑAT

// Funcion principal que procesa el archivo y crea el indice invertido
void procesarArchivo(const std::string& nombreArchivo, const std::string& archivoSalida = "indice.txt");


//AITOR

void procesarArchivo_Duplicados(const std::string& nombreArchivo);


//IKER

using IndiceConsulta = std::unordered_map<std::string, std::set<int>>;           //set<int> guarda los IDs de documentos ordenados y sin repetidos
IndiceConsulta leerIndiceConsulta(const std::string& nombreArchivo);

bool esOperador(const std::string& token);
int precedencia(const std::string& op);
std::vector<std::string> tokenizarConsulta(const std::string& consulta);              //vector<string> se usa para guardar listas de tokens. Una consulta es una secuencia ordenada de elementos y vector es ideal para eso.
std::vector<std::string> infijoAPostfijo(const std::vector<std::string>& tokens);

std::set<int> unirSets(const std::set<int>& a, const std::set<int>& b);
std::set<int> intersectarSets(const std::set<int>& a, const std::set<int>& b);

std::set<int> evaluarPostfija(const std::vector<std::string>& postfija, const IndiceConsulta& indice);

std::string vectorAString(const std::vector<std::string>& v);
std::string setAString(const std::set<int>& s);

std::string setAStringCompact(const std::set<int>& s);
std::string construirExplicacion(const std::vector<std::string>& tokens, const IndiceConsulta& indice);

void procesarConsultas(const std::string& archivoIndice,
    const std::string& archivoConsultas,
    const std::string& archivoSalida);
void procesarPeticiones(const std::string& archivoIndice,
    const std::string& archivoPeticiones,
    const std::string& archivoSalida);

#endif