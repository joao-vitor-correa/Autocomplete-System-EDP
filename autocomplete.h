#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>

#define TAM_MAX 128

    typedef struct NoAVL
    {
        char palavra[TAM_MAX];
        int altura;
        struct NoAVL *esq;
        struct NoAVL *dir;
    } NoAVL;

    // Insere uma palavra na árvore AVL
    NoAVL *inserirAVL(NoAVL *no, const char *palavra);

    // Retorna até 3 sugestões baseadas no prefixo
    int autocompleteAVL(NoAVL *r, const char *prefixo, char res[][TAM_MAX]);

#ifdef __cplusplus
}
#endif

#endif // AUTOCOMPLETE_H
