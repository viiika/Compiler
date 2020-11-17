%{
#ifndef LEXICAL
#define LEXICAL
#include "syntax.tab.h"
#include <string.h>
#include "lab1.h"
extern int iferror;
extern char* printTreename[];


int loading(int kind,int name,char* data);

int yycolumn=1;
#define YY_USER_ACTION \
    yylloc.first_line=yylloc.last_line=yylineno; \
    yylloc.first_column=yycolumn; \
    yylloc.last_column=yycolumn+yyleng-1; \
    yycolumn+=yyleng;

#endif

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


. {//iferror=1; printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext);
    iferror=1; printf("Error type A at Line %d.\n",yylineno);}//new version
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
        case k_int://yylval.treeNode->int_data=atoi(data);
                    //change in lab2
                    strcpy(yylval.treeNode->nchar_data,data);
            break;
        case k_float://yylval.treeNode->float_data=(float)atof(data); //atof treeNode double ,if to float there would be many error
                //printf("\n\n\n %f ,%f",atof(data),yylval.treeNode->float_data);
                //change in lab2
                strcpy(yylval.treeNode->nchar_data,data);
            break;
    }
    yylval.treeNode->child=NULL;
    yylval.treeNode->brother=NULL;
   
    return 0;
}