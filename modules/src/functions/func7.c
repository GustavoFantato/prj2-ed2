#include "functions.h"
#include "utils.h"

/*
# FUNCIONALIDADE [7] - Remover dados do arquivo de acordo com um criterio#
-> Utilizar da abordagem dinâmica de reaproveitamento de espaços de registros logicamente removidos
-> Deve ser implementada utilizando o conceito de pilha de registros logicamente removidos
-> Qualquer campo pode ser utilizado como forma de remocao
-> Ao remover um registro, os valores dos bytes referentes aos campos ja armazenados devem permanecer os mesmos, com excecao dos valores dos campos relacionados ao tratamento da lista encadeada
-> As chaves de busca referentes aos registros logicamente removidos devem ser removidos do arquivo de indice primario criado na funcionalidade [5]
-> Nas situacoes que o criterio de busca nao for satisfeito, ou seja, ha retorno de nenhum registro a ser removido, o programa deve continuar a executar remocoes ate completar as n vezes seguidas
-> Antes de terminar a execucao da funcionalidade, deve ser utilizada a funcao binarioNaTela()

 | arquivoEntrada.bin: arquivo binario gerado pela funcionalidade [1]
 | arquivoIndex.bin: arquivo binario gerado pela funcionalidade [5]
 | n: numero de remocoes a serem realizadas
 | m: quantidade de vezes que o par "nomeCampo" e "valorCampo" pode repetir na busca pelos registros a serem removidos 
    - deve ser deixado um espaço em branco entre nomeCampo e valorCampo
    - os valores dos campos do tipo string devem ser especificados entre aspas duplas ("")

 | CASO SUCESSO: Listar o arquivo de dados e o arquivo de indice no formato binario, usando binarioNaTela()
 | CASO FALHE: Falha no processamento do arquivo


*/

void deleteFromTable(char *arquivoDados, char *arquivoIndex, int n){}
