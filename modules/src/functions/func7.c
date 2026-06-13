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

void deleteFromTable(char *arquivoDados, char *arquivoIndex, int n) {
    
    // Abre arquivo de dados para Leitura e Escrita (rb+)
    FILE *binFile = fopen(arquivoDados, "rb+");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    char status;
    fread(&status, sizeof(char), 1, binFile);
    if (status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(binFile);
        return;
    }

    // Lê o arquivo de Indice
    FILE *indexFile = fopen(arquivoIndex, "rb");
    if (indexFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        fclose(binFile);
        return;
    }

    char indexStatus;
    fread(&indexStatus, sizeof(char), 1, indexFile);
    if (indexStatus == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(binFile);
        fclose(indexFile);
        return;
    }

    // 1. Marcar arquivo de dados como inconsistente ('0') durante a transacao
    status = '0';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, binFile);

    // Carrega o topo da Pilha (Lembrando: no seu structs.h o topo é um Byte Offset)
    int topo;
    fread(&topo, sizeof(int), 1, binFile);

    // 2. Carregar todo o Indice para a RAM para facilitar as remocoes cruzadas
    fseek(indexFile, INDEX_HEADER_SIZE, SEEK_SET);
    IndexRecord *indexList = NULL;
    int qtdIndex = 0;
    IndexRecord tempIdx;
    
    // Aloca dinamicamente o array de indice
    while (fread(&tempIdx.codEstacao, sizeof(int), 1, indexFile) == 1) {
        fread(&tempIdx.RRN, sizeof(int), 1, indexFile);
        qtdIndex++;
        indexList = realloc(indexList, qtdIndex * sizeof(IndexRecord));
        indexList[qtdIndex - 1] = tempIdx;
    }
    fclose(indexFile); // Vamos recriar o indice limpo no final da funcionalidade

    // 3. Processar as 'n' deleções
    for (int i = 0; i < n; i++) {
        
        int m;
        scanf("%d", &m); 

        char campos[8][30];
        char valores[8][100];
        
        int useIndexFile = 0;
        int indiceCodEst = -1;

        // Leitura hibrida dos filtros de remocao (ignorando o bug do utils.c)
        for (int j = 0; j < m; j++) {
            scanf("%s", campos[j]); 
            if (strcmp(campos[j], "codEstacao") == 0) {
                useIndexFile = 1;
                indiceCodEst = j;
            }     
            
            if (strcmp(campos[j], "nomeEstacao") == 0 || strcmp(campos[j], "nomeLinha") == 0) {
                ScanQuoteString(valores[j]); 
            } else { 
                scanf("%s", valores[j]);
                if (strcmp(valores[j], "NULO") == 0) {
                    strcpy(valores[j], "");
                }
            }
        }

        // ==========================================
        // OPCAO A: Delecao com Apoio do Indice O(1)
        // ==========================================
        if (useIndexFile) {
            int codBusca = atoi(valores[indiceCodEst]);
            int targetRRN = -1;
            
            // Busca o RRN na nossa lista carregada na RAM
            for (int idx = 0; idx < qtdIndex; idx++) {
                if (indexList[idx].codEstacao == codBusca) {
                    targetRRN = indexList[idx].RRN;
                    break;
                }
            }

            if (targetRRN != -1) { // Se a estacao existe no indice
                int offset = DATA_HEADER_SIZE + (targetRRN * DATA_REGISTER_SIZE);
                fseek(binFile, offset, SEEK_SET);
                
                char removed;
                fread(&removed, sizeof(char), 1, binFile);
                
                if (removed == '0') {
                    DataRecord data;
                    data.removido = removed;
                    lerRegistro(&data, binFile);
                    
                    // Dupla checagem: Garante que os outros campos do filtro tambem batem
                    int match = 1;
                    for (int f = 0; f < m; f++) {
                        checkMatch(data, campos, f, valores, &match);
                        if (!match) break;
                    }

                    if (match) {
                        // Faz a Remocao Logica e o Encadeamento da Pilha no Disco
                        fseek(binFile, offset, SEEK_SET);
                        char mark = '1';
                        fwrite(&mark, sizeof(char), 1, binFile);
                        fwrite(&topo, sizeof(int), 1, binFile); // O 'proximo' aponta para o topo antigo
                        topo = offset; // O Topo passa a ser o Byte Offset dessa nova estacao removida

                        // Remove a estacao da lista do indice na RAM (Shift Left)
                        for (int idx = 0; idx < qtdIndex; idx++) {
                            if (indexList[idx].codEstacao == codBusca) {
                                for (int k = idx; k < qtdIndex - 1; k++) {
                                    indexList[k] = indexList[k + 1];
                                }
                                qtdIndex--;
                                break;
                            }
                        }
                    }
                    if (data.nomeEstacao != NULL) free(data.nomeEstacao);
                    if (data.nomeLinha != NULL) free(data.nomeLinha);
                }
            }
        } 
        // ==========================================
        // OPCAO B: Delecao via Full Table Scan O(n)
        // ==========================================
        else { 
            fseek(binFile, DATA_HEADER_SIZE, SEEK_SET);
            char removed;
            
            while (fread(&removed, sizeof(char), 1, binFile) == 1) {
                if (removed == '1') {
                    fseek(binFile, DATA_REGISTER_SIZE - 1, SEEK_CUR);
                    continue;
                }

                long currentOffset = ftell(binFile) - 1; // Salva fisicamente o offset de inicio do registro

                DataRecord data;
                data.removido = removed;
                lerRegistro(&data, binFile);

                int garbageBytes = DATA_REGISTER_SIZE - (DATA_FIX_SIZE_FIELDS + data.tamNomeEstacao + data.tamNomeLinha);
                fseek(binFile, garbageBytes, SEEK_CUR); // Pula o lixo para alinhar o while do disco

                int match = 1; 
                for (int f = 0; f < m; f++) {
                    checkMatch(data, campos, f, valores, &match);
                    if (!match) break;
                }

                if (match) {
                    long nextRegOffset = ftell(binFile); // Guarda onde o While deve continuar a leitura depois da nossa edicao

                    // Faz a Remocao Logica e o Encadeamento da Pilha no Disco
                    fseek(binFile, currentOffset, SEEK_SET);
                    char mark = '1';
                    fwrite(&mark, sizeof(char), 1, binFile);
                    fwrite(&topo, sizeof(int), 1, binFile);
                    topo = (int)currentOffset; 

                    // Remove do Indice em RAM
                    for (int idx = 0; idx < qtdIndex; idx++) {
                        if (indexList[idx].codEstacao == data.codEstacao) {
                            for (int k = idx; k < qtdIndex - 1; k++) {
                                indexList[k] = indexList[k + 1];
                            }
                            qtdIndex--;
                            break;
                        }
                    }

                    fseek(binFile, nextRegOffset, SEEK_SET); // Restaura o ponteiro para continuarmos a varredura
                }
                
                if (data.nomeEstacao != NULL) free(data.nomeEstacao);
                if (data.nomeLinha != NULL) free(data.nomeLinha);
            }
        }
    }

    // 4. FLUSH FINAL DAS INFORMACOES NO DISCO
    
    // Atualiza o cabecalho do arquivo de dados (Status = '1' e o novo Topo Encadeado)
    status = '1';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, binFile);
    fwrite(&topo, sizeof(int), 1, binFile);
    fclose(binFile);

    // Recria e sobrescreve o IndexFile inteiro (agora limpo, sem os deletados) a partir da nossa RAM
    indexFile = fopen(arquivoIndex, "wb");
    char indexStatusOut = '0';
    fwrite(&indexStatusOut, sizeof(char), 1, indexFile);
    
    for (int i = 0; i < qtdIndex; i++) {
        fwrite(&indexList[i].codEstacao, sizeof(int), 1, indexFile);
        fwrite(&indexList[i].RRN, sizeof(int), 1, indexFile);
    }
    
    indexStatusOut = '1';
    fseek(indexFile, 0, SEEK_SET);
    fwrite(&indexStatusOut, sizeof(char), 1, indexFile);
    fclose(indexFile);

    // Previne vazamento de memoria
    if (indexList != NULL) free(indexList);

    // Saida exigida pelo projeto para validar as alteracoes de ambos os arquivos
    binarioNaTela(arquivoDados);
    binarioNaTela(arquivoIndex);
}
