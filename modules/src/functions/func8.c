#include "functions.h"
#include "utils.h"

/*
# FUNCIONALIDADE [8] - Inserir registros em um arquivo de dados de entrada  #
-> Abordagem dinamica de reaproveitamento de espacos de registros logicamente removidos 
-> Deve-se utilizar da pilha de registros logicamente removidos
-> O lixo que permanece no registro removido e que nao eh reutilizado deve ser identificado pelo caracter '$'
-> As chaves de busca referentes aos novos registros devem ser inseridas no arquivo de indice primario
-> Na entrada desta funcionalidade, os dados são referentes aos seguintes campos, na seguinte ordem: codEstacao, nomeEstacao, codLinha, nomeLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra
-> Campos com valores nulos, na entrada da funcionalidade, devem ser identificados com NULO
-> Os valores dos campos do tipo string devem ser especificados entre aspas duplas ("")
-> Usar o scan_quote_string para manipulacao de strings com aspas duplas
-> no fim da funcionalidade, executar binarioNaTela()
-> cada uma das n insercoes deve ser especificada em uma linha diferente 

 | arquivoEntrada.bin: arquivo binario gerado pela funcionalidade [1]
 | arquivoIndex.bin: arquivo binario gerado pela funcionalidade [5]
 | n: numero de vezes a se repetir

 CASO SUCESSO: Listar o arquivo de dados e o arquivo de indice no formato binario, usando binarioNaTela()

 CASO ERRO: "Falha no processamento do arquivo."

*/

void insertIntoTable(char *arquivoDados, char *arquivoIndex, int n){}
