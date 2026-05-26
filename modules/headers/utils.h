#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"

// Prototipos das funcoes utilitarias

// Funcoes gerais
void printRegistro(DataRecord data);
void lerRegistro(DataRecord *data, FILE *binFile);


// Utilizadas na funcionalidade [1]
void binarioNaTela(char *arquivo);

char *readCSVLine(char line[], int line_size, FILE *csvFile);

int verifyIfNullField(char *field);

void switchDataRecord(DataRecord *data, int i, char *field);

void verifyIfDiffStation(char *name, char ***diffStationNames, int *stationsQtd);

void verifyIfDiffPair(int orig, int dest, Par **listPar, int *nPares);



// Utilizadas na funcionalidade [3]
void ScanQuoteString(char *str);

int getValue(char *valor);

int checkStringMatch(char *data_string, char *valor_busca);

void checkMatch(DataRecord data, char campos[][30], int f, char valores[][100], int *match);


#endif