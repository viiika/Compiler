%{
#include "syntax.tab.h"
#include <string.h>
extern int iferror;
extern char* printTreename[];
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
        char* char_data;//useless
        char nchar_data[40];
    };
    struct TreeNode* child;
    struct TreeNode* brother;
};
int loading(int kind,int name,char* data);

int yycolumn=1;
#define YY_USER_ACTION \
    yylloc.first_line=yylloc.last_line=yylineno; \
    yylloc.first_column=yycolumn; \
    yylloc.last_column=yycolumn+yyleng-1; \
    yycolumn+=yyleng;

%}

%option yylineno

newline \n
delim [ \t\n\r]
ws {delim}+
digit [0-9]
digits {digit}+
letter [_a-zA-Z]
tenINT [1-9]{digit}*|0
INT {tenINT}
traditionalFLOAT {tenINT}\.{digit}+
sicFLOATa {digit}+\.{digit}*[eE][-+]?{digit}+
sicFLOATb \.{digit}+[eE][-+]?{digit}+
sicFLOAT {sicFLOATa}|{sicFLOATb}
FLOAT {traditionalFLOAT}|{sicFLOAT}
ID {letter}({letter}|{digit})*
SEMI ;
COMMA ,
ASSIGNOP =
RELOP >|<|>=|<=|==|!=
PLUS \+
MINUS -
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT \!
TYPE int|float
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while


%%

{newline} {yycolumn=1;}
{ws} {/* no action and no return*/}

{TYPE} {loading(k_char,e_TYPE,yytext); return TYPE;}
{STRUCT} {loading(k_char,e_STRUCT,yytext); return STRUCT;}
{IF} {loading(k_char,e_IF,yytext); return IF;}
{ELSE} {loading(k_char,e_ELSE,yytext); return ELSE;}
{WHILE} {loading(k_char,e_WHILE,yytext); return WHILE;}
{RETURN} {loading(k_char,e_RETURN,yytext); return RETURN;}

{SEMI} {loading(k_char,e_SEMI,yytext); return SEMI;}
{COMMA} {loading(k_char,e_COMMA,yytext); return COMMA;}
{ASSIGNOP} {loading(k_char,e_ASSIGNOP,yytext); return ASSIGNOP;}
{LP} {loading(k_char,e_LP,yytext); return LP;}
{RP} {loading(k_char,e_RP,yytext); return RP;}
{LB} {loading(k_char,e_LB,yytext); return LB;}
{RB} {loading(k_char,e_RB,yytext); return RB;}
{LC} {loading(k_char,e_LC,yytext); return LC;}
{RC} {loading(k_char,e_RC,yytext); return RC;}
{AND} {loading(k_char,e_AND,yytext); return AND;}
{OR} {loading(k_char,e_OR,yytext); return OR;}
{PLUS} {loading(k_char,e_PLUS,yytext); return PLUS;}
{MINUS} {loading(k_char,e_MINUS,yytext); return MINUS;}
{STAR} {loading(k_char,e_STAR,yytext); return STAR;}
{DIV} {loading(k_char,e_DIV,yytext); return DIV;}
{NOT} {loading(k_char,e_NOT,yytext); return NOT;}
{DOT} {loading(k_char,e_DOT,yytext); return DOT;}



{RELOP} {loading(k_char,e_RELOP,yytext); return RELOP;}  
{INT} {loading(k_int,e_INT,yytext);return INT;}
{FLOAT} {loading(k_float,e_FLOAT,yytext);return FLOAT;}
{ID} {loading(k_char,e_ID,yytext); return ID;}


. {iferror=1; printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext);}
%%
int loading(int kind,int name,char* data){
    yylval.treeNode=(struct TreeNode*)malloc(sizeof(struct TreeNode));
    yylval.treeNode->lineno=yylineno;
    yylval.treeNode->int_kind=kind;
    yylval.treeNode->int_name=name;
    switch(kind){
        case k_char:
            strcpy(yylval.treeNode->nchar_data,data); //wrong way
            
            //yylval.treeNode->char_data=data;
            //printf("%s ",yylval.treeNode->char_data);
            break;
        case k_int:yylval.treeNode->int_data=atoi(data);
            break;
        case k_float:yylval.treeNode->float_data=atof(data);
            break;
    }
    yylval.treeNode->child=NULL;
    yylval.treeNode->brother=NULL;
    //special situation
    //check
    //printf("%s \n",printTreename[yylval.treeNode->int_name]);

    return 0;
}