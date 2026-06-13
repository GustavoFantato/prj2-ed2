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

void listTableWhereIndex(char *arquivoDados, char *arquivoIndex, int n){
    
    // Abertura do arquivo binario para leitura
    FILE *binFile = fopen(arquivoDados, "rb");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Verificacao se o status do arquivo eh '1' (consistente)
    char status;
    fread(&status, sizeof(char), 1, binFile);
    if (status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(binFile);
        return;
    }

    // Loop para ler os n casos de busca
    for (int i = 0; i < n; i++) {
        
        // m eh o numero de campos a serem comparados para a busca
        int m;
        scanf("%d", &m); 

        // Campos e valores a serem comparados 
        char campos[8][30];
        char valores[8][100];
        
        // Bool para definir se percorreremos o arquivo todo ou usaremos o index File  
        int useIndexFile = 0;
        int indiceCodEst; // facilitar saber em qual indice de campos[j] armazenamos o codEstacao
        // Loop para ler os campos e valores, de acordo com o 'm' lido 
        for (int j = 0; j < m; j++) {
            scanf("%s", campos[j]); 

            if (strcmp(campos[j], "codEstacao") == 0) {
                useIndexFile = 1;
                indiceCodEst = j;
            }     

            // Se o campo for nomeEstacao ou nomeLinha, utiliza-se a funcao dada ScanQuoteString()
            if (strcmp(campos[j], "nomeEstacao") == 0 || strcmp(campos[j], "nomeLinha") == 0) {
                ScanQuoteString(valores[j]); 
            } 
            else { 
                scanf("%s", valores[j]);

                // Se nulo, coloca a string como vazia para facilitar a comparacao mais pra frente
                if (strcmp(valores[j], "NULO") == 0) {
                    strcpy(valores[j], "");
                }
            }
        } // Fim for de leitura dos m's

        // Posiciona o ponteiro no inicio dos registros, apos o header
        fseek(binFile, DATA_HEADER_SIZE, SEEK_SET);
        int foundReg = 0; // Bool para controle de registro encontrado ou nao
        char removed; // armazenar o campo se o registro foi removido ou nao     

        // Usamos o arquivo Index para a busca
        if(useIndexFile){

            int RRN = getRRNIndexFile(arquivoIndex, atoi(valores[indiceCodEst]));
            if(RRN == -2){
                printf("Falha no processamento do arquivo.\n");
                fclose(binFile);
                return; // Falha na leitura do arquivo index
            }

            if (RRN == -1){
                printf("Registro inexistente.\n");
                if (!(i == n-1)){
                    printf("\n"); // Formatacao pro run codes
                }
                continue; // codEstacao nao encontrado, pulamos pra proxima iteracao de n
            }

            // Posiciona ponteiro no registro desejado, de acordo com o RRN
            fseek(binFile, (DATA_HEADER_SIZE + (RRN * DATA_REGISTER_SIZE)),SEEK_SET);
           
            //LER byte do 'removido' para verificar se o registro esta ativo ou nao. Ja que achamos pelo indexFile, sabemos que o registro nao esta removido. Nao eh necessario verificar, mas lemos mesmo assim para posicionar o ponteiro no byte correto para ler o resto do registro
            char removed;
            fread(&removed, sizeof(char), 1, binFile);

            DataRecord data;
            data.removido = removed; // Inicializa a struct corretamente para evitar lixo
            lerRegistro(&data, binFile);
            int match = 1;

            for (int f = 0; f < m; f++) {
                checkMatch(data, campos, f, valores, &match);
                
                // Se nao der match, quebra o loop, evitando comparacoes desnecessarias
                if (match == 0) break; 
            }

            if(match){
                foundReg = 1;
                printRegistro(data);
            }

            // se deu match, ou nao, precisa liberar a memoria
            if (data.nomeEstacao != NULL) free(data.nomeEstacao);
            if (data.nomeLinha != NULL) free(data.nomeLinha);


        } else { // Se o codEstacao nao for criterio de busca, percorremos o arquivo de dados inteiro

        // Enquanto nao chegar ao fim do arquivo, le-se o campo de removido para verificar se o registro esta ativo ou nao.    
        while (fread(&removed, sizeof(char), 1, binFile) == 1) {
            
            // Se o campo foi removido pula para o proximo registro
            if (removed == '1') {
                fseek(binFile, 79, SEEK_CUR);
                continue;
            }

            // Criacao do data record, atribuindo o valor do campo removido que ja foi lido
            DataRecord data;
            data.removido = removed;

            lerRegistro(&data, binFile);

            // Guardar a quantidade de bytes de lixo, para posicionar o ponteiro no inicio do proximo registro
            int garbageBytes = DATA_REGISTER_SIZE - (DATA_FIX_SIZE_FIELDS + data.tamNomeEstacao + data.tamNomeLinha);
            fseek(binFile, garbageBytes, SEEK_CUR);

            int match = 1; 

            // Loop para comparar os campos lidos com os campos de busca, verificando se o registro atual do arquivo binario corresponde aos criterios de busca
            for (int f = 0; f < m; f++) {
                checkMatch(data, campos, f, valores, &match);
                // Se nao der match, quebra o loop, evitando comparacoes desnecessarias
                if (match == 0) break; 
            }

            // Se deu match, registro foi encontrado, printa os campos do registro
            if(match){
                foundReg = 1;
                printRegistro(data);  
            } 
            
            // se deu match, ou nao, precisa liberar a memoria
            if (data.nomeEstacao != NULL) free(data.nomeEstacao);
            if (data.nomeLinha != NULL) free(data.nomeLinha);
        } // Fim do WHile


        // Se nao encontrou nenhum registro APÓS percorrer o arquivo todo, printa a mensagem
        if (!foundReg) {
            printf("Registro inexistente.\n");
        }

        } // Fim do IF do tipo de busca
        
        // Formatacao adequada de print do '\n' para o runCodes
        if (!(i == n-1)){
            printf("\n");
        }

    } // Fim do for de 'n'

    // Fecha o arquivo
    fclose(binFile);

}