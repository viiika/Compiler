
#ifndef LAB3
#define LAB3
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int tmp_var_no;
extern int var_no;
extern int label_no;



typedef struct Operand_* Operand;
struct Operand_{
    enum {OP_VARIABLE,OP_CONSTANT,OP_ADDRESS,OP_TMP_VARIABLE,OP_TMP_ADDRESS,OP_FUNCTION,OP_LABEL} kind;//more
    union 
    {
       int var_no;
       //int value;
       char value[40];
       Operand real_value;
       //more
    } u;

    Operand prev,next; //used for args
};

typedef struct InterCode_* InterCode;
struct InterCode_{
    enum {IC_LABEL,IC_FUNCTION,IC_ASSIGN,IC_ADD,IC_SUB,IC_MUL,IC_DIV,IC_GET_ADDR1,IC_GET_ADDR2,IC_GET_VAR,IC_TO_VAR,IC_GOTO,IC_IFGOTO,IC_RETURN,IC_DEC,IC_ARG,IC_CALL,IC_PARAM,IC_READ,IC_WRITE}kind; //more
    union 
    {
        struct {Operand op1;} sinop;
        struct {Operand right,left;} binop;
        struct {Operand result,op1,op2;} triop;
        struct {Operand result,op1,op2;char relop[40];} ifgotoop;
        struct {Operand op1;int size;} decop;
        //more
    } u;
    InterCode prev, next;
};

//merge to InterCode_{}
/*
typedef struct InterCodes_* InterCodes;
struct InterCodes_ {
    InterCode code;
    struct InterCodes_ *prev, *next;
};
*/

void fprintInterCode(char *filename);
void fprintOperand(FILE *fp,Operand op);
void insertInterCodes(InterCode ic);
void deleteInterCodes(InterCode ic);
void optimizeInterCode();

#endif 