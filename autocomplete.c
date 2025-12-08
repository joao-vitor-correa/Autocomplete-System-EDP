#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "autocomplete.h"

// ========================
// FUNÇÕES AUXILIARES AVL
// ========================
static int altura(NoAVL *n)
{
    return n ? n->altura : 0;
}

static int maior(int a, int b)
{
    return a > b ? a : b;
}

static NoAVL *novoNo(const char *palavra)
{
    NoAVL *no = (NoAVL *)malloc(sizeof(NoAVL));
    if (!no)
        return NULL;

    strncpy(no->palavra, palavra, TAM_MAX - 1);
    no->palavra[TAM_MAX - 1] = '\0';

    no->esq = NULL;
    no->dir = NULL;
    no->altura = 1;

    return no;
}

static NoAVL *rotacionarDireita(NoAVL *y)
{
    NoAVL *x = y->esq;
    NoAVL *T2 = x->dir;

    x->dir = y;
    y->esq = T2;

    y->altura = maior(altura(y->esq), altura(y->dir)) + 1;
    x->altura = maior(altura(x->esq), altura(x->dir)) + 1;

    return x;
}

static NoAVL *rotacionarEsquerda(NoAVL *x)
{
    NoAVL *y = x->dir;
    NoAVL *T2 = y->esq;

    y->esq = x;
    x->dir = T2;

    x->altura = maior(altura(x->esq), altura(x->dir)) + 1;
    y->altura = maior(altura(y->esq), altura(y->dir)) + 1;

    return y;
}

static int balanceamento(NoAVL *n)
{
    return n ? altura(n->esq) - altura(n->dir) : 0;
}

static int comparar(const char *a, const char *b)
{
#ifdef _WIN32
    return _stricmp(a, b);
#else
    return strcasecmp(a, b);
#endif
}

// ========================
// FUNÇÕES EXPORTADAS
// ========================
NoAVL *inserirAVL(NoAVL *no, const char *palavra)
{
    if (!no)
        return novoNo(palavra);

    if (comparar(palavra, no->palavra) < 0)
        no->esq = inserirAVL(no->esq, palavra);
    else if (comparar(palavra, no->palavra) > 0)
        no->dir = inserirAVL(no->dir, palavra);
    else
        return no;

    no->altura = 1 + maior(altura(no->esq), altura(no->dir));

    int bal = balanceamento(no);

    // Casos de rotação
    if (bal > 1 && comparar(palavra, no->esq->palavra) < 0)
        return rotacionarDireita(no);

    if (bal < -1 && comparar(palavra, no->dir->palavra) > 0)
        return rotacionarEsquerda(no);

    if (bal > 1 && comparar(palavra, no->esq->palavra) > 0)
    {
        no->esq = rotacionarEsquerda(no->esq);
        return rotacionarDireita(no);
    }

    if (bal < -1 && comparar(palavra, no->dir->palavra) < 0)
    {
        no->dir = rotacionarDireita(no->dir);
        return rotacionarEsquerda(no);
    }

    return no;
}

// ========================
// AUTOCOMPLETE
// ========================
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

static void coletarPrefixo(NoAVL *r, const char *prefixo,
                           char res[][TAM_MAX], int *qtd)
{
    if (!r || *qtd >= 3)
        return;

    coletarPrefixo(r->esq, prefixo, res, qtd);

    if (*qtd < 3 && comecaCom(r->palavra, prefixo))
    {
        strncpy(res[*qtd], r->palavra, TAM_MAX - 1);
        res[*qtd][TAM_MAX - 1] = '\0';
        (*qtd)++;
    }

    coletarPrefixo(r->dir, prefixo, res, qtd);
}

int autocompleteAVL(NoAVL *r, const char *prefixo, char res[][TAM_MAX])
{
    int qtd = 0;
    coletarPrefixo(r, prefixo, res, &qtd);
    return qtd;
}
