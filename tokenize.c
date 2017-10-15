// tokenize.c
// Author: Nat Tuck
// 3650F2017, Challenge01 Hints
// 
// ATTRIBUTION: Not sure if I need this, my own tokenizer "worked" but failed
// valgrind tests. Because of this I am switching to the tokenizer provided in the
// starter code, in this file. Thanks.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tokenize.h"

typedef int (*char_pred)(char);

int
is_op_char(char cc)
{
    return cc == '<' || cc == '>'
        || cc == '|' || cc == '&'
        || cc == ';';
}

int
is_nop_char(char cc)
{
    return cc != 0 && !is_op_char(cc) && !isspace(cc);
}

char*
get_tok(char* text, char_pred cpred)
{
    char* tt = malloc(256);
    int ii = 0;
    for (; cpred(text[ii]); ++ii) {
        tt[ii] = text[ii];
    }
    tt[ii] = 0;
    return tt;
}

arrlist*
tokenize(char* text)
{
    arrlist* xs = make_arrlist();

    while (text[0]) {
        if (isspace(text[0])) {
            text++;
            continue;
        }

        char* tt;

        if (is_op_char(text[0])) {
            tt = get_tok(text, is_op_char);
        }
        else {
            tt = get_tok(text, is_nop_char);
        }

        arrlist_add(xs, tt);
        text += strlen(tt);
        free(tt);
    }

    return xs;
}

