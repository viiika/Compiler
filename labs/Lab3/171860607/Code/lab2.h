#ifndef LAB2
#define LAB2

#include "lab3.h"
typedef struct Type_ * Type;
typedef struct FieldList_ * FieldList;
#define HashTableSize 0x3fff

//extern struct TreeNode;
struct TreeNode{
    int lineno;
    union{
        int int_kind;
        char* char_kind;
    };
    union{
        int int_name;
        char* char_name;
    };
    union{
        int int_data;
        float float_data;
        char nchar_data[40];
    };
    struct TreeNode* child;
    struct TreeNode* brother;
};
enum treekind{
    k_char,k_int,k_float,k_nchar
};
enum treename{
    e_SEMI,e_COMMA,e_TYPE,e_LC,e_RC,e_LB,e_RB,e_LP,e_RP,e_STRUCT,e_ID,e_INT,e_FLOAT,e_ASSIGNOP,
    e_AND,e_OR,e_RELOP,e_PLUS,e_MINUS,e_STAR,e_DIV,e_NOT,e_DOT,e_IF,e_ELSE,e_WHILE,e_RETURN,

    e_Program,e_ExtDefList,e_ExtDef,e_ExtDecList,e_Specifier, 
    e_FunDec,e_CompSt,e_VarDec,e_VarList,e_StructSpecifier,e_OptTag,e_DefList,
    e_Args,e_ParamDec,e_Exp,e_Stmt,e_StmtList,e_Tag,e_Def,e_DecList,e_Dec,

    e_NULL
};


struct Type_
{
   //enum {BASIC,ARRAY,STRUCTURE,VARIABLE,FUNCTION,UNDEFINED_FUNCTION,BASIC_INT,BASIC_FLOAT} kind;
   enum {BASIC,ARRAY,STRUCTURE,FUNCTION,READWRITE} kind;
        // use for BASIC,ARRAY,STRUCTURE,FUNCTION
   union 
   {
       //int basic; //useless //BASIC_INT,BASIC_FLOAT
       enum {UBASIC_INT,UBASIC_FLOAT} basic;
       struct {Type elem;int size;}array;
       FieldList structure; //structure //struct para will be there //store all the struct
       struct {Type retType;int paraNum;FieldList para;}function;//function's parameters
       
       
   } u;
   char *structureName; //only useful for structure kind. //now useless
   int curNonNameStruct;
   enum {LEFT_VALUE,RIGHT_VALUE,BOTH_VALUE,OTHERS_VALUE} value;//left include VARIABLE,FUNCTION
};

struct FieldList_ //every hashtong is a series of FieldList, and each f means a symbol which saved in type.
{
    char* name;
    Type type;
    FieldList tail; //store about chart not data
    //int depth;
};

FieldList HashTable[HashTableSize];

unsigned int hash_pjw(char* name);
FieldList insertHashTable(FieldList f,int kind);
FieldList searchHashTable(char *name,Type typ,int specialCode);



void createHashTable();
void symbolTable(struct TreeNode *root);
void Program(struct TreeNode * root);
void ExtDefList(struct TreeNode * extdeflist);
void ExtDecList(struct TreeNode* extdeclist,Type type);
void ExtDef(struct TreeNode * extdef);
Type Specifier(struct TreeNode * specifier);
Type StructSpecifier(struct TreeNode * structspecifier);
FieldList DefList(struct TreeNode *deflist,int structure);
FieldList Def(struct TreeNode *def,int structure);
FieldList DecList(struct TreeNode *declist,Type type,int structure);
FieldList Dec(struct TreeNode* dec,Type type,int structure);
FieldList VarDec(struct TreeNode* vardec,Type type,int structure);
FieldList FunDec(struct TreeNode* fundec,Type type);
FieldList VarList(struct TreeNode* fundec);
FieldList ParamDec(struct TreeNode* paramdec);
void CompSt(struct TreeNode* compst,Type type);
void StmtList(struct TreeNode* stmtlist,Type type);
void Stmt(struct TreeNode* stmt,Type type);
Type Exp(struct TreeNode* exp,Operand place);
FieldList Args(struct TreeNode* args,Operand place);

void printError(int errorType,int line,char *name);
int typeCheck(Type t1, Type t2);


Type translate_cond(struct TreeNode* condroot,Operand trueop,Operand falseop);
int arrayLen(Type t,int choose);

#endif