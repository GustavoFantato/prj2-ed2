#include "functions.h"
#include "utils.h"

/*
# FUNCIONALIDADE [9] - Atualizacao de registro de dados #
-> Desde que os registros de dados sao de tamanho fixo, a atualizacao deve ser feita diretamente no registro existente que nao esteja marcado como removido
-> O lixo que porventura permanecer no registro atualizado deve ser identificado pelo caracter '$'
-> As chaves de busca referentes aos registros que foram atualizados devem ser atualizadas no arquivo Index
-> Os registros a serem atualizados devem ser aqueles que satisfaçam um criterio de busca determinado pelo usuario
-> Qualquer campo pode ser usado como forma de atualizacao
-> O campo a ser utilizado como busca nao necessariamente deve ser atualizado
-> Campos a serem atualizados com valores nulos devem ser identificados, na entrada da funcionalidade, como NULO
-> Em situacoes nas quais um determinado criterio de busca nao seja satisfeito, ou seja, caso a solicitação do usuário não retorne nenhum registro a ser atualizado, o programa deve continuar a executar as atualizações até completar as n vezes seguidas
-> Antes de terminar a execucao da funcionalidade, deve usar binarioNaTela()

 | arquivoEntrada.bin: arquivo binario gerado pela funcionalidade [1]
 | arquivoIndex.bin: arquivo binario gerado pela funcionalidade [5]
 | n: numero de vezes a se repetir
 | m: m é a quantidade de vezes que o par nomeCampoB (ou seja, nome do campo de busca) e valorCampoB (ou seja, valor do campo de busca) pode repetir na busca pelos registros a serem atualizados. Deve ser deixado um espaço em branco entre nomeCampoB e valorCampoB. Os valores dos campos do tipo string devem ser especificados entre aspas duplas (").
 | p: p é a quantidade de vezes que o par nomeCampoA (nome do campo a ser atualizado) e valorCampoA (valor do campo a ser atualizado) pode repetir. Deve ser deixado um espaço em branco entre nomeCampoA e valorCampoA. Os valores dos campos do tipo string devem ser especificados entre aspas duplas (")

 CASO SUCESSO: binarioNaTela() do arquivo de dados e o arquivo Index
 CASO FALHE: "Falha no processamento do arquivo"


*/

void updateTable(char *arquivoDados, char *arquivoIndex, int n){}
