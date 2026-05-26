# Nome do executável exigido pelo projeto
BINARY = programaTrab

# Caminhos exatos de acordo com a sua árvore
SRC_DIR = ./src
MODULES_SRC = ./modules/src
HEADERS_DIR = ./modules/headers

# Compilador e Flags (o -I aponta para onde estão os seus arquivos .h)
CC = gcc
FLAGS = -g -Wall -Werror -I$(HEADERS_DIR)

# Captura o programaTrab.c, o utils.c e TODOS os funcX.c dentro da subpasta de funções
SOURCES = $(SRC_DIR)/programaTrab.c \
          $(MODULES_SRC)/utils.c \
          $(wildcard $(MODULES_SRC)/functions/*.c)

# 1. REGRA DE COMPILAÇÃO (Obrigatória: 'make all')
all:
	$(CC) $(FLAGS) $(SOURCES) -o $(BINARY) -lm

# 2. REGRA DE EXECUÇÃO (Obrigatória: 'make run')
run:
	./$(BINARY)

# 3. REGRA PARA CRIAR O ZIP (Garante que envia a estrutura exata exigida)
zip: clean
	zip -r submissao.zip Makefile modules/ src/

# 4. LIMPEZA
clean:
	rm -f $(BINARY) submissao.zip