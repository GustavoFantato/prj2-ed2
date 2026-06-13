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

void updateTable(char *arquivoDados, char *arquivoIndex, int n) {
    
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

    // 1. Marcar arquivo de dados como inconsistente ('0')
    status = '0';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, binFile);

    // 2. Carregar o Indice para a RAM
    fseek(indexFile, INDEX_HEADER_SIZE, SEEK_SET);
    IndexRecord *indexList = NULL;
    int qtdIndex = 0;
    IndexRecord tempIdx;
    
    while (fread(&tempIdx.codEstacao, sizeof(int), 1, indexFile) == 1) {
        fread(&tempIdx.RRN, sizeof(int), 1, indexFile);
        qtdIndex++;
        indexList = realloc(indexList, qtdIndex * sizeof(IndexRecord));
        indexList[qtdIndex - 1] = tempIdx;
    }
    fclose(indexFile); 

    // 3. Processar as 'n' atualizacoes
    for (int i = 0; i < n; i++) {
        
        int m; // Quantidade de criterios de busca
        if (scanf("%d", &m) != 1) break;

        char camposBusca[8][30];
        char valoresBusca[8][100];
        int useIndexFile = 0;
        int indiceCodEst = -1;

        // Scanner Hibrido para os campos de BUSCA
        for (int j = 0; j < m; j++) {
            scanf("%s", camposBusca[j]); 
            if (strcmp(camposBusca[j], "codEstacao") == 0) {
                useIndexFile = 1;
                indiceCodEst = j;
            }     
            
            if (strcmp(camposBusca[j], "nomeEstacao") == 0 || strcmp(camposBusca[j], "nomeLinha") == 0) {
                ScanQuoteString(valoresBusca[j]); 
            } else { 
                scanf("%s", valoresBusca[j]);
                if (strcmp(valoresBusca[j], "NULO") == 0) {
                    strcpy(valoresBusca[j], "");
                }
            }
        }

        int p; // Quantidade de campos a serem atualizados
        scanf("%d", &p);

        char camposAtualiza[8][30];
        char valoresAtualiza[8][100];

        // Scanner Hibrido para os campos de ATUALIZACAO
        for (int j = 0; j < p; j++) {
            scanf("%s", camposAtualiza[j]); 
            if (strcmp(camposAtualiza[j], "nomeEstacao") == 0 || strcmp(camposAtualiza[j], "nomeLinha") == 0) {
                ScanQuoteString(valoresAtualiza[j]); 
            } else { 
                scanf("%s", valoresAtualiza[j]);
                if (strcmp(valoresAtualiza[j], "NULO") == 0) {
                    strcpy(valoresAtualiza[j], "");
                }
            }
        }

        // ==========================================
        // Logica de Varredura e Atualizacao
        // ==========================================
        fseek(binFile, DATA_HEADER_SIZE, SEEK_SET);
        int currentRRN = 0;
        char removed;

        // Vamos varrer o arquivo inteiro. Se usar indice, podemos otimizar o salto.
        if (useIndexFile) {
            int codBusca = atoi(valoresBusca[indiceCodEst]);
            int targetRRN = -1;
            
            for (int idx = 0; idx < qtdIndex; idx++) {
                if (indexList[idx].codEstacao == codBusca) {
                    targetRRN = indexList[idx].RRN;
                    break;
                }
            }
            if (targetRRN == -1) continue; // Nao achou no indice, pula pra proxima busca
            
            currentRRN = targetRRN;
            fseek(binFile, DATA_HEADER_SIZE + (targetRRN * DATA_REGISTER_SIZE), SEEK_SET);
        }

        while (fread(&removed, sizeof(char), 1, binFile) == 1) {
            
            if (removed == '1') {
                fseek(binFile, DATA_REGISTER_SIZE - 1, SEEK_CUR);
                currentRRN++;
                continue;
            }

            long currentOffset = ftell(binFile) - 1; // Salva o offset do registro atual

            DataRecord data;
            data.removido = removed;
            lerRegistro(&data, binFile);

            int garbageBytes = DATA_REGISTER_SIZE - (DATA_FIX_SIZE_FIELDS + data.tamNomeEstacao + data.tamNomeLinha);
            fseek(binFile, garbageBytes, SEEK_CUR);

            // Verifica se o registro atual da match com a busca
            int match = 1; 
            for (int f = 0; f < m; f++) {
                checkMatch(data, camposBusca, f, valoresBusca, &match);
                if (!match) break;
            }

            // Se for o registro certo, atualizamos!
            if (match) {
                long nextRegOffset = ftell(binFile); // Salva de onde o while deve continuar
                
                int alterouCodEstacao = 0;
                int oldCodEstacao = data.codEstacao;

                // Aplica os novos valores na RAM
                for (int k = 0; k < p; k++) {
                    if (strcmp(camposAtualiza[k], "codEstacao") == 0) {
                        data.codEstacao = (strcmp(valoresAtualiza[k], "") == 0) ? -1 : atoi(valoresAtualiza[k]);
                        alterouCodEstacao = 1;
                    } 
                    else if (strcmp(camposAtualiza[k], "codLinha") == 0) {
                        data.codLinha = (strcmp(valoresAtualiza[k], "") == 0) ? -1 : atoi(valoresAtualiza[k]);
                    }
                    else if (strcmp(camposAtualiza[k], "codProxEstacao") == 0) {
                        data.codProxEstacao = (strcmp(valoresAtualiza[k], "") == 0) ? -1 : atoi(valoresAtualiza[k]);
                    }
                    else if (strcmp(camposAtualiza[k], "distProxEstacao") == 0) {
                        data.distProxEstacao = (strcmp(valoresAtualiza[k], "") == 0) ? -1 : atoi(valoresAtualiza[k]);
                    }
                    else if (strcmp(camposAtualiza[k], "codLinhaIntegra") == 0) {
                        data.codLinhaIntegra = (strcmp(valoresAtualiza[k], "") == 0) ? -1 : atoi(valoresAtualiza[k]);
                    }
                    else if (strcmp(camposAtualiza[k], "codEstIntegra") == 0) {
                        data.codEstIntegra = (strcmp(valoresAtualiza[k], "") == 0) ? -1 : atoi(valoresAtualiza[k]);
                    }
                    else if (strcmp(camposAtualiza[k], "nomeEstacao") == 0) {
                        if (data.nomeEstacao != NULL) free(data.nomeEstacao);
                        if (strcmp(valoresAtualiza[k], "") == 0) {
                            data.nomeEstacao = NULL;
                            data.tamNomeEstacao = 0;
                        } else {
                            data.nomeEstacao = strdup(valoresAtualiza[k]);
                            data.tamNomeEstacao = strlen(valoresAtualiza[k]);
                        }
                    }
                    else if (strcmp(camposAtualiza[k], "nomeLinha") == 0) {
                        if (data.nomeLinha != NULL) free(data.nomeLinha);
                        if (strcmp(valoresAtualiza[k], "") == 0) {
                            data.nomeLinha = NULL;
                            data.tamNomeLinha = 0;
                        } else {
                            data.nomeLinha = strdup(valoresAtualiza[k]);
                            data.tamNomeLinha = strlen(valoresAtualiza[k]);
                        }
                    }
                }

                // Volta o ponteiro e sobrescreve In-Place
                fseek(binFile, currentOffset, SEEK_SET);

                fwrite(&data.removido, sizeof(char), 1, binFile);
                fwrite(&data.proximo, sizeof(int), 1, binFile);
                fwrite(&data.codEstacao, sizeof(int), 1, binFile);
                fwrite(&data.codLinha, sizeof(int), 1, binFile);
                fwrite(&data.codProxEstacao, sizeof(int), 1, binFile);
                fwrite(&data.distProxEstacao, sizeof(int), 1, binFile);
                fwrite(&data.codLinhaIntegra, sizeof(int), 1, binFile);
                fwrite(&data.codEstIntegra, sizeof(int), 1, binFile);
                
                fwrite(&data.tamNomeEstacao, sizeof(int), 1, binFile);
                if (data.tamNomeEstacao > 0) fwrite(data.nomeEstacao, sizeof(char), data.tamNomeEstacao, binFile);
                
                fwrite(&data.tamNomeLinha, sizeof(int), 1, binFile);
                if (data.tamNomeLinha > 0) fwrite(data.nomeLinha, sizeof(char), data.tamNomeLinha, binFile);

                // Recalcula e sobrescreve o lixo
                int newGarbageBytes = DATA_REGISTER_SIZE - (DATA_FIX_SIZE_FIELDS + data.tamNomeEstacao + data.tamNomeLinha);
                for(int k = 0; k < newGarbageBytes; k++){
                    fputc('$', binFile); 
                }

                // Atualiza a chave no Indice em RAM se ela mudou
                if (alterouCodEstacao) {
                    for (int idx = 0; idx < qtdIndex; idx++) {
                        if (indexList[idx].codEstacao == oldCodEstacao && indexList[idx].RRN == currentRRN) {
                            indexList[idx].codEstacao = data.codEstacao;
                            break;
                        }
                    }
                }

                fseek(binFile, nextRegOffset, SEEK_SET); // Restaura o ponteiro proximo registro

                // Se a busca inicial usou Indice (que garante achado unico), podemos quebrar o while aqui
                if (useIndexFile) {
                    if (data.nomeEstacao != NULL) free(data.nomeEstacao);
                    if (data.nomeLinha != NULL) free(data.nomeLinha);
                    break; 
                }
            }
            
            if (data.nomeEstacao != NULL) free(data.nomeEstacao);
            if (data.nomeLinha != NULL) free(data.nomeLinha);

            currentRRN++;
        }
    }

    // Ordenacao do Indice (Pois algumas chaves primarias podem ter sido alteradas)
    qsort(indexList, qtdIndex, sizeof(IndexRecord), compairRegisters);

    // 4. FLUSH FINAL DAS INFORMACOES NO DISCO
    
    status = '1';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&status, sizeof(char), 1, binFile);
    fclose(binFile);

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

    if (indexList != NULL) free(indexList);

    binarioNaTela(arquivoDados);
    binarioNaTela(arquivoIndex);
}
