// Desafio Detective Quest
// Tema 4 - Árvores e Tabela Hash
// Este código inicial serve como base para o desenvolvimento das estruturas de navegação, pistas e suspeitos.
// Use as instruções de cada região para desenvolver o sistema completo com árvore binária, árvore de busca e tabela hash.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------
// Estrutura da sala (nó da árvore binária)
// ---------------------------------------------
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda; 
    struct Sala *direita;
} Sala;

// -------------------------------------------------
// Função: criarSala
// Cria dinamicamente uma sala com nome especificado
// -------------------------------------------------
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));

    if (!nova) {
        printf("Erro ao alocar memória!\n");
        exit(1);
    }

    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;

    return nova;
}

// --------------------------------------------------------------------
// Função: explorarSalas
// Permite o jogador explorar a mansão a partir de uma sala (nó atual)
// O jogador escolhe 'e' para ir à esquerda, 'd' para a direita, ou 's' para sair
// A exploração termina quando o jogador chega a um nó-folha
// --------------------------------------------------------------------
void explorarSalas(Sala *atual) {
    char escolha;

    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Caso seja nó-folha
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Este cômodo não possui mais caminhos. Exploração encerrada!\n");
            return;
        }

        printf("Escolha um caminho:\n");
        printf("  (e) Ir para a esquerda\n");
        printf("  (d) Ir para a direita\n");
        printf("  (s) Sair da exploração\n");
        printf("Sua escolha: ");
        scanf(" %c", &escolha);

        switch (escolha) {
            case 'e':
            case 'E':
                if (atual->esquerda != NULL)
                    atual = atual->esquerda;
                else
                    printf("Não há caminho à esquerda!\n");
                break;

            case 'd':
            case 'D':
                if (atual->direita != NULL)
                    atual = atual->direita;
                else
                    printf("Não há caminho à direita!\n");
                break;

            case 's':
            case 'S':
                printf("Saindo da exploração...\n");
                return;

            default:
                printf("Opção inválida! Tente novamente.\n");
        }
    }
}
int main() {

    // 🌱 Nível Novato: Mapa da Mansão com Árvore Binária
    //
    // - Crie uma struct Sala com nome, e dois ponteiros: esquerda e direita.
    // - Use funções como criarSala(), conectarSalas() e explorarSalas().
    // - A árvore pode ser fixa: Hall de Entrada, Biblioteca, Cozinha, Sótão etc.
    // - O jogador deve poder explorar indo à esquerda (e) ou à direita (d).
    // - Finalize a exploração com uma opção de saída (s).
    // - Exiba o nome da sala a cada movimento.
    // - Use recursão ou laços para caminhar pela árvore.
    // - Nenhuma inserção dinâmica é necessária neste nível.

    // 🔍 Nível Aventureiro: Armazenamento de Pistas com Árvore de Busca
    //
    // - Crie uma struct Pista com campo texto (string).
    // - Crie uma árvore binária de busca (BST) para inserir as pistas coletadas.
    // - Ao visitar salas específicas, adicione pistas automaticamente com inserirBST().
    // - Implemente uma função para exibir as pistas em ordem alfabética (emOrdem()).
    // - Utilize alocação dinâmica e comparação de strings (strcmp) para organizar.
    // - Não precisa remover ou balancear a árvore.
    // - Use funções para modularizar: inserirPista(), listarPistas().
    // - A árvore de pistas deve ser exibida quando o jogador quiser revisar evidências.

    // 🧠 Nível Mestre: Relacionamento de Pistas com Suspeitos via Hash
    //
    // - Crie uma struct Suspeito contendo nome e lista de pistas associadas.
    // - Crie uma tabela hash (ex: array de ponteiros para listas encadeadas).
    // - A chave pode ser o nome do suspeito ou derivada das pistas.
    // - Implemente uma função inserirHash(pista, suspeito) para registrar relações.
    // - Crie uma função para mostrar todos os suspeitos e suas respectivas pistas.
    // - Adicione um contador para saber qual suspeito foi mais citado.
    // - Exiba ao final o “suspeito mais provável” baseado nas pistas coletadas.
    // - Para hashing simples, pode usar soma dos valores ASCII do nome ou primeira letra.
    // - Em caso de colisão, use lista encadeada para tratar.
    // - Modularize com funções como inicializarHash(), buscarSuspeito(), listarAssociacoes().

    // Criando as salas manualmente (estrutura fixa)
    Sala *hall            = criarSala("Hall de Entrada");
    Sala *salaEstar       = criarSala("Sala de Estar");
    Sala *cozinha         = criarSala("Cozinha");
    Sala *biblioteca      = criarSala("Biblioteca");
    Sala *jardim          = criarSala("Jardim");
    Sala *porao           = criarSala("Porão");
    Sala *quartoSecreto   = criarSala("Quarto Secreto");

    // Montando a árvore (mapa da mansão)
    //
    //              Hall
    //            /       \
    //      Sala Estar     Cozinha
    //       /      \        /
    // Biblioteca  Jardim  Porão
    //                         \
    //                     Quarto Secreto

    hall->esquerda = salaEstar;
    hall->direita  = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita  = jardim;

    cozinha->esquerda = porao;
    porao->direita = quartoSecreto;

    // Iniciar exploração
    printf("=== Detective Quest: Exploração da Mansão ===\n");
    explorarSalas(hall);

    return 0;
}

