#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define INSTRUCTION_TAILLE 11 /* Définit la taille d'une instruction */
#define CONVERTION_BASE 10 /* Définit la basse de convertion */

int err = 0; /* Variable global */

typedef struct pile
{
    int value;
    struct pile *under;
}pile_t;

pile_t* level_create(int value, pile_t* under) /* Ajoute un niveau au dessus de la pile donné en paramètre */
{
    pile_t* level = (pile_t*)malloc(sizeof(pile_t));
    if ( level == NULL ) /* Problème de mémoire */
    {
        fprintf(stderr, "Mémoire insuffisante à la creation d'un nouveau level de la pile\n");
        exit(1);
    }
    level->value = value;
    level->under = under;
    return level;
}

void pile_reset(pile_t* level) /* Libère l'espace mémoire de chaque level de la pile */
{
    if ( level != NULL ) /* Pile vidé de ses niveau, plus de niveau */
    {
        pile_reset(level->under);
        free(level);
    }
}

void pile_show(pile_t* level, int i) /* Affiche les valeurs des différents niveau de la pile, en partant du BAS vers le HAUT */
{
    if ( level != NULL ) /* Plus de niveau inférieur, on a fini de parcourir la pile */
    {
        pile_show(level->under, i+1);
        printf("%d", level->value); /* du bas vers le haut, à passer au dessus de l'instruction pile_show(level->under); pour inverser */
        if ( i != 0 )
            printf(" ");
    }
}

pile_t* level_add(pile_t* pile, pile_t* level) /* Ajoute un nouveau level à la pile et l'ajoute AU DESSUS DE LA PILE */
{
    if ( pile == NULL ) /* Aucune pile, deviens la pile */
    {
        level->under = NULL;
        return level;
    }
    level->under = pile; /* AJOUT AU DESSUS DE LA PILE */
    return /*pile*/level;
}

int est_un_int (char instruction[INSTRUCTION_TAILLE])
{
    int i;
    for ( i = 0; i < INSTRUCTION_TAILLE && instruction[i] != '\0'; i++ )
        if ( instruction[0] != '-' && !isdigit(instruction[i]) ) // n'est pas un char et n'est pas un -
            return 0;
    return 1;
}

int cast_en_int (char instruction[INSTRUCTION_TAILLE])
{
    char *ptr;
    return strtol(instruction, &ptr, CONVERTION_BASE);
}

int pile_rol ( pile_t * back, pile_t* pile, int i ) /* selectionne l'element i de la pile et le remonte au haut de la pile */
{
    if ( back == NULL || pile == NULL ) /* la pile est vide */
    {
        err = 1;
        return 0;
    }
    if ( i == 1 )
    {
        pile_t* under = pile->under;
        int value = pile->value;
        free(pile); /* supprime le level actuel */
        back->under = under;
        return value;
    }
    else
         return pile_rol ( pile, pile->under, i-1 );
}

pile_t* pile_swp ( pile_t* pile ) /* change les positions du premier et second element de la pile */
{
    if ( pile == NULL ) /* la pile est vide */
        return NULL;
    if ( pile->under != NULL ) /* il existe un deuxième élément dans la pile */
    {
        int temp = pile->value;
        pile->value = pile->under->value;
        pile->under->value = temp;
    }
    return pile;
}

pile_t* pile_dup ( pile_t* level ) /* duplique le premier element de la pile */
{
    if ( level == NULL ) /* la pile est vide */
        return NULL;
    return level_add(level, level_create(level->value, level));
}

pile_t* pile_pop ( pile_t* level ) /* retire le premier element de la pile */
{
    pile_t* under = NULL;
    if ( level != NULL ) /* la pile n'est pas vide */
    {
        under = level->under;
        free(level);
    }
    return under;
}

pile_t* pile_mod ( pile_t* level ) /* effectue le modulo */
{
    if ( level == NULL ) /* la pile est vide */
        return NULL;
    if ( level->under == NULL ) /* la pile comporte qu'un element */
    {
        err = 1;
        free(level);
        return level;
    }
    int gauche = level->under->value, droit = level->value;
    pile_t* under = level->under->under;
    free(level->under);
    free(level);
    return level_add(under, level_create(gauche%droit, under));
}

pile_t* pile_div ( pile_t* level ) /* effectue la division */
{
    if ( level == NULL ) /* la pile est vide */
        return NULL;
    if ( level->under == NULL ) /* la pile comporte qu'un element */
    {
        err = 1;
        level = pile_pop(level);
        level = pile_pop(level);
        return level;
    }
    int gauche = level->under->value, droit = level->value;
    if ( droit == 0 )
    {
        err = 1;
        level = pile_pop(level);
        level = pile_pop(level);
        return level;
    }
    pile_t* under = level->under->under;
    free(level->under);
    free(level);
    return level_add(under, level_create(gauche/droit, under));
}

pile_t* pile_mul ( pile_t* level ) /* effectue la multiplication */
{
    if ( level == NULL ) /* la pile est vide */
        return NULL;
    if ( level->under == NULL ) /* la pile comporte qu'un element */
    {
        err = 1;
        level = pile_pop(level);
        return level;
    }
    int gauche = level->under->value, droit = level->value;
    pile_t* under = level->under->under;
    free(level->under);
    free(level);
    return level_add(under, level_create(gauche*droit, under));
}

pile_t* pile_sub ( pile_t* level ) /* effectue la soustraction */
{
    if ( level == NULL ) /* la pile est vide */
        return NULL;
    if ( level->under == NULL ) /* la pile comporte qu'un element */
    {
        err = 1;
        level = pile_pop(level);
        return level;
    }
    int gauche = level->under->value, droit = level->value;
    pile_t* under = level->under->under;
    free(level->under);
    free(level);
    return level_add(under, level_create(gauche-droit, under));
}

pile_t* pile_add ( pile_t* level ) /* effectue une addition */
{
    if ( level == NULL ) /* la pile est vide */
        return NULL;
    if ( level->under == NULL ) /* la pile comporte qu'un element */
    {
        err = 1;
        level = pile_pop(level);
        return level;
    }
    int gauche = level->under->value, droit = level->value;
    pile_t* under = level->under->under;
    free(level->under);
    free(level);
    return level_add(under, level_create(gauche+droit, under));
}

pile_t* operation ( pile_t* pile, char instruction[INSTRUCTION_TAILLE] )
{
    if ( !strcmp("ROL", instruction) )
    {
        int n = pile->value;
        pile = pile_pop(pile);
        n = pile_rol(pile, pile->under, n-1);
        pile = level_add(pile, level_create(n, pile));
    }
    else if ( !strcmp("SWP", instruction) )
        pile = pile_swp(pile);
    else if ( !strcmp("DUP", instruction) )
        pile = pile_dup(pile);
    else if ( !strcmp("POP", instruction) )
        pile = pile_pop(pile);
    else if ( !strcmp("MOD", instruction) )
        pile = pile_mod(pile);
    else if ( !strcmp("DIV", instruction) )
        pile = pile_div(pile);
    else if ( !strcmp("MUL", instruction) )
        pile = pile_mul(pile);
    else if ( !strcmp("SUB", instruction) )
        pile = pile_sub(pile);
    else if ( !strcmp("ADD", instruction) )
        pile = pile_add(pile);
    else
        err = 1;
    return pile;
}

int main()
{
    int N, i;
    pile_t* pile = NULL;
    scanf("%d", &N);
    for ( i = 0; i < N && err != 1; i++ )
    {
        char instruction[INSTRUCTION_TAILLE];
        scanf("%s", instruction);
        if ( est_un_int(instruction) )
            pile = level_add(pile, level_create(cast_en_int(instruction), pile));
        else
            pile = operation(pile, instruction);
    }
    pile_show(pile, 0);
    if ( err )
    {
        if ( pile != NULL )
            printf(" ");
        printf("ERROR");
    }
    printf("\n");
    pile_reset(pile);
    return 0;
}