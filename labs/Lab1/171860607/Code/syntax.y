%{
#include "lex.yy.c"
#include <string.h>
#include <stdio.h>
struct TreeNode* root;
void fill(struct TreeNode *t);
void printTree(struct TreeNode * tree,int depth);
extern void yyerror(char* msg);
int iferror=0;
int syntaxErrorType=0;
extern enum treekind treekind;
extern enum treename treename;
char* printTreename[]={
    "SEMI","COMMA","TYPE","LC","RC","LB","RB","LP","RP","STRUCT","ID","INT","FLOAT","ASSIGNOP",
    "AND","OR","RELOP","PLUS","MINUS","STAR","DIV","NOT","DOT","IF","ELSE","WHILE","RETURN",
    "Program","ExtDefList","ExtDef","ExtDecList","Specifier", 
    "FunDec","CompSt","VarDec","VarList","StructSpecifier","OptTag","DefList",
    "Args","ParamDec","Exp","Stmt","StmtList","Tag","Def","DecList","Dec",
    
    "NULL"
};
struct TreeNode* loadingsyntax(enum treename tn,enum treekind tk);
/*
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
*/

%}
%union{
    struct TreeNode* treeNode;
}
%locations

%token <treeNode> SEMI
%token <treeNode> COMMA
%token <treeNode> TYPE
%token <treeNode> LC RC LB RB LP RP
%token <treeNode> STRUCT
%token <treeNode> ID
%token <treeNode> INT
%token <treeNode> FLOAT
%token <treeNode> ASSIGNOP
%token <treeNode> AND OR RELOP PLUS MINUS STAR DIV NOT DOT
%token <treeNode> IF ELSE WHILE RETURN

%type <treeNode> Program ExtDefList ExtDef ExtDecList Specifier 
%type <treeNode> FunDec CompSt VarDec VarList StructSpecifier OptTag DefList
%type <treeNode> Args ParamDec Exp Stmt StmtList Tag Def DecList Dec






%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS 
%left STAR DIV
%right NOT
%left LB RB LP RP DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
/* High-levell Definitions */
Program : ExtDefList {$$=loadingsyntax(e_Program,k_nchar);$$->child=$1;fill($$);root=$$;}
    ;
ExtDefList : ExtDef ExtDefList {$$=loadingsyntax(e_ExtDefList,k_nchar);$$->child=$1;$1->brother=$2;fill($$);}
    | /* empty */ {$$=loadingsyntax(e_NULL,k_nchar);$$->child=NULL;fill($$);}
    ;
ExtDef : Specifier ExtDecList SEMI {$$=loadingsyntax(e_ExtDef,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Specifier SEMI {$$=loadingsyntax(e_ExtDef,k_nchar);$$->child=$1;$1->brother=$2;fill($$);}
    | error SEMI {}
    | Specifier FunDec CompSt {$$=loadingsyntax(e_ExtDef,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    
    //| error CompSt {}
    
    ;
ExtDecList : VarDec {$$=loadingsyntax(e_ExtDecList,k_nchar);$$->child=$1;fill($$);}
    | VarDec COMMA ExtDecList {$$=loadingsyntax(e_ExtDecList,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    ;

/*Specifiers*/
Specifier : TYPE {$$=loadingsyntax(e_Specifier,k_nchar);$$->child=$1;fill($$);}
    | StructSpecifier {$$=loadingsyntax(e_Specifier,k_nchar);$$->child=$1;fill($$);}
    //| error {}
    ;
StructSpecifier : STRUCT OptTag LC DefList RC {$$=loadingsyntax(e_StructSpecifier,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;$3->brother=$4;$4->brother=$5;fill($$);}
    | STRUCT Tag {$$=loadingsyntax(e_StructSpecifier,k_nchar);$$->child=$1;$1->brother=$2;fill($$);}    

    ;
OptTag : ID {$$=loadingsyntax(e_OptTag,k_nchar);$$->child=$1;fill($$);}
    | /*empty*/ {$$=loadingsyntax(e_NULL,k_nchar);$$->child=NULL;fill($$);}
    ;
Tag :ID {$$=loadingsyntax(e_Tag,k_nchar);$$->child=$1;fill($$);}
    ;

/*Declarators*/
VarDec : ID {$$=loadingsyntax(e_VarDec,k_nchar);$$->child=$1;fill($$);}
    | VarDec LB INT RB {$$=loadingsyntax(e_VarDec,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;$3->brother=$4;fill($$);}
    | VarDec LB error RB {}
    ;
FunDec : ID LP VarList RP {$$=loadingsyntax(e_FunDec,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;$3->brother=$4;fill($$);}
    | ID LP RP {$$=loadingsyntax(e_FunDec,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    //| error LP VarList RP {}
    | error RP {syntaxErrorType=3;}
    
    
    ;
VarList : ParamDec COMMA VarList {$$=loadingsyntax(e_VarList,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | ParamDec {$$=loadingsyntax(e_VarList,k_nchar);$$->child=$1;fill($$);}
    ;
ParamDec : Specifier VarDec {$$=loadingsyntax(e_ParamDec,k_nchar);$$->child=$1;$1->brother=$2;fill($$);}
    ;

//below may be okay
/*Statements*/
CompSt : LC DefList StmtList RC {$$=loadingsyntax(e_CompSt,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;$3->brother=$4;fill($$);}

    //|  error StmtList RC {}  //new add for debug test
                            //seems need to delete StmtList
    | error RC {} //change to this 
        ;
StmtList : Stmt StmtList {$$=loadingsyntax(e_StmtList,k_nchar);$$->child=$1;$1->brother=$2;fill($$);}
    | /*empty*/ {$$=loadingsyntax(e_NULL,k_nchar);$$->child=NULL;fill($$);}
    
    ;
Stmt : Exp SEMI {$$=loadingsyntax(e_Stmt,k_nchar);$$->child=$1;$1->brother=$2;fill($$);}
    | CompSt {$$=loadingsyntax(e_Stmt,k_nchar);$$->child=$1;fill($$);}
    | RETURN Exp SEMI {$$=loadingsyntax(e_Stmt,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE Stmt {$$=loadingsyntax(e_Stmt,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;$3->brother=$4;$4->brother=$5;fill($$);}
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE Stmt {}
    | IF LP Exp RP Stmt ELSE Stmt {$$=loadingsyntax(e_Stmt,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;$3->brother=$4;$4->brother=$5;$5->brother=$6;$6->brother=$7;fill($$);}
    | IF LP error RP Stmt ELSE Stmt {}
    | WHILE LP Exp RP Stmt {$$=loadingsyntax(e_Stmt,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;$3->brother=$4;$4->brother=$5;fill($$);}
    | WHILE LP error RP Stmt {}
    | error SEMI {}
    
    ;
//below would be ok *2
/*Local Definitions*/
DefList : Def DefList {$$=loadingsyntax(e_DefList,k_nchar);$$->child=$1;$1->brother=$2;fill($$);}
    | /*empty*/ {$$=loadingsyntax(e_NULL,k_nchar);$$->child=NULL;fill($$);}

    ;
Def : Specifier DecList SEMI {$$=loadingsyntax(e_Def,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Specifier error SEMI {} //add Specifier or not, i donnot know,that is a question
    //| error  DecList SEMI {}
    //| error SEMI {}
    ;
DecList : Dec {$$=loadingsyntax(e_DecList,k_nchar);$$->child=$1;fill($$);}
    | Dec COMMA DecList {$$=loadingsyntax(e_DecList,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}

    ;
Dec : VarDec {$$=loadingsyntax(e_Dec,k_nchar);$$->child=$1;fill($$);}
    | VarDec ASSIGNOP Exp {$$=loadingsyntax(e_Dec,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    ;

/*Eexpressions*/
Exp : Exp ASSIGNOP Exp {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Exp AND Exp {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Exp OR Exp {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Exp RELOP Exp {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Exp PLUS Exp {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Exp MINUS Exp {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Exp STAR Exp {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Exp DIV Exp {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | LP Exp RP {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | MINUS Exp {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;fill($$);}
    | NOT Exp {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;fill($$);}
    | ID LP Args RP {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;;$3->brother=$4;fill($$);}
    | ID LP RP {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Exp LB Exp RB {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;;$3->brother=$4;fill($$);}
    | Exp DOT ID {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | ID {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;fill($$);}
    | INT {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;fill($$);}
    | FLOAT {$$=loadingsyntax(e_Exp,k_nchar);$$->child=$1;fill($$);}
    | Exp LB error RB {}
    //| error RP {}
    //| error RB {} serious bug

    ;
Args : Exp COMMA Args {$$=loadingsyntax(e_Args,k_nchar);$$->child=$1;$1->brother=$2;$2->brother=$3;fill($$);}
    | Exp {$$=loadingsyntax(e_Args,k_nchar);$$->child=$1;fill($$);}
    ;

%%
struct TreeNode* loadingsyntax(enum treename tn,enum treekind tk){
    struct TreeNode* t= (struct TreeNode*)malloc(sizeof(struct TreeNode));
    //t->lineno=lineno; error
    t->int_kind=tk;
    t->int_name=tn;
    switch(tk){
        case k_char:
        case k_int:
        case k_float:
        case k_nchar:strcpy(t->nchar_data,printTreename[tn]); break;
    }
    t->child=NULL;
    t->brother=NULL;
    //check 
    //printf("%s %s \n",printTreename[t->int_name],t->nchar_data);
    return t;
}
void fill(struct TreeNode *t){
    if(t!=NULL &&t->child!=NULL){
        t->lineno=t->child->lineno;
    }
}
/*
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
        char* char_data;
        char nchar_data[40];
    };
    struct TreeNode* child;
    struct TreeNode* brother;
};
*/

void yyerror(char* msg)
{
    iferror=1;
	
    switch(syntaxErrorType){
        //struct {a=1;}
        case 1:fprintf(stderr, "Error type B at line %d: %s,type name requires a specifier or qualifier\n",yylineno,msg);break;
        //struct abc a=1};
        case 2:fprintf(stderr, "Error type B at line %d: %s,missing '(', variable has incomplete type\n",yylineno,msg);break;
        //int [3];
        case 3:fprintf(stderr, "Error type B at line %d: %s,expected identifier or '('\n",yylineno,msg);break;
        //int a[];
        case 4:fprintf(stderr, "Error type B at line %d: %s,definition of variable with array type needs an explicit size or an initializer\n",yylineno,msg);break;
        //int fun(a){}
        case 5:fprintf(stderr, "Error type B at line %d: %s,variable has incomplete type\n",yylineno,msg);break;
        //int fun (a {};
        case 6:fprintf(stderr, "Error type B at line %d: %s,expected ')' to match this '('\n",yylineno,msg);break;
        

        case 7: //abandoned

        case 8:

        case 9:

        case 10:




        //syntaxErrorType==0 other error
        default: fprintf(stderr, "Error type B at line %d: %s\n",yylineno,msg);break;
    }
}
void printTree(struct TreeNode * tree,int depth){
    
    if(tree==NULL){
        return;
    }
    if(tree->int_name==e_Program && tree->child->child==NULL){
        printf("%s (%d)\n",printTreename[tree->int_name],yylineno);
        return;
    }
    if(tree->int_name!=e_NULL){
        int i=0;
        for(;i<depth;i++){
            printf("  ");
        }
        //printf("%s %s",printTreename[tree->int_name],tree->nchar_data);
        
        switch(tree->int_kind){
            case k_char:if(tree->int_name==e_ID||tree->int_name==e_TYPE){
                            printf("%s: %s",printTreename[tree->int_name],tree->nchar_data);
                        }
                        else{
                            printf("%s",printTreename[tree->int_name]);
                        }
                        break;
            case k_int:printf("%s: %d",printTreename[tree->int_name],tree->int_data);break;
            case k_float:printf("%s: %f",printTreename[tree->int_name],tree->float_data);break;
            case k_nchar:printf("%s (%d)",printTreename[tree->int_name],tree->lineno);break;
            default: break;

        }
        printf("\n");
    }
    
    printTree(tree->child,depth+1);
    printTree(tree->brother,depth);
}
