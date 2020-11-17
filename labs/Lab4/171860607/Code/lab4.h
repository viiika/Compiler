#ifndef LAB4
#define LAB4

#include "lab2.h"
#include "lab3.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct REG_ *REG;
typedef struct VAR_ *VAR;
struct REG_
{
    int free;
    char *name;
};

struct VAR_
{
    int reg;
    char *name;
    Operand op; //only read
    VAR next;
    int offset;
};

void fprintFinalCode(FILE *fp);

int staynightpack(FILE *fp, int start, Operand op);

#endif
