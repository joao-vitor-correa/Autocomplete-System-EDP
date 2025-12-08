#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "autocomplete.h"

// ========================
// FUNÇÕES AUXILIARES AVL
// ========================

// Retorna a altura de um nó (0 se for NULL)
static int altura(NoAVL *n)
{
    return n ? n->altura : 0;
}

// Retorna o maior entre dois inteiros
static int maior(int a, int b)
{
    return a > b ? a : b;
}

// Cria um novo nó AVL com a palavra passada
static NoAVL *novoNo(const char *palavra)
{
    NoAVL *no = (NoAVL *)malloc(sizeof(NoAVL));
    if (!no)
        return NULL;

    // Copia a palavra limitada ao TAM_MAX
    strncpy(no->palavra, palavra, TAM_MAX - 1);
    no->palavra[TAM_MAX - 1] = '\0';

    no->esq = NULL;
    no->dir = NULL;
    no->altura = 1; // Nó novo sempre tem altura 1

    return no;
}

// Rotação simples à direita
static NoAVL *rotacionarDireita(NoAVL *y)
{
    NoAVL *x = y->esq;  // Filho esquerdo de y
    NoAVL *T2 = x->dir; // Subárvore direita de x

    // Realiza a rotação
    x->dir = y;
    y->esq = T2;

    // Atualiza alturas
    y->altura = maior(altura(y->esq), altura(y->dir)) + 1;
    x->altura = maior(altura(x->esq), altura(x->dir)) + 1;

    return x; // Novo nó raiz desta subárvore
}

// Rotação simples à esquerda
static NoAVL *rotacionarEsquerda(NoAVL *x)
{
    NoAVL *y = x->dir;  // Filho direito de x
    NoAVL *T2 = y->esq; // Subárvore esquerda de y

    // Realiza a rotação
    y->esq = x;
    x->dir = T2;

    // Atualiza alturas
    x->altura = maior(altura(x->esq), altura(x->dir)) + 1;
    y->altura = maior(altura(y->esq), altura(y->dir)) + 1;

    return y; // Novo nó raiz desta subárvore
}

// Calcula o fator de balanceamento (altura esquerda - altura direita)
static int balanceamento(NoAVL *n)
{
    return n ? altura(n->esq) - altura(n->dir) : 0;
}

// Comparação case-insensitive entre duas strings
static int comparar(const char *a, const char *b)
{
#ifdef _WIN32
    return _stricmp(a, b);
#else
    return strcasecmp(a, b);
#endif
}

// ========================
// FUNÇÃO DE INSERÇÃO AVL
// ========================
NoAVL *inserirAVL(NoAVL *no, const char *palavra)
{
    // Insere como em uma BST normal
    if (!no)
        return novoNo(palavra);

    if (comparar(palavra, no->palavra) < 0)
        no->esq = inserirAVL(no->esq, palavra);
    else if (comparar(palavra, no->palavra) > 0)
        no->dir = inserirAVL(no->dir, palavra);
    else
        return no; // Palavra duplicada → não insere

    // Atualiza altura deste nó
    no->altura = 1 + maior(altura(no->esq), altura(no->dir));

    // Verifica balanceamento
    int bal = balanceamento(no);

    // ------- Casos de rotação ---------

    // Caso ESQUERDA-ESQUERDA
    if (bal > 1 && comparar(palavra, no->esq->palavra) < 0)
        return rotacionarDireita(no);

    // Caso DIREITA-DIREITA
    if (bal < -1 && comparar(palavra, no->dir->palavra) > 0)
        return rotacionarEsquerda(no);

    // Caso ESQUERDA-DIREITA
    if (bal > 1 && comparar(palavra, no->esq->palavra) > 0)
    {
        no->esq = rotacionarEsquerda(no->esq);
        return rotacionarDireita(no);
    }

    // Caso DIREITA-ESQUERDA
    if (bal < -1 && comparar(palavra, no->dir->palavra) < 0)
    {
        no->dir = rotacionarDireita(no->dir);
        return rotacionarEsquerda(no);
    }

    return no; // Retorna o nó possivelmente reequilibrado
}

// ========================
// AUTOCOMPLETE
// ========================

// Verifica se `palavra` começa com `prefixo` (case-insensitive)
static int comecaCom(const char *palavra, const char *prefixo)
{
    while (*prefixo)
    {
        if (tolower((unsigned char)*prefixo) !=
            tolower((unsigned char)*palavra))
            return 0;

        prefixo++;
        palavra++;
    }
    return 1;
}

// Percorre a árvore coletando até 3 palavras que começam com o prefixo
static void coletarPrefixo(NoAVL *r, const char *prefixo,
                           char res[][TAM_MAX], int *qtd)
{
    if (!r || *qtd >= 3)
        return;

    // Percorre em ordem (ordenado alfabeticamente)
    coletarPrefixo(r->esq, prefixo, res, qtd);

    // Se ainda tem espaço e a palavra combina com o prefixo
    if (*qtd < 3 && comecaCom(r->palavra, prefixo))
    {
        strncpy(res[*qtd], r->palavra, TAM_MAX - 1);
        res[*qtd][TAM_MAX - 1] = '\0';
        (*qtd)++;
    }

    coletarPrefixo(r->dir, prefixo, res, qtd);
}

// Interface principal do autocomplete
int autocompleteAVL(NoAVL *r, const char *prefixo, char res[][TAM_MAX])
{
    int qtd = 0;
    coletarPrefixo(r, prefixo, res, &qtd);
    return qtd; // Retorna quantas opções foram encontradas
}
