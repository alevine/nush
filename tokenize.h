// tokenize.h
// Author: Nat Tuck
// 3650F2017, Challenge01 Hints

#ifndef TOKENIZE_H
#define TOKENIZE_H

#include "arrlist.h"

// This returns a newly allocated svec.
// Caller is responsible for freeing it with free_svec(svec*)
arrlist* tokenize(char* text);

#endif
