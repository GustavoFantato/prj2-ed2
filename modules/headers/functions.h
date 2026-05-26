#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "structs.h"

#define LINE_SIZE 1200 

// Protótipos das funções para cada funcionalidade


// FUNCIONALIDADE [1]
void createTable(char *arquivoEntrada, char *arquivoSaida);

// FUNCIONALIDADE [2]
void listTable(char *arquivoEntrada);

// FUNCIONALIDADE [3]
void listTableWhere(char *arquivoEntrada, int n);

// FUNCIONALIDADE [4]
void listTableRRN(char *arquivoEntrada, int RRN);

#endif