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

void insertIntoTable(char *arquivoDados, char *arquivoIndex, int n) {
    
    // Abre arquivo de dados para Leitura e Escrita (rb+)
    FILE *binFile = fopen(arquivoDados, "rb+");
    if (binFile == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    // Leitura completa do Header de Dados
    HeaderRecord header;
    fread(&header.status, sizeof(char), 1, binFile);
    if (header.status == '0') {
        printf("Falha no processamento do arquivo.\n");
        fclose(binFile);
        return;
    }
    fread(&header.topo, sizeof(int), 1, binFile);
    fread(&header.proxRRN, sizeof(int), 1, binFile);
    fread(&header.nroEstacoes, sizeof(int), 1, binFile);
    fread(&header.nroParesEstacao, sizeof(int), 1, binFile);

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
    header.status = '0';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&header.status, sizeof(char), 1, binFile);

    // 2. Carregar todo o Indice para a RAM
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

    // 3. Processar as 'n' inserções
    for (int i = 0; i < n; i++) {
        
        DataRecord data;
        memset(&data, 0, sizeof(DataRecord));
        data.removido = '0';
        data.proximo = -1;

        char strNomeEstacao[100], strNomeLinha[100];
        char strCodLinha[20], strCodProx[20], strDist[20], strCodLinInt[20], strCodEstInt[20];

        // Ordem fixa de leitura exigida: codEstacao, nomeEstacao, codLinha, nomeLinha, codProxEstacao, distProxEstacao, codLinhaIntegra, codEstIntegra
        scanf("%d", &data.codEstacao);

        ScanQuoteString(strNomeEstacao);
        if (strcmp(strNomeEstacao, "") == 0) {
            data.nomeEstacao = NULL;
            data.tamNomeEstacao = 0;
        } else {
            data.nomeEstacao = strdup(strNomeEstacao);
            data.tamNomeEstacao = strlen(strNomeEstacao);
        }

        scanf("%s", strCodLinha);
        data.codLinha = (strcmp(strCodLinha, "NULO") == 0) ? -1 : atoi(strCodLinha);

        ScanQuoteString(strNomeLinha);
        if (strcmp(strNomeLinha, "") == 0) {
            data.nomeLinha = NULL;
            data.tamNomeLinha = 0;
        } else {
            data.nomeLinha = strdup(strNomeLinha);
            data.tamNomeLinha = strlen(strNomeLinha);
        }

        scanf("%s", strCodProx);
        data.codProxEstacao = (strcmp(strCodProx, "NULO") == 0) ? -1 : atoi(strCodProx);

        scanf("%s", strDist);
        data.distProxEstacao = (strcmp(strDist, "NULO") == 0) ? -1 : atoi(strDist);

        scanf("%s", strCodLinInt);
        data.codLinhaIntegra = (strcmp(strCodLinInt, "NULO") == 0) ? -1 : atoi(strCodLinInt);

        scanf("%s", strCodEstInt);
        data.codEstIntegra = (strcmp(strCodEstInt, "NULO") == 0) ? -1 : atoi(strCodEstInt);

        int targetRRN;
        long writeOffset;

        // ==========================================
        // Logica de Insercao (Pilha vs Fim do Arquivo)
        // ==========================================
        if (header.topo != -1) {
            // Reaproveitamento de espaco
            writeOffset = header.topo;
            targetRRN = (writeOffset - DATA_HEADER_SIZE) / DATA_REGISTER_SIZE; // Calcula RRN pelo byte offset

            // Lemos o encadeamento antes de sobrescrever
            fseek(binFile, writeOffset + 1, SEEK_SET); // Pula a flag 'removido'
            int proximoRemovido;
            fread(&proximoRemovido, sizeof(int), 1, binFile);
            
            // O topo da nossa pilha passa a ser o proximo removido
            header.topo = proximoRemovido;
        } else {
            // Sem espaco reutilizavel, escrevemos no final do arquivo
            targetRRN = header.proxRRN;
            writeOffset = DATA_HEADER_SIZE + (targetRRN * DATA_REGISTER_SIZE);
            header.proxRRN++;
        }

        // ==========================================
        // Escrita do Registro no Disco
        // ==========================================
        fseek(binFile, writeOffset, SEEK_SET);

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

        // Padding (Lixo de Memoria)
        int garbageBytes = DATA_REGISTER_SIZE - (DATA_FIX_SIZE_FIELDS + data.tamNomeEstacao + data.tamNomeLinha);
        for(int k = 0; k < garbageBytes; k++){
            fputc('$', binFile); 
        }

        // ==========================================
        // Atualizacao do Indice na RAM
        // ==========================================
        qtdIndex++;
        indexList = realloc(indexList, qtdIndex * sizeof(IndexRecord));
        indexList[qtdIndex - 1].codEstacao = data.codEstacao;
        indexList[qtdIndex - 1].RRN = targetRRN;

        // Limpeza do Heap
        if (data.nomeEstacao != NULL) free(data.nomeEstacao);
        if (data.nomeLinha != NULL) free(data.nomeLinha);
    }

    // Ordenacao OBRIGATORIA do Indice apos todas as insercoes
    qsort(indexList, qtdIndex, sizeof(IndexRecord), compairRegisters);

    // 4. FLUSH FINAL DAS INFORMACOES NO DISCO
    
    // Atualiza o cabecalho de dados
    header.status = '1';
    fseek(binFile, 0, SEEK_SET);
    fwrite(&header.status, sizeof(char), 1, binFile);
    fwrite(&header.topo, sizeof(int), 1, binFile);
    fwrite(&header.proxRRN, sizeof(int), 1, binFile);
    // (nroEstacoes e nroParesEstacao permanecem intocados nesta operacao conforme as especificacoes)
    fclose(binFile);

    // Reescreve o Indice
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

    // Saida exigida pelo projeto
    binarioNaTela(arquivoDados);
    binarioNaTela(arquivoIndex);
}
