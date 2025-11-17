#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Detective Quest - Mapa da Mansão + Coleta de Pistas (BST)
 *
 * Estruturas:
 *  - Sala: nó da árvore binária que representa um cômodo da mansão.
 *  - NoPista: nó da BST que armazena pistas coletadas (ordenadas alfabeticamente).
 *
 * Funções principais:
 *  - criarSala()            : cria dinamicamente uma sala (nome + pista).
 *  - inserirPistaBST()      : insere uma pista na BST (evita duplicatas).
 *  - explorarSalasComPistas(): navegação interativa e coleta automática de pistas.
 *  - exibirPistas()         : imprime as pistas coletadas em ordem alfabética.
 *  - liberarSalas() / liberarPistas(): limpa memória alocada.
 */

/* ----------------------------- Estruturas ----------------------------- */

/* Nó da árvore que representa uma sala */
typedef struct Sala {
    char nome[64];
    char pista[128];    /* pista opcional; string vazia se não houver pista */
    int coletada;       /* flag para evitar coletar a mesma pista várias vezes */
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

/* Nó da BST que armazena pistas */
typedef struct NoPista {
    char *pista;              /* armazenada dinamicamente (strdup) */
    struct NoPista *esq;
    struct NoPista *dir;
} NoPista;

/* ----------------------------- Utilitários ----------------------------- */

/* cria dinamicamente uma sala com nome e pista (pista pode ser NULL ou string vazia) */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro: falha na alocação de memória para sala.\n");
        exit(EXIT_FAILURE);
    }
    strncpy(s->nome, nome, sizeof(s->nome)-1);
    s->nome[sizeof(s->nome)-1] = '\0';

    if (pista && pista[0] != '\0') {
        strncpy(s->pista, pista, sizeof(s->pista)-1);
        s->pista[sizeof(s->pista)-1] = '\0';
    } else {
        s->pista[0] = '\0';
    }

    s->coletada = 0;
    s->esquerda = s->direita = NULL;
    return s;
}

/* Cria nó de pista (assume pista não-nula). */
NoPista* criarNoPista(const char *pista) {
    NoPista *n = (NoPista*) malloc(sizeof(NoPista));
    if (!n) {
        fprintf(stderr, "Erro: falha na alocação de memória para NoPista.\n");
        exit(EXIT_FAILURE);
    }
    n->pista = strdup(pista); /* copia dinâmica */
    if (!n->pista) {
        fprintf(stderr, "Erro: falha na alocação de memória para string da pista.\n");
        exit(EXIT_FAILURE);
    }
    n->esq = n->dir = NULL;
    return n;
}

/* Comparador de strings case-insensitive para ordenar alfabeticamente de forma mais amigável.
   Retorna <0 se a < b, 0 se iguais, >0 se a > b. */
int compararPistas(const char *a, const char *b) {
    /* comparação case-insensitive */
    while (*a && *b) {
        char ca = tolower((unsigned char)*a);
        char cb = tolower((unsigned char)*b);
        if (ca != cb) return (ca - cb);
        a++; b++;
    }
    return (unsigned char)tolower((unsigned char)*a) - (unsigned char)tolower((unsigned char)*b);
}

/* ----------------------------- BST de Pistas ----------------------------- */

/* Insere uma nova pista na BST (evita duplicatas). Retorna a raiz (possivelmente nova). */
NoPista* inserirPistaBST(NoPista *raiz, const char *pista) {
    if (!pista || pista[0] == '\0') return raiz; /* não inserir pistas vazias */

    if (raiz == NULL) {
        return criarNoPista(pista);
    }

    int cmp = compararPistas(pista, raiz->pista);
    if (cmp < 0) {
        raiz->esq = inserirPistaBST(raiz->esq, pista);
    } else if (cmp > 0) {
        raiz->dir = inserirPistaBST(raiz->dir, pista);
    } else {
        /* duplicata: não inserir */
        /* opcional: poderia contar ocorrências, mas requisito pede apenas organização */
    }
    return raiz;
}

/* Imprime as pistas em ordem alfabética (in-order traversal). */
void exibirPistas(NoPista *raiz) {
    if (!raiz) return;
    exibirPistas(raiz->esq);
    printf(" - %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

/* Libera memória da BST */
void liberarPistas(NoPista *raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz->pista);
    free(raiz);
}

/* ----------------------------- Exploração ----------------------------- */

/*
 * explorarSalasComPistas:
 *  - percorre a árvore de salas interativamente a partir da sala inicial.
 *  - ao entrar em uma sala com pista não coletada, insere a pista na BST automaticamente.
 *  - opções do jogador: 'e' (esquerda), 'd' (direita), 's' (sair).
 *  - retorna a raiz da BST de pistas com as inserções realizadas.
 */
NoPista* explorarSalasComPistas(Sala *atual, NoPista *bstPistas) {
    if (!atual) {
        printf("Mapa vazio. Nada a explorar.\n");
        return bstPistas;
    }

    char escolha;
    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);

        /* se existir pista e não tiver sido coletada, coletar automaticamente */
        if (atual->pista[0] != '\0' && !atual->coletada) {
            printf("Você encontrou uma pista: \"%s\"\n", atual->pista);
            bstPistas = inserirPistaBST(bstPistas, atual->pista);
            atual->coletada = 1;
        } else if (atual->pista[0] != '\0' && atual->coletada) {
            printf("Esta sala já teve sua pista coletada.\n");
        } else {
            printf("Nenhuma pista nesta sala.\n");
        }

        /* mostra opções */
        printf("\nEscolha um caminho:\n");
        if (atual->esquerda) printf("  (e) Ir para a esquerda -> %s\n", atual->esquerda->nome);
        else printf("  (e) Ir para a esquerda -> (não disponível)\n");
        if (atual->direita) printf("  (d) Ir para a direita -> %s\n", atual->direita->nome);
        else printf("  (d) Ir para a direita -> (não disponível)\n");
        printf("  (s) Sair e mostrar todas as pistas coletadas\n");
        printf("Sua escolha: ");

        if (scanf(" %c", &escolha) != 1) {
            /* entrada inválida: limpar stdin e pedir novamente */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("Entrada inválida. Tente novamente.\n");
            continue;
        }

        if (escolha == 'e' || escolha == 'E') {
            if (atual->esquerda) {
                atual = atual->esquerda;
            } else {
                printf("Não há caminho à esquerda nesta sala.\n");
            }
        } else if (escolha == 'd' || escolha == 'D') {
            if (atual->direita) {
                atual = atual->direita;
            } else {
                printf("Não há caminho à direita nesta sala.\n");
            }
        } else if (escolha == 's' || escolha == 'S') {
            printf("Saindo da exploração...\n");
            break;
        } else {
            printf("Opção inválida. Use 'e', 'd' ou 's'.\n");
        }
    }

    return bstPistas;
}

/* ----------------------------- Limpeza ----------------------------- */

/* Libera todas as salas (árvore binária) */
void liberarSalas(Sala *raiz) {
    if (!raiz) return;
    liberarSalas(raiz->esquerda);
    liberarSalas(raiz->direita);
    free(raiz);
}

/* ----------------------------- Exemplo de mapa (main) ----------------------------- */

int main(void) {
    /* Montagem manual da mansão (fixa) com pistas associadas */
    Sala *hall            = criarSala("Hall de Entrada", "Bilhete com a letra A");
    Sala *salaEstar       = criarSala("Sala de Estar", "Pegadas de lama");
    Sala *cozinha         = criarSala("Cozinha", "Faca com cabo quebrado");
    Sala *biblioteca      = criarSala("Biblioteca", "Página arrancada de um diário");
    Sala *jardim          = criarSala("Jardim", "Rastro de pneus");
    Sala *porao           = criarSala("Porão", "Frascos químicos vazios");
    Sala *quartoSecreto   = criarSala("Quarto Secreto", "Relógio parado às 03:15");
    Sala *banheiro        = criarSala("Banheiro", "Lenço com perfume floral");
    Sala *escritorio      = criarSala("Escritório", "Chave dourada");

    /* Montando conexões (árvore binária):
     *
     *                   Hall
     *                 /      \
     *          Sala Estar    Cozinha
     *          /     \         /   \
     *  Biblioteca  Jardim   Porão  Banheiro
     *                         \        \
     *                   Quarto Secreto Escritório
     *
     */

    hall->esquerda = salaEstar;
    hall->direita  = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;

    cozinha->esquerda = porao;
    cozinha->direita  = banheiro;

    porao->direita = quartoSecreto;
    banheiro->direita = escritorio;

    /* BST vazia inicialmente */
    NoPista *bstPistas = NULL;

    printf("=== Detective Quest: Coleta de Pistas ===\n");
    printf("Iniciando exploração a partir do Hall de Entrada.\n");
    printf("Digite 'e' para esquerda, 'd' para direita, 's' para sair.\n");

    bstPistas = explorarSalasComPistas(hall, bstPistas);

    /* Exibir pistas coletadas em ordem alfabética */
    printf("\n=== Pistas coletadas (ordem alfabética) ===\n");
    if (!bstPistas) {
        printf("Nenhuma pista foi coletada.\n");
    } else {
        exibirPistas(bstPistas);
    }

    /* Limpeza de memória */
    liberarPistas(bstPistas);
    liberarSalas(hall);

    printf("\nFim do programa. Boa investigação!\n");
    return 0;
}

