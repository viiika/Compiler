#include<stdio.h>
#include "syntax.tab.h"
extern FILE* yyin;
extern int yylineno;
extern int iferror;
extern void yyrestart(FILE*);
extern struct TreeNode * root;
extern void printTree(struct TreeNode * tree,int depth);
int main(int argc, char** argv) {
    //STEP 1
    /*
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
            } 
        }
    while (yylex() != 0);
    return 0;
    */


    //STEP 2
    ///*
    if (argc <= 1) { return 1;}
    FILE* f=fopen(argv[1],"r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    yylineno=1;
    yyrestart(f);
    yyparse();
    if(!iferror){
        printTree(root,0);
    }
    return 0;
    //*/
   
}