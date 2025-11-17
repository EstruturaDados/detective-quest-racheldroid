#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//
// ===============================
//    FUNÇÃO STRDUP COMPATÍVEL
// ===============================
char* str_dup(const char *src) {
    if (src == NULL) return NULL;
    char *d = (char*) malloc(strlen(src) + 1);
    if (d == NULL) return NULL;
    strcpy(d, src);
    return d;
}

//
// ===============================
//     ESTRUTURA DA SALA
// ===============================
typedef struct Sala {
    char nome[50];
    char pista[100];
    struct Sala *esquerda;
    struct Sala *direita;
    int visitada; // Novo campo para controlar se a sala foi visitada
} Sala;

//
// ===============================
//   ESTRUTURA DA BST DE PISTAS
// ===============================
typedef struct NoPista {
    char *pista;
    struct NoPista *esq;
    struct NoPista *dir;
} NoPista;

//
// ===============================
//      HASH TABLE (PISTA → SUSPEITO)
// ===============================
typedef struct HashEntry {
    char *pista;
    char *suspeito;
    struct HashEntry *prox;
} HashEntry;

typedef struct {
    int tamanho;
    HashEntry **buckets;
} HashTable;

//
// ===================================================
//            FUNÇÃO: criarSala
// ===================================================
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (s == NULL) return NULL;
    
    strncpy(s->nome, nome, sizeof(s->nome) - 1);
    s->nome[sizeof(s->nome) - 1] = '\0';
    
    if (pista != NULL) {
        strncpy(s->pista, pista, sizeof(s->pista) - 1);
        s->pista[sizeof(s->pista) - 1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    
    s->esquerda = NULL;
    s->direita = NULL;
    s->visitada = 0; // Inicialmente não visitada
    return s;
}

//
// ===================================================
//           BST: inserirPista
// ===================================================
NoPista* inserirPista(NoPista *raiz, const char *pista) {
    if (raiz == NULL) {
        NoPista *novo = (NoPista*) malloc(sizeof(NoPista));
        if (novo == NULL) return NULL;
        novo->pista = str_dup(pista);
        novo->esq = novo->dir = NULL;
        return novo;
    }
    
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else if (cmp > 0)
        raiz->dir = inserirPista(raiz->dir, pista);
    // Se for igual, não insere duplicata
    
    return raiz;
}

//
// ===================================================
//        HASH TABLE — criar
// ===================================================
unsigned int hash(const char *str, int mod) {
    unsigned int h = 0;
    while (*str)
        h = (h * 31) + *str++;
    return h % mod;
}

HashTable* criarHash(int tamanho) {
    HashTable *h = (HashTable*) malloc(sizeof(HashTable));
    if (h == NULL) return NULL;
    
    h->tamanho = tamanho;
    h->buckets = (HashEntry**) calloc(tamanho, sizeof(HashEntry*));
    if (h->buckets == NULL) {
        free(h);
        return NULL;
    }
    return h;
}

//
// ===================================================
//        HASH TABLE — inserir pista → suspeito
// ===================================================
void hashInserir(HashTable *h, const char *pista, const char *suspeito) {
    if (h == NULL || pista == NULL || suspeito == NULL) return;
    
    unsigned int idx = hash(pista, h->tamanho);

    HashEntry *novo = (HashEntry*) malloc(sizeof(HashEntry));
    if (novo == NULL) return;
    
    novo->pista = str_dup(pista);
    novo->suspeito = str_dup(suspeito);
    novo->prox = h->buckets[idx];

    h->buckets[idx] = novo;
}

//
// ===================================================
//        HASH TABLE — procurar suspeito
// ===================================================
char* hashBuscar(HashTable *h, const char *pista) {
    if (h == NULL || pista == NULL) return NULL;
    
    unsigned int idx = hash(pista, h->tamanho);
    HashEntry *e = h->buckets[idx];

    while (e != NULL) {
        if (strcmp(e->pista, pista) == 0)
            return e->suspeito;
        e = e->prox;
    }
    return NULL;
}

//
// ===================================================
//     EXIBIR PISTAS EM ORDEM (IN-ORDER)
// ===================================================
void exibirPistas(NoPista *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esq);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

//
// ===================================================
//      EXPLORAÇÃO DA MANSÃO + COLETA DE PISTAS
// ===================================================
NoPista* explorar(Sala *atual, NoPista *arvorePistas, HashTable *hashSuspeitos) {
    char op;

    while (1) {
        printf("\n=== Você está em: %s ===\n", atual->nome);

        // Verificar se já coletou a pista desta sala
        if (!atual->visitada && strcmp(atual->pista, "") != 0) {
            printf("*** Pista encontrada: %s ***\n", atual->pista);
            arvorePistas = inserirPista(arvorePistas, atual->pista);
            atual->visitada = 1; // Marcar como visitada
        } else if (atual->visitada) {
            printf("(Esta sala já foi explorada)\n");
        }

        printf("\nEscolha:\n");
        printf(" (e) Ir para esquerda\n");
        printf(" (d) Ir para direita\n");
        printf(" (v) Voltar para o hall\n");
        printf(" (s) Sair da exploração\n");
        printf("Opção: ");
        scanf(" %c", &op);
        op = tolower(op);

        if (op == 's') {
            return arvorePistas;
        } else if (op == 'v') {
            // Para voltar ao hall, precisamos navegar de volta
            // Em uma implementação mais complexa, usaríamos uma pilha
            // Por enquanto, vamos apenas informar que não é suportado
            printf("Para voltar ao hall, navegue manualmente pelos cômodos.\n");
            continue;
        } else if (op == 'e') {
            if (atual->esquerda != NULL) {
                atual = atual->esquerda;
            } else {
                printf("🚫 Caminho indisponível para a esquerda!\n");
            }
        } else if (op == 'd') {
            if (atual->direita != NULL) {
                atual = atual->direita;
            } else {
                printf("🚫 Caminho indisponível para a direita!\n");
            }
        } else {
            printf("❌ Opção inválida! Use e, d, v ou s.\n");
        }
    }
}

//
// ===================================================
//      CONTAR PISTAS DO SUSPEITO
// ===================================================
int contarPistasSuspeito(NoPista *pistas, HashTable *hash, const char *suspeito) {
    if (pistas == NULL || hash == NULL || suspeito == NULL) return 0;
    
    int contador = 0;
    NoPista *stack[50];
    int top = 0;
    NoPista *curr = pistas;

    while (curr != NULL || top > 0) {
        while (curr != NULL) {
            stack[top++] = curr;
            curr = curr->esq;
        }
        curr = stack[--top];

        char *s = hashBuscar(hash, curr->pista);
        if (s != NULL && strcasecmp(s, suspeito) == 0)
            contador++;

        curr = curr->dir;
    }
    return contador;
}

//
// ===================================================
//      LISTAR SUSPEITOS DISPONÍVEIS
// ===================================================
void listarSuspeitos() {
    printf("\n=== SUSPEITOS POSSÍVEIS ===\n");
    printf("- Marcos\n");
    printf("- Ana\n");
    printf("- Carlos\n");
    printf("============================\n");
}

//
// ===================================================
//      LIBERAR MEMÓRIA
// ===================================================
void liberarArvorePistas(NoPista *raiz) {
    if (raiz == NULL) return;
    liberarArvorePistas(raiz->esq);
    liberarArvorePistas(raiz->dir);
    free(raiz->pista);
    free(raiz);
}

void liberarHash(HashTable *h) {
    if (h == NULL) return;
    
    for (int i = 0; i < h->tamanho; i++) {
        HashEntry *e = h->buckets[i];
        while (e != NULL) {
            HashEntry *temp = e;
            e = e->prox;
            free(temp->pista);
            free(temp->suspeito);
            free(temp);
        }
    }
    free(h->buckets);
    free(h);
}

void liberarMansao(Sala *raiz) {
    if (raiz == NULL) return;
    liberarMansao(raiz->esquerda);
    liberarMansao(raiz->direita);
    free(raiz);
}

//
// ===================================================
//                      MAIN
// ===================================================
int main() {
    printf("🕵️  DETECTIVE QUEST - O MISTÉRIO DA MANSÃO 🕵️\n");
    printf("=============================================\n\n");

    // ------------------ MANSÃO ------------------
    Sala *hall = criarSala("Hall de Entrada", "Pegadas de barro fresco");
    Sala *salaEstar = criarSala("Sala de Estar", "Copo quebrado");
    Sala *cozinha = criarSala("Cozinha", "Faca fora do lugar");
    Sala *biblioteca = criarSala("Biblioteca", "Livro arrancado da estante");
    Sala *jardim = criarSala("Jardim", "Luvas enterradas");
    Sala *quartosecreto = criarSala("Quarto Secreto", "Bilhete ameaçador");

    // Verificar se todas as salas foram criadas
    if (!hall || !salaEstar || !cozinha || !biblioteca || !jardim || !quartosecreto) {
        printf("Erro: Não foi possível criar todas as salas!\n");
        return 1;
    }

    // Ligações da árvore
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->direita = quartosecreto;

    // ------------------ TABELA HASH ------------------
    HashTable *hash = criarHash(50);
    if (hash == NULL) {
        printf("Erro: Não foi possível criar a tabela hash!\n");
        return 1;
    }

    hashInserir(hash, "Pegadas de barro fresco", "Marcos");
    hashInserir(hash, "Copo quebrado", "Ana");
    hashInserir(hash, "Faca fora do lugar", "Carlos");
    hashInserir(hash, "Livro arrancado da estante", "Marcos");
    hashInserir(hash, "Luvas enterradas", "Ana");
    hashInserir(hash, "Bilhete ameaçador", "Carlos");

    // ------------------ EXPLORAÇÃO ------------------
    NoPista *pistas = NULL;
    printf("Explore a mansão para coletar pistas!\n");
    printf("Encontre pelo menos 2 pistas que apontem para o mesmo suspeito.\n\n");
    
    pistas = explorar(hall, pistas, hash);

    printf("\n\n===== PISTAS COLETADAS =====\n");
    if (pistas == NULL) {
        printf("Nenhuma pista foi coletada!\n");
    } else {
        exibirPistas(pistas);
    }

    // ------------------ ACUSAÇÃO FINAL ------------------
    char suspeito[50];
    printf("\n=== ACUSAÇÃO FINAL ===\n");
    listarSuspeitos();
    printf("\nQuem você acusa? ");
    scanf(" %[^\n]", suspeito);

    int contador = contarPistasSuspeito(pistas, hash, suspeito);

    printf("\n===== RESULTADO =====\n");
    printf("Pistas que apontam para %s: %d\n", suspeito, contador);
    
    if (contador >= 2) {
        printf("🎉 Acusação correta! %s é realmente o culpado.\n", suspeito);
        printf("A polícia prendeu o culpado baseado em suas evidências!\n");
    } else {
        printf("❌ Acusação incorreta! As pistas não provam que %s seja o culpado.\n", suspeito);
        printf("O verdadeiro assassino escapou... Tente novamente!\n");
    }

    // Liberar memória
    liberarArvorePistas(pistas);
    liberarHash(hash);
    liberarMansao(hall); // Libera toda a mansão a partir do hall

    return 0;
}