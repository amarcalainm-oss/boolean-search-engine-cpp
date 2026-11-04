#include "headers.h"

int main()

{
	//BEÑAT
	procesarArchivo("corpus.txt");

	//AITOR
	procesarArchivo_Duplicados("corpus_con_duplicados.txt");

	//AITOR
	procesarConsultas("indice.txt", "consultas.txt", "resultados_consultas.txt");

	//Denok
	procesarPeticiones("indice.txt","peticiones.txt","log_procesamiento.txt");

	//Experimento 1 
	//1000 palabras
	// Se guarda en otro fichero para no sobrescribir el indice usado en las consultas
	procesarArchivo("corpus1000.txt", "indice_corpus1000.txt");

	return 0;
}