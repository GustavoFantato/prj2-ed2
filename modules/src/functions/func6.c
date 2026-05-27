#include "functions.h"
#include "utils.h"

/*
# FUNCIONALIDADE [6] - Listar dados de uma tabela a partir de um criterio de busca, com apoio de um indice primario # 

 | arquivoEntrada.bin: arquivo binario gerado pela funcionalidade [1]
 | arquivoIndex.bin: arquivo binario gerado pela funcionalidade [5]
 | n: numero de vezes a se repetir a busca
 | m: quantidade de vezes que o par "nomeCampo" e "valorCampo" pode repetir em uma busca. 
    - deve ser deixado um espaço em branco entre nomeCampo e valorCampo
    - os valores dos campos do tipo string devem ser especificados entre aspas duplas ("")
    - buscas por campos nulos devem ser especificados usando o valor NULO

  | Caso sucesso: 
        - Cada registro deve ser mostrado em uma unica linha e os seus campos devem ser mostrados de forma sequencial separado por um espaco em branco
        - Campos de tamanho fixo que tiverem o valor nulo devem ser exibidos da seguinte forma: ao inves de exibir -1, escreva NULO
        - Campos de tamanho variavel que tiverem valor nulo devem ser exibidos com: NULO
        - A ordem da exibicao dos campos dos registros deve ser: codEstacao, nomeEstacao, codLinha, nomeLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra
  | Caso nao seja encontrado ou o campo pertence a um registro removido:
        - "Registro inexistente"

m1 nomeCampo1 valorCampo1 ... nomeCampom1 valorCampom1
m2 nomeCampo1 valorCampo1 ... nomeCampom2 valorCampom2
...
mn nomeCampo1 valorCampo1 ... nomeCampomn valorCampomn

ENTRADA:
6 estacao.bin indexaEstacao.bin 1

SAIDA:
1 nomeEstacao "Luz"
9 Luz 1 Azul 10 NULO 4 55
55 Luz 4 Amarela 56 1257 1 9
111 Luz 7 Rubi 112 NULO NULO NULO
... 

*/

void listTableWhereIndex(char *arquivoDados, char *arquivoIndex, int n){}