#ifndef LAB1
#define LAB1

enum treekind
{
    k_char,
    k_int,
    k_float,
    k_nchar
};
enum treename
{
    e_SEMI,
    e_COMMA,
    e_TYPE,
    e_LC,
    e_RC,
    e_LB,
    e_RB,
    e_LP,
    e_RP,
    e_STRUCT,
    e_ID,
    e_INT,
    e_FLOAT,
    e_ASSIGNOP,
    e_AND,
    e_OR,
    e_RELOP,
    e_PLUS,
    e_MINUS,
    e_STAR,
    e_DIV,
    e_NOT,
    e_DOT,
    e_IF,
    e_ELSE,
    e_WHILE,
    e_RETURN,

    e_Program,
    e_ExtDefList,
    e_ExtDef,
    e_ExtDecList,
    e_Specifier,
    e_FunDec,
    e_CompSt,
    e_VarDec,
    e_VarList,
    e_StructSpecifier,
    e_OptTag,
    e_DefList,
    e_Args,
    e_ParamDec,
    e_Exp,
    e_Stmt,
    e_StmtList,
    e_Tag,
    e_Def,
    e_DecList,
    e_Dec,

    e_NULL
};
struct TreeNode
{
    int lineno;
    union
    {
        int int_kind;
        char *char_kind;
    };
    union
    {
        int int_name;
        char *char_name;
    };
    union
    {
        int int_data;     //useless
        float float_data; //useless
        char nchar_data[40];
    };
    struct TreeNode *child;
    struct TreeNode *brother;
};

#endif