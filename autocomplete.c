#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM_MAX 128

typedef struct NoAVL {
    char palavra[TAM_MAX];
    int altura;
    struct NoAVL *esq;
    struct NoAVL *dir;
} NoAVL;

int altura(NoAVL *n) { return n ? n->altura : 0; }
int maior(int a, int b) { return a > b ? a : b; }

NoAVL* novoNo(const char *palavra) {
    NoAVL *no = malloc(sizeof(NoAVL));
    strcpy(no->palavra, palavra);
    no->esq = no->dir = NULL;
    no->altura = 1;
    return no;
}

NoAVL* rotacionarDireita(NoAVL *y) {
    NoAVL *x = y->esq;
    NoAVL *T2 = x->dir;
    x->dir = y;
    y->esq = T2;
    y->altura = maior(altura(y->esq), altura(y->dir)) + 1;
    x->altura = maior(altura(x->esq), altura(x->dir)) + 1;
    return x;
}

NoAVL* rotacionarEsquerda(NoAVL *x) {
    NoAVL *y = x->dir;
    NoAVL *T2 = y->esq;
    y->esq = x;
    x->dir = T2;
    x->altura = maior(altura(x->esq), altura(x->dir)) + 1;
    y->altura = maior(altura(y->esq), altura(y->dir)) + 1;
    return y;
}

int balanceamento(NoAVL *n) { return n ? altura(n->esq) - altura(n->dir) : 0; }

int comparar(const char *a, const char *b) {
#ifdef _WIN32
    return _stricmp(a, b);
#else
    return strcasecmp(a, b);
#endif
}

NoAVL* inserirAVL(NoAVL *no, const char *palavra) {
    if (!no) return novoNo(palavra);

    if (comparar(palavra, no->palavra) < 0)
        no->esq = inserirAVL(no->esq, palavra);
    else if (comparar(palavra, no->palavra) > 0)
        no->dir = inserirAVL(no->dir, palavra);
    else
        return no;

    no->altura = 1 + maior(altura(no->esq), altura(no->dir));

    int bal = balanceamento(no);

    if (bal > 1 && comparar(palavra, no->esq->palavra) < 0)
        return rotacionarDireita(no);
    if (bal < -1 && comparar(palavra, no->dir->palavra) > 0)
        return rotacionarEsquerda(no);
    if (bal > 1 && comparar(palavra, no->esq->palavra) > 0) {
        no->esq = rotacionarEsquerda(no->esq);
        return rotacionarDireita(no);
    }
    if (bal < -1 && comparar(palavra, no->dir->palavra) < 0) {
        no->dir = rotacionarDireita(no->dir);
        return rotacionarEsquerda(no);
    }

    return no;
}

int comecaCom(const char *palavra, const char *prefixo) {
    while (*prefixo) {
        if (tolower(*prefixo) != tolower(*palavra))
            return 0;
        prefixo++;
        palavra++;
    }
    return 1;
}

void coletarPrefixo(NoAVL *r, const char *prefixo, char res[][TAM_MAX], int *qtd) {
    if (!r || *qtd >= 3) return;
    coletarPrefixo(r->esq, prefixo, res, qtd);
    if (*qtd < 3 && comecaCom(r->palavra, prefixo))
        strcpy(res[(*qtd)++], r->palavra);
    coletarPrefixo(r->dir, prefixo, res, qtd);
}

int autocompleteAVL(NoAVL *r, const char *prefixo, char res[][TAM_MAX]) {
    int qtd = 0;
    coletarPrefixo(r, prefixo, res, &qtd);
    return qtd;
}

int main() {
    const char *lista[] = {
        "abacate","abacaxi","abakashi","abiu","abrico","acai","acerola","amora",
        "ameixa","araca","araticum","banana","bergamota","bacuri","buriti","caju",
        "caqui","cereja","cupuaçu","damasco","figo","framboesa","goiaba","graviola",
        "groselha","jabuticaba","jaca","jambo","jenipapo","kiwi","laranja","limao",
        "lima","maca","mamao","manga","maracuja","melancia","melao","morango",
        "nectarina","pera","pessego","pitanga","pitaya","seriguela","tamarindo",
        "tangerina","umbu","uva"
    };

    int total = sizeof(lista)/sizeof(lista[0]);
    NoAVL *raiz = NULL;

    for (int i = 0; i < total; i++)
        raiz = inserirAVL(raiz, lista[i]);

    char prefixo[TAM_MAX];
    char resultados[3][TAM_MAX];

    printf("Digite prefixo: ");
    scanf("%s", prefixo);

    int qtd = autocompleteAVL(raiz, prefixo, resultados);

    printf("\nSugestoes:\n");
    for (int i = 0; i < qtd; i++)
        printf("- %s\n", resultados[i]);

    if (qtd == 0)
        printf("Nenhuma palavra encontrada.\n");

    return 0;
}
